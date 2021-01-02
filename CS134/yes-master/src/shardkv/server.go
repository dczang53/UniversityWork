package shardkv

import "net"
import "fmt"
import "net/rpc"
import "log"
import "time"
import "paxos"
import "sync"
import "sync/atomic"
import "os"
import "syscall"
import "encoding/gob"
import "math/rand"
import "shardmaster"


const Debug = 0

func DPrintf(format string, a ...interface{}) (n int, err error) {
	if Debug > 0 {
		log.Printf(format, a...)
	}
	return
}


type Op struct {
	// Your definitions here.
	OpType string
	Key    string
	Value  string
	Id 	   int64

	// reconfigure stuff elements
	Num int
	gid int64
	shard map[string]string
	state map[int64]Op
	config shardmaster.Config
}


type ShardKV struct {
	mu         sync.Mutex
	l          net.Listener
	me         int
	dead       int32 // for testing
	unreliable int32 // for testing
	sm         *shardmaster.Clerk
	px         *paxos.Paxos

	gid int64 // my replica group ID

	// Your definitions here.
	kvShardData map[string]string
	config 		shardmaster.Config
	pastIds 	map[int64]Op
	lastUpdated int
	received	[shardmaster.NShards]bool
	needsToRec	[shardmaster.NShards]bool
	seqnum 		int
	mapwriter   chan bool
	outdated    bool

}


func (kv *ShardKV) GetNextInstance() interface{} {
    to := 10 * time.Millisecond
    for {
       status, op := kv.px.Status(kv.seqnum)
       if status == paxos.Decided{
           return op
       }
       time.Sleep(to)
       if to < 10 * time.Second {
           to *= 2
       }
       //fmt.Printf("%d\n", status)
    }
}

func (kv *ShardKV) ApplyOp(op Op) Err{

	//fmt.Printf("PERFORMING %s  key : %s value : %s ConfigNum: %d \n", op.OpType, op.Key, op.Value, op.config.Num)
	if op.OpType == "Get" {
		<-kv.mapwriter
		cid := key2shard(op.Key)
		if kv.config.Shards[cid] != kv.gid {
			kv.mapwriter <- true
			return ErrWrongGroup
		} 
		//fmt.Printf("GET\n")
		// <-kv.mapwriter
		_, ok := kv.kvShardData[op.Key]
		kv.pastIds[op.Id] = op
		kv.mapwriter <- true
		if ok {
			return OK
		} else {
			return ErrNoKey
		}

	} else if op.OpType == "Put"  || op.OpType == "Append" {
		<-kv.mapwriter
		cid := key2shard(op.Key)
		if kv.config.Shards[cid] != kv.gid {
			kv.mapwriter <- true
			return ErrWrongGroup
		} 
		_, isin := kv.pastIds[op.Id]
		if isin {
			kv.mapwriter <- true
			return OK
		}
		//fmt.Printf("PUT\n")
		// <-kv.mapwriter
		_, ok := kv.kvShardData[op.Key]
		kv.pastIds[op.Id] = op
		if !ok || op.OpType == "Put" {
			kv.kvShardData[op.Key] = op.Value
		} else {
			kv.kvShardData[op.Key] += op.Value
		}
		kv.mapwriter <- true
		return OK
	} else if op.OpType == "Reconfigure" {
		// reconfiguration
		//fmt.Printf("Reconfig\n")
		//fmt.Println(kv.config.Num)
		if op.config.Num <= kv.config.Num {
			return OK
		}

		//kv.config.Shards[op.Num] = op.gid

		// kv.config.Num = op.config.Num
		// kv.config.Shards = op.config.Shards
		// kv.config.Groups = nil
		// kv.config.Groups =  map[int64][]string{}
		// for key, val := range op.config.Groups {
		// 	kv.config.Groups[key] = val
		// }
		// kv.received[op.Num] = true
		// kv.needsToRec[op.Num] = false

		//fmt.Printf("After Reconfig \n")
		//fmt.Println(kv.config.Num)
		kv.lastUpdated = op.Num
		return OK
	} else if op.OpType == "Update" {

		<-kv.mapwriter
		if op.config.Num <= kv.config.Num {
			kv.mapwriter <- true
			return OK
		}

		kv.config.Num = op.config.Num
		kv.config.Groups = nil
		kv.config.Groups =  map[int64][]string{}
		for key, val := range op.config.Groups {
			kv.config.Groups[key] = val
		}

		// for index, val := range op.config.Shards {
		// 	if kv.config.Shards[index] == kv.gid && val != kv.gid {
		// 		if !kv.received[index] {
		// 			kv.needsToRec[op.Num] = true
		// 		}
		// 	}
		// }

		kv.config.Shards = op.config.Shards

		// for i:=0; i < shardmaster.NShards; i++ {
		// 	kv.received[op.Num] = false
		// }

		for key, val := range op.shard {
			kv.kvShardData[key] = val
		}

		for key, val := range op.state {
			kv.pastIds[key] = val
		}
		kv.mapwriter <- true


	}
	return OK
}

