package viewservice

import "net"
import "net/rpc"
import "log"
import "time"
import "sync"
import "fmt"
import "os"
import "sync/atomic"

type ViewServer struct {
	mu       sync.Mutex
	l        net.Listener
	dead     int32 // for testing
	rpccount int32 // for testing
	me       string


	// Your declarations here.
	Servers map[string]*ServerInfo
	primary string
	backup string
	currentViewNum uint
	lastView View

}

type ServerInfo struct {
	lastPinged time.Time
	isDead bool
	lastAcked uint
}
//
// server Ping RPC handler.
//
func (vs *ViewServer) Ping(args *PingArgs, reply *PingReply) error {

	// Your code here.
	//vs.tick()
	vs.mu.Lock()
	if vs.primary == "" && vs.currentViewNum == 0 {
		vs.primary = args.Me
		vs.Servers[vs.primary] = &ServerInfo{}
		vs.Servers[vs.primary].lastAcked = 0

		vs.lastView.Primary = vs.primary
		vs.lastView.Viewnum = 0

		vs.currentViewNum = 1
	} else if vs.primary == args.Me {
		if args.Viewnum == vs.currentViewNum {
			vs.Servers[vs.primary].lastAcked = vs.currentViewNum
			vs.lastView.Viewnum = vs.currentViewNum
			vs.currentViewNum += 1
		} else if (args.Viewnum == 0) {
			temp := vs.primary
			vs.primary = vs.backup
			vs.backup = temp

			vs.Servers[vs.primary].lastAcked = vs.currentViewNum
			vs.Servers[vs.backup].lastAcked = vs.currentViewNum

			vs.lastView.Primary = vs.primary
			vs.lastView.Backup = vs.backup
			vs.lastView.Viewnum = vs.currentViewNum

			vs.currentViewNum += 1
		}
	}

	if vs.backup == "" && vs.primary != "" && vs.primary != args.Me && args.Viewnum <= vs.lastView.Viewnum {
		vs.backup = args.Me 
		vs.lastView.Backup = vs.backup

		vs.Servers[vs.backup] = &ServerInfo{}
		vs.Servers[vs.backup].lastAcked = args.Viewnum
	}


	if vs.Servers[args.Me] == nil {
		vs.Servers[args.Me] = &ServerInfo{}
	}

	if vs.lastView.Viewnum == args.Viewnum {
		vs.Servers[args.Me].lastAcked =  args.Viewnum
	} 


	reply.View.Viewnum = vs.currentViewNum
	vs.Servers[args.Me].lastPinged = time.Now()
	reply.View.Primary = vs.primary
	reply.View.Backup = vs.backup
	vs.mu.Unlock()
	return nil
}

//
// server Get() RPC handler.
//
func (vs *ViewServer) Get(args *GetArgs, reply *GetReply) error {

	// Your code here.
	vs.mu.Lock()
	reply.View.Viewnum = vs.currentViewNum
	reply.View.Primary = vs.primary
	reply.View.Backup = vs.backup
	vs.mu.Unlock()
	return nil
}


//
// tick() is called once per PingInterval; it should notice
// if servers have died or recovered, and change the view
// accordingly.
//
func (vs *ViewServer) tick() {

	// Your code here.
	vs.mu.Lock()
	for _ , server := range vs.Servers {
		if time.Since(server.lastPinged).Milliseconds() >= DeadPings*PingInterval.Milliseconds() {
			server.isDead = true
		} else  {
			server.isDead = false
		}
	}

 	if vs.backup != "" && vs.Servers[vs.backup].isDead {
 		vs.backup = ""
 		vs.lastView.Backup = ""
 	}

 	if vs.primary != "" && vs.Servers[vs.primary].isDead && vs.backup != "" {
		if vs.Servers[vs.backup].lastAcked == vs.lastView.Viewnum  {
			vs.primary = vs.backup
			vs.backup = ""

			vs.lastView.Primary = vs.primary
			vs.lastView.Backup = ""
			vs.lastView.Viewnum = vs.currentViewNum
			vs.currentViewNum += 1
		}
	} 
	vs.mu.Unlock()
}

//
// tell the server to shut itself down.
// for testing.
// please don't change these two functions.
//
func (vs *ViewServer) Kill() {
	atomic.StoreInt32(&vs.dead, 1)
	vs.l.Close()
}

//
// has this server been asked to shut down?
//
func (vs *ViewServer) isdead() bool {
	return atomic.LoadInt32(&vs.dead) != 0
}

// please don't change this function.
func (vs *ViewServer) GetRPCCount() int32 {
	return atomic.LoadInt32(&vs.rpccount)
}

func StartServer(me string) *ViewServer {
	vs := new(ViewServer)
	vs.me = me
	// Your vs.* initializations here.
	vs.Servers = make(map[string]*ServerInfo)
	// tell net/rpc about our RPC server and handlers.
	rpcs := rpc.NewServer()
	rpcs.Register(vs)

	// prepare to receive connections from clients.
	// change "unix" to "tcp" to use over a network.
	os.Remove(vs.me) // only needed for "unix"
	l, e := net.Listen("unix", vs.me)
	if e != nil {
		log.Fatal("listen error: ", e)
	}
	vs.l = l

	// please don't change any of the following code,
	// or do anything to subvert it.

	// create a thread to accept RPC connections from clients.
	go func() {
		for vs.isdead() == false {
			conn, err := vs.l.Accept()
			if err == nil && vs.isdead() == false {
				atomic.AddInt32(&vs.rpccount, 1)
				go rpcs.ServeConn(conn)
			} else if err == nil {
				conn.Close()
			}
			if err != nil && vs.isdead() == false {
				fmt.Printf("ViewServer(%v) accept: %v\n", me, err.Error())
				vs.Kill()
			}
		}
	}()

	// create a thread to call tick() periodically.
	go func() {
		for vs.isdead() == false {
			vs.tick()
			time.Sleep(PingInterval)
		}
	}()

	return vs
}
