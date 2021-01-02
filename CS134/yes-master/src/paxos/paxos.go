package paxos

//
// Paxos library, to be included in an application.
// Multiple applications will run, each including
// a Paxos peer.
//
// Manages a sequence of agreed-on values.
// The set of peers is fixed.
// Copes with network failures (partition, msg loss, &c).
// Does not store anything persistently, so cannot handle crash+restart.
//
// The application interface:
//
// px = paxos.Make(peers []string, me string)
// px.Start(seq int, v interface{}) -- start agreement on new instance
// px.Status(seq int) (Fate, v interface{}) -- get info about an instance
// px.Done(seq int) -- ok to forget all instances <= seq
// px.Max() int -- highest instance seq known, or -1
// px.Min() int -- instances before this seq have been forgotten
//

import "net"
import "net/rpc"
import "log"
import "time"
import "os"
import "syscall"
import "sync"
import "sync/atomic"
import "fmt"
import "math/rand"

const PingInterval = time.Millisecond * 100

// px.Status() return values, indicating
// whether an agreement has been decided,
// or Paxos has not yet reached agreement,
// or it was agreed but forgotten (i.e. < Min()).
type Fate int

const (
	Decided   Fate = iota + 1
	Pending        // not yet decided.
	Forgotten      // decided but forgotten.
)


type Paxos struct {
	mu         sync.Mutex
	l          net.Listener
	dead       int32 // for testing
	unreliable int32 // for testing
	rpcCount   int32 // for testing
	peers      []string
	me         int // index into peers[]

	
	values 	   map[int]*Acceptor
	minDone    []int
	actMin	   int
	seqMin	   int



	// Your data here.
}

type Acceptor struct {
	N_a 	int
	N_p 	int
	Status	Fate
	V_a 	interface{}
}

type PrepareArgs struct {
	N 		int
	Seq 	int
}

type PrepareReply struct {
	N 		int
	N_a 	int
	V 		interface{}
	Promise	bool
}

type AcceptArgs struct {
	N 		int
	V 		interface{}
	Seq 	int
}

type AcceptReply struct {
	N 		int
	Accept 	bool
}

type DecidedArgs struct {
	V 		interface{}
	Seq 	int
}

type DecidedReply struct {}

type DoneArgs struct {
	Seq 	int
	Peer 	string
}

type DoneReply struct {
}
//
// call() sends an RPC to the rpcname handler on server srv
// with arguments args, waits for the reply, and leaves the
// reply in reply. the reply argument should be a pointer
// to a reply structure.
//
// the return value is true if the server responded, and false
// if call() was not able to contact the server. in particular,
// the replys contents are only valid if call() returned true.
//
// you should assume that call() will time out and return an
// error after a while if it does not get a reply from the server.
//
// please use call() to send all RPCs, in client.go and server.go.
// please do not change this function.
//
func call(srv string, name string, args interface{}, reply interface{}) bool {
	c, err := rpc.Dial("unix", srv)
	if err != nil {
		err1 := err.(*net.OpError)
		if err1.Err != syscall.ENOENT && err1.Err != syscall.ECONNREFUSED {
			fmt.Printf("paxos Dial() failed: %v\n", err1)
		}
		return false
	}
	defer c.Close()

	err = c.Call(name, args, reply)
	if err == nil {
		return true
	}

	fmt.Println(err)
	return false
}


