package pbservice

import "net"
import "fmt"
import "net/rpc"
import "log"
import "time"
import "viewservice"
import "sync"
import "sync/atomic"
import "container/list"
import "os"
import "syscall"
import "math/rand"



type PBServer struct {
	mu         sync.Mutex
	l          net.Listener
	dead       int32 // for testing
	unreliable int32 // for testing
	me         string
	vs         *viewservice.Clerk
	// Your declarations here.
	vshost 		string 
	currView 	viewservice.View
	data 		map[string]string 
	requests 	map[int64]*PutAppendArgs
	//toProcess	[]int64
	requestOrdering *list.List
	ping 		uint
	isUpdating  bool
	updateMu	sync.Mutex
	vsIsDead 	bool
}

/*
//single instance of goroutine -----------------------------
var (
    hProcessRunning	bool = false
    hProcessMu		sync.Mutex
    pb				*PBServer
)

func setHProcess(pbNew *PBServer) {
	hProcessMu.Lock()
	shouldStart := !hProcessRunning
	jobIsRunning = true
	pb = pbNew
	hProcessgMu.Unlock()
	if shouldStart {
		//processses putAppends in heap continuously
		go func() {
				for {
					
				}
			}()
	}
}
//----------------------------------------------------------
*/

func (pb *PBServer) Get(args *GetArgs, reply *GetReply) error {

	// Your code here.
	//shouldn't run unless requests is empty
	pb.mu.Lock()
	//time.Sleep(viewservice.PingInterval)
	if pb.isdead() {
		reply.Err = ErrWrongServer
		pb.mu.Unlock()
		return nil
	}
	//concurrency protection needed
	if pb.me != pb.currView.Primary {
		reply.Err = ErrWrongServer
		pb.mu.Unlock()
		return nil
	}
	value, ok := pb.data[args.Key]
	if !ok {
		reply.Err = ErrNoKey
		pb.mu.Unlock()
		return nil
	} 
	reply.Err = OK
	reply.Value = value
	pb.mu.Unlock()

	return nil
}


