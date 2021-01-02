package shardmaster

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
import "errors"

type ShardMaster struct {
    mu         sync.Mutex
    l          net.Listener
    me         int
    dead       int32 // for testing
    unreliable int32 // for testing
    px         *paxos.Paxos

    configs []Config // indexed by config num
    seqnum    int 
}


type Op struct {
    Optype  string
    Num     int
    GID     int64
    Shard   int
    Servers []string
    Id 		int64
}


func (sm *ShardMaster) GetNextInstance() interface{} {
    to := 10 * time.Millisecond
    for {
       status, op := sm.px.Status(sm.seqnum)
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

func (sm *ShardMaster) ApplyOp(op Op) string {

	if op.Optype == "Query" {
		return ""
	}

    buffer := new(Config)
    buffer.Num = len(sm.configs)
    if op.Optype == "Join" {
        //fmt.Printf("Join()\n")
        if op.GID == 0 {
            return "Invalid GID"
        }
        GID2Shards := make(map[int64][]int64)
        if len(sm.configs[len(sm.configs) - 1].Groups) == 0 {
            GID2Shards[op.GID] = make([]int64, NShards)
            for i := 0; i < NShards; i++ {
                GID2Shards[op.GID][i] = int64(i)
            }
        } else {
            GID2Shards[op.GID] = make([]int64, 0)
            for ind, val := range sm.configs[len(sm.configs) - 1].Shards {
                if val == op.GID {
                    return ""
                } else if val == 0 {
                    return "Error with Shards; GID 0 present for Join()"
                } else {
                    if _, ok := GID2Shards[val]; !ok {
                        GID2Shards[val] = []int64{int64(ind)}
                    } else {
                        GID2Shards[val] = append(GID2Shards[val], int64(ind))
                    }
                }
            }
            partBegin := NShards / len(GID2Shards)
            if partBegin != 0 {
                for key, val := range GID2Shards {
                    if (key != op.GID) && (len(val) > partBegin) && (len(GID2Shards[op.GID]) < partBegin) {
                        GID2Shards[op.GID] = append(GID2Shards[op.GID], val[partBegin:]...)
                        GID2Shards[key] = val[0:partBegin]
                    }
                }
            }
        }
        for key, arr := range GID2Shards {
            for _, val := range arr {
                buffer.Shards[val] = key
            }
        }
        buffer.Groups = make(map[int64][]string)
        for key, val := range sm.configs[len(sm.configs) - 1].Groups {
            buffer.Groups[key] = val
        }
        buffer.Groups[op.GID] = op.Servers
        sm.configs = append(sm.configs, *buffer)
    } else if op.Optype == "Leave" {
        // fmt.Printf("Leave()\n")
        if op.GID == 0 {
            return "Invalid GID"
        }
        if len(sm.configs[len(sm.configs) - 1].Groups) == 0 {
            return ""
        }
        if _, ok := sm.configs[len(sm.configs) - 1].Groups[op.GID]; !ok {
            //return "Error with Leave(); GID not in Groups"
            return ""
        }
        GID2Shards := make(map[int64][]int64)
        toAllocate := make([]int64, 0)
        for ind, val := range sm.configs[len(sm.configs) - 1].Shards {
            if val == op.GID {
                toAllocate = append(toAllocate, int64(ind))
            } else if val == 0 {
                return "Error with Shards; GID 0 present for Leave()"
            } else if _, ok := GID2Shards[val]; !ok {
                GID2Shards[val] = []int64{int64(ind)}
            } else {
                GID2Shards[val] = append(GID2Shards[val], int64(ind))
            }
        }
        for key, _ := range sm.configs[len(sm.configs) - 1].Groups {
            if _, ok := GID2Shards[key]; (!ok) && (key != op.GID) {
                GID2Shards[key] = make([]int64, 0)
            }
        }
        if len(GID2Shards) != 0 {
            partEnd := (NShards + len(GID2Shards) - 1) / len(GID2Shards)
            for _, val := range toAllocate {
                for key, arr := range GID2Shards {
                    if len(arr) < partEnd {
                        GID2Shards[key] = append(arr, val)
                        break
                    }
                }
            }
        } else {
            for j := 0; j < NShards; j++ {
                GID2Shards[0] = append(GID2Shards[0], int64(j))
            }
        }
        for key, arr := range GID2Shards {
            for _, val := range arr {
                buffer.Shards[val] = key
            }
        }
        buffer.Groups = make(map[int64][]string)
        for key, val := range sm.configs[len(sm.configs) - 1].Groups {
            if key != op.GID {
                buffer.Groups[key] = val
            }
        }
        sm.configs = append(sm.configs, *buffer)
    } else if op.Optype == "Move" {
        // fmt.Printf("Move()\n")
        buffer.Shards = sm.configs[len(sm.configs) - 1].Shards
        originalGID := buffer.Shards[op.Shard]
        buffer.Shards[op.Shard] = op.GID
        buffer.Groups = make(map[int64][]string)
        for key, val := range sm.configs[len(sm.configs) - 1].Groups {
            buffer.Groups[key] = val
        }
        i := 0
        for ; i < NShards; i++ {
            if buffer.Shards[i] == originalGID {
                break
            }
        }
        if i == NShards {
            delete(buffer.Groups, originalGID)
        }
        sm.configs = append(sm.configs, *buffer)
    }
    ///*
    // fmt.Printf("-----\n")
    // fmt.Printf("%d\n", sm.seqnum)
    // fmt.Printf("%d\n", buffer.Num)
    // fmt.Printf("%v\n", buffer.Shards)
    // fmt.Printf("%v\n", buffer.Groups)
    // fmt.Printf("-----\n\n\n")
    //*/
    return ""
}

func (sm *ShardMaster) Join(args *JoinArgs, reply *JoinReply) error {
    // Your code here.
    sm.mu.Lock()
    var op Op
    op.Optype = "Join"
    op.GID = args.GID
    op.Servers = args.Servers
    for {
        sm.px.Start(sm.seqnum, op)
        currentOp := sm.GetNextInstance().(Op)
        err := sm.ApplyOp(currentOp)
        if err != "" {
            return errors.New(err)
        }
        sm.px.Done(sm.seqnum)
        sm.seqnum+=1
        if currentOp.GID == op.GID && op.Optype == currentOp.Optype && (err == "") {
            break
        }
    }
    sm.mu.Unlock()
    return nil
}

func (sm *ShardMaster) Leave(args *LeaveArgs, reply *LeaveReply) error {
    // Your code here.
    sm.mu.Lock()
    var op Op
    op.Optype = "Leave"
    op.GID = args.GID
    for {
        sm.px.Start(sm.seqnum, op)
        currentOp := sm.GetNextInstance().(Op)
        err := sm.ApplyOp(currentOp)
        if err != "" {
            return errors.New(err)
        }
        sm.px.Done(sm.seqnum)
        sm.seqnum+=1
        if currentOp.GID == op.GID && op.Optype == currentOp.Optype {
            break
        }
    }
    sm.mu.Unlock()
    return nil
}

func (sm *ShardMaster) Move(args *MoveArgs, reply *MoveReply) error {
    // Your code here.
    sm.mu.Lock()
    var op Op
    op.Optype = "Move"
    op.GID = args.GID
    op.Shard = args.Shard
    for {
        sm.px.Start(sm.seqnum, op)
        currentOp := sm.GetNextInstance().(Op)
        err := sm.ApplyOp(currentOp)
        if err != "" {
            return errors.New(err)
        }
        sm.px.Done(sm.seqnum)
        sm.seqnum+=1
        if currentOp.GID == op.GID && op.Optype == currentOp.Optype && op.Shard == currentOp.Shard {
            break
        }
    }
    sm.mu.Unlock()
    return nil
}

func (sm *ShardMaster) Query(args *QueryArgs, reply *QueryReply) error {
    // Your code here.
    sm.mu.Lock()
    if args.Num != -1 && args.Num < len(sm.configs) {
        reply.Config = sm.configs[args.Num]
    } else {
    	var op Op
    	op.Optype = "Query"
    	op.Id = rand.Int63()
        for {
        	sm.px.Start(sm.seqnum, op)
            currentOp := sm.GetNextInstance().(Op)
            err := sm.ApplyOp(currentOp)
            if err != "" {
                return errors.New(err)
            }
            sm.px.Done(sm.seqnum)
            sm.seqnum+=1
            if op.Id == currentOp.Id {
            	break
            }
        }
        reply.Config = sm.configs[len(sm.configs)-1]
    }
    sm.mu.Unlock()
    return nil

    /*
    sm.mu.Lock()
    if args.Num >= len(sm.configs) {
        sm.mu.Unlock()
        return errors.New("Invalid Num")
    } else if args.Num == -1 {
        reply.Config = sm.configs[len(sm.configs) - 1]
    } else {
        reply.Config = sm.configs[args.Num]
    }
    fmt.Printf("-----\n")
    fmt.Printf("Query()\n")
    fmt.Printf("%d\n", reply.Config.Num)
    fmt.Printf("%v\n", reply.Config.Shards)
    fmt.Printf("%v\n", reply.Config.Groups)
    fmt.Printf("-----\n\n\n")
    sm.mu.Unlock()
    return nil
    */
}

// please don't change these two functions.
func (sm *ShardMaster) Kill() {
    atomic.StoreInt32(&sm.dead, 1)
    sm.l.Close()
    sm.px.Kill()
}

// call this to find out if the server is dead.
func (sm *ShardMaster) isdead() bool {
    return atomic.LoadInt32(&sm.dead) != 0
}

// please do not change these two functions.
func (sm *ShardMaster) setunreliable(what bool) {
    if what {
        atomic.StoreInt32(&sm.unreliable, 1)
    } else {
        atomic.StoreInt32(&sm.unreliable, 0)
    }
}

func (sm *ShardMaster) isunreliable() bool {
    return atomic.LoadInt32(&sm.unreliable) != 0
}

//
// servers[] contains the ports of the set of
// servers that will cooperate via Paxos to
// form the fault-tolerant shardmaster service.
// me is the index of the current server in servers[].
//
func StartServer(servers []string, me int) *ShardMaster {
    sm := new(ShardMaster)
    sm.me = me

    sm.configs = make([]Config, 1)
    sm.configs[0].Groups = map[int64][]string{}

    rpcs := rpc.NewServer()

    gob.Register(Op{})
    rpcs.Register(sm)
    sm.px = paxos.Make(servers, me, rpcs)

    os.Remove(servers[me])
    l, e := net.Listen("unix", servers[me])
    if e != nil {
        log.Fatal("listen error: ", e)
    }
    sm.l = l

    // please do not change any of the following code,
    // or do anything to subvert it.

    go func() {
        for sm.isdead() == false {
            conn, err := sm.l.Accept()
            if err == nil && sm.isdead() == false {
                if sm.isunreliable() && (rand.Int63()%1000) < 100 {
                    // discard the request.
                    conn.Close()
                } else if sm.isunreliable() && (rand.Int63()%1000) < 200 {
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
            if err != nil && sm.isdead() == false {
                fmt.Printf("ShardMaster(%v) accept: %v\n", me, err.Error())
                sm.Kill()
            }
        }
    }()

    return sm
}