// Proposer function
func (px *Paxos) Proposer(seq int, V interface{}) {
	decided := false
	largestN := 1
	amountPromised := 0
	for !decided {
		amountPromised = 0
		largestWeHaveSeen := largestN
		largestPromised := -1
		v_a := V

		if px.isdead() {
			return
		}

		for index, peer := range px.peers {
			args := &PrepareArgs{}
			var reply PrepareReply
			args.N = largestN
			args.Seq = seq
			var ok bool
			if index == px.me {
				okerr := px.Prepare(args, &reply)
				if okerr == nil{
					ok = true
				}
			} else {
				ok = call(peer, "Paxos.Prepare", args, &reply)
			}
			if ok {
				if reply.Promise {
					amountPromised += 1
					
					if reply.V != nil {
						if largestPromised < reply.N_a {
							largestPromised = reply.N_a
							v_a = reply.V
						}
					}
				} else if largestWeHaveSeen < reply.N_a {
					largestWeHaveSeen = reply.N_a
				}
			}
		}


		if len(px.peers)/2 >= amountPromised {
			if largestN < largestWeHaveSeen {
				largestN = largestWeHaveSeen + 1
			} 
			time.Sleep(PingInterval)
			continue
		} 

		// we can use largestN or largestPromised
		N := largestN

		// send accept woot woot
		amountAccept := 0
		largest := N
		for index, peer := range px.peers {
			args := &AcceptArgs{}
			args.N = N
			args.V = v_a
			args.Seq = seq
			var reply AcceptReply

			var ok bool
			if index == px.me {
				okerr := px.Accept(args, &reply)
				if okerr == nil {
					ok = true
				}
			} else {
				ok = call(peer, "Paxos.Accept", args, &reply)
			}
			if ok {
				if reply.Accept {
					amountAccept += 1
				}
				if largest < reply.N {
					largest = reply.N
				}
			}
		}


		if len(px.peers)/2 >= amountAccept {
			if largestN < largest {
				largestN = largest + 1
			}
			continue
			time.Sleep(PingInterval)
		}

		if px.isdead() {
			return
		}
		// send decided :)
		decided = true
		for index, peer := range px.peers {
			args := &DecidedArgs{}
			args.Seq = seq
			args.V = v_a

			var reply DecidedReply

			if index == px.me {
				px.Decide(args, &reply)
			} else {
				ok := call(peer, "Paxos.Decide", args, &reply)
				i := 1
				for !ok && i < 3 {
					ok = call(peer, "Paxos.Decide", args, &reply)
					i+=1
				}
			}

		}


	}
}


// rpc functions
func (px *Paxos) Prepare(args *PrepareArgs, reply *PrepareReply) error{
	px.mu.Lock()
	value, ok := px.values[args.Seq]
	if !ok && px.actMin <= args.Seq{
		px.values[args.Seq] = &Acceptor{}
		px.values[args.Seq].N_p = args.N
		px.values[args.Seq].Status = Pending
		px.values[args.Seq].V_a = nil
		reply.N = args.N
		reply.V = nil
		reply.Promise = true

		px.mu.Unlock()
		return nil
	}

	if px.actMin <= args.Seq && args.N > value.N_p {
		value.N_p = args.N
		if value.V_a == nil {
			reply.V = nil
		} else {
			reply.N_a = value.N_a
			reply.V = value.V_a
		}
		reply.N = args.N
		reply.Promise = true

	} else {
		reply.Promise = false
		if px.actMin <= args.Seq {
			reply.N_a = value.N_p+1
		}
	}
	px.mu.Unlock()
	return nil
}


func (px *Paxos) Accept(args *AcceptArgs, reply *AcceptReply) error{
	px.mu.Lock()
	value, ok := px.values[args.Seq]
	if !ok && px.actMin <= args.Seq{
		px.values[args.Seq] = &Acceptor{}
		px.values[args.Seq].N_p = args.N
		px.values[args.Seq].Status = Pending
		px.values[args.Seq].N_a = args.N
		px.values[args.Seq].V_a = args.V
		reply.N = args.N
		reply.Accept = true
	} else {
		if px.actMin <= args.Seq && args.N >= value.N_p {
			value.N_p = args.N
			value.N_a = args.N
			value.V_a = args.V
			reply.N = args.N
			reply.Accept = true
		} else {
			if px.actMin <= args.Seq {
				reply.N = value.N_p
			}
			reply.Accept = false
		}
	}
	px.mu.Unlock()
	return nil

}