func (kv *ShardKV) SendShard(args *TransferShardArgs, reply *TransferShardReply) error {
	

	//serve := (kv.config.Shards[args.Num] == kv.gid)

	//fmt.Printf("BEFORE %d %d %d\n", kv.config.Num, kv.config.Shards[args.Num], kv.gid)

	// max := kv.px.Max()
	// for i:=kv.seqnum; i < max; i++ {
	// 	//fmt.Printf("DURING %d %d %d\n", kv.config.Num, kv.config.Shards[args.Num], kv.gid)
	// 	currentOp := kv.GetNextInstance().(Op)
	// 	kv.ApplyOp(currentOp)
	// 	//if currentOp.OpType == "Reconfigure" && currentOp.config.Shards[args.Num] == kv.gid {
	// 		//serve = true
	// 	//}
	// }



	//fmt.Printf("AFTER %d %d %d\n", kv.config.Num, kv.config.Shards[args.Num], kv.gid)

	///<-kv.mapwriter

	if kv.config.Num < args.ConfigNum {
		reply.Err = ErrInvalidConfig
		return nil
	}

	//fmt.Printf("We here\n")
	// kv.mu.Lock()
	// defer kv.mu.Unlock()

	//fmt.Printf("after\n")
	// var op Op 
	// op.OpType = "Dummy"
	// op.Id = nrand()
	// op.Num = args.Num
	// op.config.Num = args.ConfigNum
	// op.gid = args.Gid
	// for {
	// 	kv.px.Start(kv.seqnum, op)
	// 	currentOp := kv.GetNextInstance().(Op)
	// 	kv.ApplyOp(currentOp)
	// 	kv.px.Done(kv.seqnum)
	// 	kv.seqnum +=1
	// 	if currentOp.OpType == op.OpType && op.Id == currentOp.Id {
	// 		break
	// 	}

	// 	// if currentOp.OpType == "Update" && currentOp.config.Num == args.Num {
	// 	// 	break
	// 	// }
	// }	 

	//fmt.Printf("Extracting Shard\n")
	<-kv.mapwriter
	reply.Shard = kv.ExtractShard(args.Num)
	reply.State = make(map[int64]Op)
	for key, val := range kv.pastIds {
		reply.State[key] = val
	}
	reply.Err = OK

	kv.mapwriter <- true
	//if serve {
		// if kv.config.Num < args.ConfigNum {
		// 	reply.Err = ErrInvalidConfig
		// 	return nil
		// }

		// shard := make(map[string]string)
		// for key, val := range kv.kvShardData {
		// 	if key2shard(key) == args.Num {
		// 		shard[key] = val
		// 	}
		// }

		// clientState := make(map[int64]Op)
		// for key, val := kv.

		// reply.Err = OK
		//reply.Shard = shard
	//} else {
	//	reply.Err = ErrWrongGroup
	//}
	return nil
}

