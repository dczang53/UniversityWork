package kvpaxos

import "net"
import "fmt"
import "net/rpc"
import "log"
import "paxos"
import "sync"
import "sync/atomic"
import "os"
import "syscall"
import "encoding/gob"
import "math/rand"
import "time"


const Debug = 0

func DPrintf(format string, a ...interface{}) (n int, err error) {
	if Debug > 0 {
		log.Printf(format, a...)
	}
	return
}


type Op struct {
	// Your definitions here.
	// Field names must start with capital letters,
	// otherwise RPC will break.
	Key 	string
	Value   string
	Op		string
	Id      int64
}

type KVPaxos struct {
	mu         sync.Mutex
	l          net.Listener
	me         int
	dead       int32 // for testing
	unreliable int32 // for testing
	px         *paxos.Paxos

	// Your definitions here.
	data		 map[string]string
	requests	 map[int64]Op

	lastExecuted int
}


func (kv *KVPaxos) Get(args *GetArgs, reply *GetReply) error {
	// Your code here.
	kv.mu.Lock()
	if kv.isdead() {
		reply.Err = ErrDead
		kv.mu.Unlock()
		return nil
	}
	var op Op
	op.Op = "Get"
	op.Key = args.Key
	op.Id = args.Id
	for {
		if kv.isdead() {
			reply.Err = ErrDead
			kv.mu.Unlock()
			return nil
		}
		//fmt.Printf("Trying to Get at %d %d OP: %s , KEY: %s , VALUE: %s %d\n", kv.lastExecuted, kv.me, op.Op, op.Key, op.Value, op.Id)
		kv.px.Start(kv.lastExecuted, op)
		to := 10 * time.Millisecond
		time.Sleep(to)
    	for {
    		if kv.isdead() {
				reply.Err = ErrDead
				kv.mu.Unlock()
				return nil
			}
      		status, val := kv.px.Status(kv.lastExecuted)
        	if status == paxos.Decided{
        		re, in := kv.requests[val.(Op).Id]
        		// fmt.Printf("IN GET: %d %d OP: %s , KEY: %s , VALUE: %s %d\n", kv.lastExecuted, kv.me, val.(Op).Op, val.(Op).Key, val.(Op).Value, val.(Op).Id)
            	if val != op {
            		if !in || re != val{
            			kv.applyVal(val.(Op))
            		}
            		kv.lastExecuted += 1
            		break
           		} else {
            		v, ok := kv.data[op.Key]
            		if !ok {
            			reply.Err = ErrNoKey
            		} else {
            			reply.Value = v
            		}
            		//kv.px.Done(kv.lastExecuted-2)
            		kv.lastExecuted += 1
            		kv.mu.Unlock()
            		return nil
            	}
        	} 

        	time.Sleep(to)
        	if to < 10 * time.Second {
            	to *= 2
        	}
    	}
    }
}

func (kv *KVPaxos) PutAppend(args *PutAppendArgs, reply *PutAppendReply) error {
	// Your code here.
	kv.mu.Lock()
	if kv.isdead() {
		reply.Err = ErrDead
		kv.mu.Unlock()
		return nil
	}
	var op Op
	op.Op = args.Op
	op.Key = args.Key
	op.Value = args.Value
	op.Id = args.Id
	for {
		if kv.isdead() {
			reply.Err = ErrDead
			kv.mu.Unlock()
			return nil
		}
		//fmt.Printf("WE Starting %d\n", kv.lastExecuted)
		//fmt.Printf("Trying to PUT AT at %d %d OP: %s , KEY: %s , VALUE: %s %d\n", kv.lastExecuted, kv.me, op.Op, op.Key, op.Value, op.Id)
		kv.px.Start(kv.lastExecuted, op)
		to := 10 * time.Millisecond
		time.Sleep(to)
    	for {
    		if kv.isdead() {
				reply.Err = ErrDead
				kv.mu.Unlock()
				return nil
			}
      		status, val := kv.px.Status(kv.lastExecuted)
        	if status == paxos.Decided{
        		re, in := kv.requests[val.(Op).Id]

            	if val != op {
            		if !in || re != val{
            			kv.applyVal(val.(Op))
            		}
            		kv.lastExecuted += 1
            		break
           		} else {
           			if !in || re != op {
            			kv.applyVal(op)
            		}
            		//kv.px.Done(kv.lastExecuted-2)
            		kv.lastExecuted += 1
            		kv.mu.Unlock()
					return nil
            	}
        	} 
        	time.Sleep(to)
        	if to < 10 * time.Second {
            	to *= 2
        	}
    	}
    }
}

func (kv *KVPaxos) applyVal(op Op) {
	//kv.mu.Lock()
	if op.Op == "Get" {
		return
	}
	_, ok := kv.data[op.Key]
	kv.requests[op.Id] = op
	if op.Op == "Put" || !ok {
		kv.data[op.Key] = op.Value
	} else if (op.Op == "Append"){
		kv.data[op.Key] += op.Value
	}
	//kv.mu.Unlock()
}

// tell the server to shut itself down.
// please do not change these two functions.
func (kv *KVPaxos) kill() {
	DPrintf("Kill(%d): die\n", kv.me)
	atomic.StoreInt32(&kv.dead, 1)
	kv.l.Close()
	kv.px.Kill()
}

// call this to find out if the server is dead.
func (kv *KVPaxos) isdead() bool {
	return atomic.LoadInt32(&kv.dead) != 0
}

// please do not change these two functions.
func (kv *KVPaxos) setunreliable(what bool) {
	if what {
		atomic.StoreInt32(&kv.unreliable, 1)
	} else {
		atomic.StoreInt32(&kv.unreliable, 0)
	}
}

func (kv *KVPaxos) isunreliable() bool {
	return atomic.LoadInt32(&kv.unreliable) != 0
}

//
// servers[] contains the ports of the set of
// servers that will cooperate via Paxos to
// form the fault-tolerant key/value service.
// me is the index of the current server in servers[].
//
func StartServer(servers []string, me int) *KVPaxos {
	// call gob.Register on structures you want
	// Go's RPC library to marshall/unmarshall.
	gob.Register(Op{})

	kv := new(KVPaxos)
	kv.me = me

	kv.data = make(map[string]string)
	kv.requests = make(map[int64]Op)
	// Your initialization code here.

	rpcs := rpc.NewServer()
	rpcs.Register(kv)

	kv.px = paxos.Make(servers, me, rpcs)

	os.Remove(servers[me])
	l, e := net.Listen("unix", servers[me])
	if e != nil {
		log.Fatal("listen error: ", e)
	}
	kv.l = l


	// please do not change any of the following code,
	// or do anything to subvert it.

	go func() {
		for kv.isdead() == false {
			conn, err := kv.l.Accept()
			if err == nil && kv.isdead() == false {
				if kv.isunreliable() && (rand.Int63()%1000) < 100 {
					// discard the request.
					conn.Close()
				} else if kv.isunreliable() && (rand.Int63()%1000) < 200 {
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
			if err != nil && kv.isdead() == false {
				fmt.Printf("KVPaxos(%v) accept: %v\n", me, err.Error())
				kv.kill()
			}
		}
	}()

	return kv
}