func (px *Paxos) Decide(args *DecidedArgs, reply *DecidedReply) error {
	px.mu.Lock()
	value, ok := px.values[args.Seq]
	if !ok && px.actMin <= args.Seq{
		px.values[args.Seq] = &Acceptor{}
		px.values[args.Seq].Status = Decided
		px.values[args.Seq].V_a = args.V
	} else if px.actMin <= args.Seq {
		value.Status = Decided
		value.V_a = args.V
	}
	px.mu.Unlock()
	return nil
}
//
// the application wants paxos to start agreement on
// instance seq, with proposed value v.
// Start() returns right away; the application will
// call Status() to find out if/when agreement
// is reached.
//
func (px *Paxos) Start(seq int, v interface{}) {
	// Your code here.
	if px.isdead() {
		return
	}
	go func() {
		px.Proposer(seq, v)
	}()

}

//
// the application on this machine is done with
// all instances <= seq.
//
// see the comments for Min() for more explanation.
//
func (px *Paxos) Done(seq int) {
	// Your code here.
	if px.minDone[px.me] >= seq {
		return
	}
	px.minDone[px.me] = seq
	for index, val := range px.peers {
		args := &DoneArgs{}
		args.Seq = seq
		args.Peer = px.peers[px.me]
		var reply DoneReply
		if index == px.me {
			continue
		}
		call(val, "Paxos.SendDone", args, &reply)
	}

	px.mu.Lock()
	min := px.minDone[0]
	for _, v := range px.minDone {
		if v < min {
			min = v
		}
	}

	for index, _ := range px.values {
		if index < min+1 {
			// delete
			px.values[index].V_a = nil

			px.values[index] = nil
			delete(px.values, index)
		}
	}
	px.mu.Unlock()
}


func (px *Paxos) SendDone(args *DoneArgs, reply *DoneReply) error{
	for index, val  := range px.peers {
		if val == args.Peer && px.minDone[index] < args.Seq{
			px.minDone[index] = args.Seq
			break
		} else if val == args.Peer && px.minDone[index] >= args.Seq {
			return nil
		}
	}	
	px.mu.Lock()
	min := px.minDone[0]
	for _, v := range px.minDone {
		if v < min {
			min = v
		}
	}

	for index, _ := range px.values {
		if index < min+1 {
			// delete
			px.values[index].V_a = nil

			px.values[index] = nil
			delete(px.values, index)
		}
	}
	px.mu.Unlock()
	return nil
}

//
// the application wants to know the
// highest instance sequence known to
// this peer.
//
func (px *Paxos) Max() int {
	// Your code here.
	px.mu.Lock()
	maxSeq := -1
	for k, v := range px.values {
		if v.Status == Pending || v.Status == Decided {
			if maxSeq < k {
				maxSeq = k
			}
		}
	}
	if maxSeq < px.actMin {
		maxSeq = px.actMin-1
	}

	px.mu.Unlock()
	return maxSeq
}

//
// Min() should return one more than the minimum among z_i,
// where z_i is the highest number ever passed
// to Done() on peer i. A peers z_i is -1 if it has
// never called Done().
//
// Paxos is required to have forgotten all information
// about any instances it knows that are < Min().
// The point is to free up memory in long-running
// Paxos-based servers.
//
// Paxos peers need to exchange their highest Done()
// arguments in order to implement Min(). These
// exchanges can be piggybacked on ordinary Paxos
// agreement protocol messages, so it is OK if one
// peers Min does not reflect another Peers Done()
// until after the next instance is agreed to.
//
// The fact that Min() is defined as a minimum over
// *all* Paxos peers means that Min() cannot increase until
// all peers have been heard from. So if a peer is dead
// or unreachable, other peers Min()s will not increase
// even if all reachable peers call Done. The reason for
// this is that when the unreachable peer comes back to
// life, it will need to catch up on instances that it
// missed -- the other peers therefor cannot forget these
// instances.
//
func (px *Paxos) Min() int {
	// You code here.
	px.mu.Lock()
	retVal := px.minDone[0]
	for _, v := range px.minDone {
		if v < retVal {
			retVal = v
		}
	}
	px.mu.Unlock()
	return retVal+1
}