func (kv *ShardKV) ExtractShard(num int) map[string]string{
	shard := make(map[string]string)
	for key, val := range kv.kvShardData {
		if key2shard(key) == num {
			shard[key] = val
		}
	}
	return shard
}

func (kv *ShardKV) Get(args *GetArgs, reply *GetReply) error {
	//Your code here.
	if kv.config.Num < args.ConfNum {
		reply.Err = ErrWrongGroup
		return nil
	}


	kv.mu.Lock()
	defer kv.mu.Unlock()
	var op Op
	op.OpType = "Get"
	op.Id = args.Id
	op.Key = args.Key

	for {
		kv.px.Start(kv.seqnum, op)
		currentOp := kv.GetNextInstance().(Op)
		err := kv.ApplyOp(currentOp)
		kv.px.Done(kv.seqnum)
		kv.seqnum +=1
		if currentOp.Id == op.Id && currentOp.OpType == op.OpType {
			if err == OK {
				reply.Value = kv.kvShardData[op.Key]
				reply.Err = OK
			} else {
				reply.Err = err
			}
			break
		}
	}

	return nil
}

// RPC handler for client Put and Append requests
func (kv *ShardKV) PutAppend(args *PutAppendArgs, reply *PutAppendReply) error {
	// Your code here.
	// for kv.config.Num < args.ConfNum {
	// 	kv.tick()
	// }

	// if kv.config.Num < args.ConfNum {
	// 	time.Sleep(250*time.Millisecond)
	// 	reply.Err = ErrWrongGroup
	// 	return nil
	// }
	if kv.config.Num < args.ConfNum {
		reply.Err = ErrWrongGroup
		return nil
	}

	kv.mu.Lock()
	defer kv.mu.Unlock()
	var op Op
	op.OpType = args.Op
	op.Id = args.Id
	op.Key = args.Key
	op.Value = args.Value
	for {
		kv.px.Start(kv.seqnum, op)
		currentOp := kv.GetNextInstance().(Op)
		err := kv.ApplyOp(currentOp)
		kv.px.Done(kv.seqnum)
		kv.seqnum +=1
		if currentOp.Id == op.Id && currentOp.OpType == op.OpType {
			if err == OK {
				reply.Err = OK
			} else {
				reply.Err = err
			}
			break
		}
	}
	return nil
}