func (pb *PBServer) PutAppend(args *PutAppendArgs, reply *PutAppendReply) error {

	// Your code here.
	//time.Sleep(viewservice.PingInterval)
	pb.mu.Lock()
	if pb.isdead() {
		reply.Err = ErrWrongServer
		pb.mu.Unlock()
		return nil
	}

	if pb.me != pb.currView.Primary {
		reply.Err = ErrWrongServer
		pb.mu.Unlock()
		return nil
	}
	var backupreply PutAppendReply
	oldArgs , exists := pb.requests[args.Id]
	_ , ok := pb.data[args.Key]
	if !ok || args.Type == "Put" {
		if !exists || oldArgs.Value != args.Value || oldArgs.Key != args.Key {
			pb.data[args.Key] = args.Value
			if pb.currView.Backup != "" {
				///*
				//pb.mu.Unlock()
				//pb.updateMu.Lock()
				ok := call(pb.currView.Backup, "PBServer.PutAppendFromPrimary", args, &backupreply)
				if !ok {
					t := time.Now()
					for !ok && pb.currView.Backup != "" && !pb.isdead() && pb.currView.Primary == pb.me && backupreply.Err != ErrWrongServer{
						ok = call(pb.currView.Backup, "PBServer.PutAppendFromPrimary", args, &backupreply)
						if time.Since(t).Milliseconds() < viewservice.PingInterval.Milliseconds(){
							getargs := &viewservice.GetArgs{}
							var getreply viewservice.GetReply
							get := call(pb.vshost, "ViewServer.Get", getargs, &getreply)
							if get {
								pb.currView = getreply.View
							}
						} else {
							pingArgs := &viewservice.PingArgs{}
							pingArgs.Me = pb.me
							pingArgs.Viewnum = pb.currView.Viewnum-1
							var pingreply viewservice.PingReply
							get := call(pb.vshost, "ViewServer.Ping", pingArgs, &pingreply)
							if get {
								pb.currView = pingreply.View
							}
							t = time.Now()
						}
						
					}
				}
				//pb.updateMu.Unlock()
				//pb.mu.Lock()
				//*/
			}
			pb.requests[args.Id] = args
		}
		reply.Err = OK
	}  else {
		if !exists || oldArgs.Value != args.Value || oldArgs.Key != args.Key {
			pb.data[args.Key]+=args.Value
			if pb.currView.Backup != "" {
				///*
				//pb.mu.Unlock()
				//pb.updateMu.Lock()
				ok := call(pb.currView.Backup, "PBServer.PutAppendFromPrimary", args, &backupreply)
				if !ok {
					t := time.Now()
					for !ok && pb.currView.Backup != "" && !pb.isdead() && pb.currView.Primary == pb.me && backupreply.Err != ErrWrongServer{
						ok = call(pb.currView.Backup, "PBServer.PutAppendFromPrimary", args, &backupreply)
						if time.Since(t).Milliseconds() < viewservice.PingInterval.Milliseconds() {
							getargs := &viewservice.GetArgs{}
							var getreply viewservice.GetReply
							get := call(pb.vshost, "ViewServer.Get", getargs, &getreply)
							if get {
								pb.currView = getreply.View
							}
						} else {
							pingArgs := &viewservice.PingArgs{}
							pingArgs.Me = pb.me
							pingArgs.Viewnum = pb.currView.Viewnum-1
							var pingreply viewservice.PingReply
							get := call(pb.vshost, "ViewServer.Ping", pingArgs, &pingreply)
							if get {
								pb.currView = pingreply.View
							}
							t = time.Now()
						}
					}
				}
				//pb.updateMu.Unlock()
				//pb.mu.Lock()
				//*/
			}
			pb.requests[args.Id] = args
		}
		reply.Err = OK
	}

	pb.mu.Unlock()
	return nil
}

func (pb *PBServer) PutAppendFromPrimary(args *PutAppendArgs, reply *PutAppendReply) error {
	pb.mu.Lock()
	if args.Primary == pb.currView.Primary {
		oldArgs , exists := pb.requests[args.Id]
		_, ok := pb.data[args.Key]
		if args.Type == "Put" || !ok && (!exists || oldArgs.Value != args.Value || oldArgs.Key != args.Key) {
			pb.data[args.Key] = args.Value
		} else if !exists || oldArgs.Value != args.Value || oldArgs.Key != args.Key  {
			pb.data[args.Key]+=args.Value
			
		}
		pb.requests[args.Id] = args
		reply.Err = OK
		pb.mu.Unlock()
		return nil
	}

	reply.Err = ErrWrongServer
	pb.mu.Unlock()
	return nil
}

func (pb *PBServer) GetData(args *SendDataArgs, reply *SendDataReply) error {
	pb.mu.Lock()
	if args.Primary == pb.currView.Primary {
		pb.data = args.Data
		reply.Err = OK
		pb.mu.Unlock()
		return nil
	} else {
		reply.Err = ErrWrongServer
		pb.mu.Unlock()
		return nil
	}
}