//
// the application wants to know whether this
// peer thinks an instance has been decided,
// and if so what the agreed value is. Status()
// should just inspect the local peer state;
// it should not contact other Paxos peers.
//
func (px *Paxos) Status(seq int) (Fate, interface{}) {
	// Your code here.
	px.mu.Lock()

	min := px.minDone[0]
	for _, v := range px.minDone {
		if v < min {
			min = v
		}
	}

	if seq < min {
		px.mu.Unlock()
		return Forgotten, nil
	}

	val, ok := px.values[seq]
	if ok {
		status := val.Status
		value  := val.V_a
		px.mu.Unlock()
		return status, value
	}
	px.mu.Unlock()
	return Forgotten, nil
}



//
// tell the peer to shut itself down.
// for testing.
// please do not change these two functions.
//
func (px *Paxos) Kill() {
	atomic.StoreInt32(&px.dead, 1)
	if px.l != nil {
		px.l.Close()
	}
}

//
// has this peer been asked to shut down?
//
func (px *Paxos) isdead() bool {
	return atomic.LoadInt32(&px.dead) != 0
}

// please do not change these two functions.
func (px *Paxos) setunreliable(what bool) {
	if what {
		atomic.StoreInt32(&px.unreliable, 1)
	} else {
		atomic.StoreInt32(&px.unreliable, 0)
	}
}

func (px *Paxos) isunreliable() bool {
	return atomic.LoadInt32(&px.unreliable) != 0
}

//
// the application wants to create a paxos peer.
// the ports of all the paxos peers (including this one)
// are in peers[]. this servers port is peers[me].
//
func Make(peers []string, me int, rpcs *rpc.Server) *Paxos {
	px := &Paxos{}
	px.peers = peers
	px.me = me


	// Your initialization code here.
	px.values = make(map[int]*Acceptor)
	px.minDone = make([]int, len(peers))

	for index, _ := range px.minDone {
		px.minDone[index] = -1
	}

	if rpcs != nil {
		// caller will create socket &c
		rpcs.Register(px)
	} else {
		rpcs = rpc.NewServer()
		rpcs.Register(px)

		// prepare to receive connections from clients.
		// change "unix" to "tcp" to use over a network.
		os.Remove(peers[me]) // only needed for "unix"
		l, e := net.Listen("unix", peers[me])
		if e != nil {
			log.Fatal("listen error: ", e)
		}
		px.l = l

		// please do not change any of the following code,
		// or do anything to subvert it.

		// create a thread to accept RPC connections
		go func() {
			for px.isdead() == false {
				conn, err := px.l.Accept()
				if err == nil && px.isdead() == false {
					if px.isunreliable() && (rand.Int63()%1000) < 100 {
						// discard the request.
						conn.Close()
					} else if px.isunreliable() && (rand.Int63()%1000) < 200 {
						// process the request but force discard of reply.
						c1 := conn.(*net.UnixConn)
						f, _ := c1.File()
						err := syscall.Shutdown(int(f.Fd()), syscall.SHUT_WR)
						if err != nil {
							fmt.Printf("shutdown: %v\n", err)
						}
						atomic.AddInt32(&px.rpcCount, 1)
						go rpcs.ServeConn(conn)
					} else {
						atomic.AddInt32(&px.rpcCount, 1)
						go rpcs.ServeConn(conn)
					}
				} else if err == nil {
					conn.Close()
				}
				if err != nil && px.isdead() == false {
					fmt.Printf("Paxos(%v) accept: %v\n", me, err.Error())
				}
			}
		}()
	}


	return px
}