//
// Ask the shardmaster if there's a new configuration;
// if so, re-configure.
//
func (kv *ShardKV) tick() {
	//kv.mu.Lock()
	//defer kv.mu.Unlock()

	//fmt.Printf("TICK %d\n", kv.me)
	// kv.mu.Lock()
	// max:= kv.px.Max()
	// for kv.seqnum < max{
	// 		//kv.px.Start(kv.seqnum, op)
	// 	currentOp := kv.GetNextInstance().(Op)
	// 	kv.ApplyOp(currentOp)
	// 	kv.px.Done(kv.seqnum)
	// 	kv.seqnum +=1
	// 	// if currentOp.OpType == "Update" && currentOp.config.Num == newConfig.Num {
	// 	// 	cont = true
	// 	// 	break
	// 	// }
	// }	 
	// kv.mu.Unlock()	
	// kv.mu.Lock()
	// defer kv.mu.Unlock()
	
	for kv.sm.Query(-1).Num > kv.config.Num {
		//fmt.Printf("INLOOP %d\n", kv.me)
		newConfig := kv.sm.Query( kv.config.Num + 1)

		oldShards := kv.config.Shards 
		oldGroups := kv.config.Groups
		var op Op
		op.OpType = "Reconfigure"
		op.config.Shards = newConfig.Shards
		op.config.Num = newConfig.Num
		op.config.Groups = make(map[int64][]string)


		for key, val := range newConfig.Groups {
			op.config.Groups[key] = val
		}


		// ask for shards
		total := make(map[string]string)
		state := make(map[int64]Op)
		for k:=0; k < shardmaster.NShards; k++ {
			if newConfig.Shards[k] == kv.gid && oldShards[k] != kv.gid && oldShards[k] != 0 {
				//fmt.Printf("Reconfiguring %d\n", k)
				reconfigure := false
				//for !reconfigure {
					servers := oldGroups[oldShards[k]]
					for j := 0; j < len(servers); j++ {
						args := &TransferShardArgs{}
						args.Num = k
						args.ConfigNum = newConfig.Num
						args.Gid = kv.gid
						//fmt.Printf("Shard num %d\n", k)
						var reply TransferShardReply
						//fmt.Printf("NOT STUCK\n")
						ok := false
						ok = call(servers[j], "ShardKV.SendShard", args, &reply)
						//fmt.Printf("STUCK\n")
						if ok && reply.Err == OK {
							//fmt.Printf("Got Shards\n")
							for key, value := range reply.Shard {
									total[key] = value
							}

							for key, value := range reply.State {
								state[key] = value
							}
							reconfigure = true
							break
						}
						time.Sleep(10* time.Millisecond)
					}
					time.Sleep(100 * time.Millisecond)
					if !reconfigure {
						//fmt.Printf("FAILED TO RECONFIG %d, TRYING AGAIN LATER\n", kv.config.Num)
						return
					}
				//}
					// if !reconfigure {
					// 	return
					// }
			//}
				// if !reconfigure {
				// 	return
				// }
			}
		}
		//fmt.Println(total)

		//var op Op
		//kv.mu.Lock()
		kv.mu.Lock()
		op.OpType = "Update"
		op.shard = total
		op.state = state
		// op.config.Shards = newConfig.Shards
		// op.config.Num = newConfig.Num
		// op.config.Groups = make(map[int64][]string)
		// for key, val := range newConfig.Groups {
		// 	op.config.Groups[key] = val
		// }

		for {
			kv.px.Start(kv.seqnum, op)
			currentOp := kv.GetNextInstance().(Op)
			kv.ApplyOp(currentOp)
			kv.px.Done(kv.seqnum)
			kv.seqnum +=1
			if currentOp.OpType == op.OpType && currentOp.config.Num == op.config.Num{
				break
			}
		}
		kv.mu.Unlock()
		//kv.mu.Unlock()




	}

}

// tell the server to shut itself down.
// please don't change these two functions.
func (kv *ShardKV) kill() {
	atomic.StoreInt32(&kv.dead, 1)
	kv.l.Close()
	kv.px.Kill()
}

// call this to find out if the server is dead.
func (kv *ShardKV) isdead() bool {
	return atomic.LoadInt32(&kv.dead) != 0
}

// please do not change these two functions.
func (kv *ShardKV) Setunreliable(what bool) {
	if what {
		atomic.StoreInt32(&kv.unreliable, 1)
	} else {
		atomic.StoreInt32(&kv.unreliable, 0)
	}
}

func (kv *ShardKV) isunreliable() bool {
	return atomic.LoadInt32(&kv.unreliable) != 0
}

//
// Start a shardkv server.
// gid is the ID of the server's replica group.
// shardmasters[] contains the ports of the
//   servers that implement the shardmaster.
// servers[] contains the ports of the servers
//   in this replica group.
// Me is the index of this server in servers[].
//
func StartServer(gid int64, shardmasters []string,
	servers []string, me int) *ShardKV {
	gob.Register(Op{})

	kv := new(ShardKV)
	kv.me = me
	kv.gid = gid
	kv.sm = shardmaster.MakeClerk(shardmasters)

	// Your initialization code here.
	// Don't call Join().
	kv.kvShardData = make(map[string]string)
	kv.pastIds = make(map[int64]Op)
	kv.mapwriter = make(chan bool, 1)
	kv.mapwriter <- true

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
				fmt.Printf("ShardKV(%v) accept: %v\n", me, err.Error())
				kv.kill()
			}
		}
	}()

	go func() {
		for kv.isdead() == false {
			kv.tick()
			time.Sleep(250 * time.Millisecond)
		}
	}()

	return kv
}