//
// ping the viewserver periodically.
// if view changed:
//   transition to new view.
//   manage transfer of state from primary to new backup.
//
func (pb *PBServer) tick() {

	// Your code here.
	pb.mu.Lock()
	if (pb.isdead()) {
		return 
	}

	pingArgs := &viewservice.PingArgs{}
	pingArgs.Me = pb.me
	// if pb.me == pb.currView.Primary {
	pingArgs.Viewnum = pb.ping
	// } else {

	// }
	var reply viewservice.PingReply

	// if pb.isUpdating {
	// 	pingArgs.Viewnum = pb.currView.Viewnum-1
	// }


	ok := call(pb.vshost, "ViewServer.Ping", pingArgs, &reply)
	// i := 1
	for !ok {
		// if i % 2 == 0 {
		// 	return
		// }
		ok = call(pb.vshost, "ViewServer.Ping", pingArgs, &reply)
		// i++
	}

	pb.ping+=1

	if pb.currView.Backup != reply.View.Backup && pb.me == reply.View.Primary && reply.View.Backup != "" {
		// rpc call to send data to newbackup
		getArgs := &SendDataArgs{}
		getArgs.Primary = pb.me
		getArgs.Backup = reply.View.Backup
		getArgs.Data = pb.data

		var getReply SendDataReply
		get := call(reply.View.Backup, "PBServer.GetData", getArgs, &getReply)
		// i = 1
		for !get {
			// if i % 2 == 0 {
			// 	break
			// }
			get = call(reply.View.Backup, "PBServer.GetData", getArgs, &getReply)
			// i++
		}
	}

	if pb.me != pb.currView.Primary && pb.me == pb.currView.Backup && pb.ping <= reply.View.Viewnum  {
		pb.ping = reply.View.Viewnum-1
	} else {
		pb.ping = reply.View.Viewnum
	}

	//pb.updateMu.Lock()
	pb.currView.Primary = reply.View.Primary
	pb.currView.Backup = reply.View.Backup
	pb.currView.Viewnum = reply.View.Viewnum
	//pb.updateMu.Unlock()

	pb.mu.Unlock()
}

// tell the server to shut itself down.
// please do not change these two functions.
func (pb *PBServer) kill() {
	atomic.StoreInt32(&pb.dead, 1)
	pb.l.Close()
}

// call this to find out if the server is dead.
func (pb *PBServer) isdead() bool {
	return atomic.LoadInt32(&pb.dead) != 0
}

// please do not change these two functions.
func (pb *PBServer) setunreliable(what bool) {
	if what {
		atomic.StoreInt32(&pb.unreliable, 1)
	} else {
		atomic.StoreInt32(&pb.unreliable, 0)
	}
}

func (pb *PBServer) isunreliable() bool {
	return atomic.LoadInt32(&pb.unreliable) != 0
}


func StartServer(vshost string, me string) *PBServer {
	pb := new(PBServer)
	pb.me = me
	pb.vs = viewservice.MakeClerk(me, vshost)
	// Your pb.* initializations here.
	pb.data = make(map[string]string)
	pb.requests = make(map[int64]*PutAppendArgs)
	pb.requestOrdering = list.New()
	//pb.currentOrdered = make(map[int]*PutAppendUpdateArgs)
	pb.vshost = vshost

	rpcs := rpc.NewServer()
	rpcs.Register(pb)

	os.Remove(pb.me)
	l, e := net.Listen("unix", pb.me)
	if e != nil {
		log.Fatal("listen error: ", e)
	}
	pb.l = l

	// please do not change any of the following code,
	// or do anything to subvert it.

	go func() {
		for pb.isdead() == false {
			conn, err := pb.l.Accept()
			if err == nil && pb.isdead() == false {
				if pb.isunreliable() && (rand.Int63()%1000) < 100 {
					// discard the request.
					conn.Close()
				} else if pb.isunreliable() && (rand.Int63()%1000) < 200 {
					// process the request but force discard of reply.
					c1 := conn.(*net.UnixConn)
					f, _ := c1.File()
					err := syscall.Shutdown(int(f.Fd()), syscall.SHUT_WR)
					if err != nil {
						fmt.Printf("shutdown: %v\n", err)
					}
					go rpcs.ServeConn(conn)
				} else {
					go rpcs.ServeConn(conn)
				}
			} else if err == nil {
				conn.Close()
			}
			if err != nil && pb.isdead() == false {
				fmt.Printf("PBServer(%v) accept: %v\n", me, err.Error())
				pb.kill()
			}
		}
	}()

	go func() {
		for pb.isdead() == false {
			pb.tick()
			time.Sleep(viewservice.PingInterval)
		}
	}()

	return pb
}

//go test | grep "test_test.go\|FAIL"
