package pbservice

import "viewservice"
import "net/rpc"
import "fmt"
import "time"

import "crypto/rand"
import "math/big"


type Clerk struct {
	vs *viewservice.Clerk
	// Your declarations here
	vshost string
	Primary string
	me string
}

// this may come in handy.
func nrand() int64 {
	max := big.NewInt(int64(1) << 62)
	bigx, _ := rand.Int(rand.Reader, max)
	x := bigx.Int64()
	return x
}

func MakeClerk(vshost string, me string) *Clerk {
	ck := new(Clerk)
	ck.vs = viewservice.MakeClerk(me, vshost)
	// Your ck.* initializations here
	ck.vshost = vshost
	ck.me = me
	ck.tick()
	time.Sleep(viewservice.PingInterval)
	return ck
}

func (ck *Clerk) tick() {
	args := &viewservice.GetArgs{}
	var reply viewservice.GetReply

	ok := call(ck.vshost, "ViewServer.Get", args, &reply)
	if !ok {
		return
	}

	ck.Primary = reply.View.Primary
}


//
// call() sends an RPC to the rpcname handler on server srv
// with arguments args, waits for the reply, and leaves the
// reply in reply. the reply argument should be a pointer
// to a reply structure.
//
// the return value is true if the server responded, and false
// if call() was not able to contact the server. in particular,
// the reply's contents are only valid if call() returned true.
//
// you should assume that call() will return an
// error after a while if the server is dead.
// don't provide your own time-out mechanism.
//
// please use call() to send all RPCs, in client.go and server.go.
// please don't change this function.
//
func call(srv string, rpcname string,
	args interface{}, reply interface{}) bool {
	c, errx := rpc.Dial("unix", srv)
	if errx != nil {
		return false
	}
	defer c.Close()

	err := c.Call(rpcname, args, reply)
	if err == nil {
		return true
	}

	fmt.Println(err)
	return false
}

//
// fetch a key's value from the current primary;
// if they key has never been set, return "".
// Get() must keep trying until it either the
// primary replies with the value or the primary
// says the key doesn't exist (has never been Put().
//
func (ck *Clerk) Get(key string) string {

	// Your code here.
	for ck.Primary == "" {
		ck.tick()
		time.Sleep(viewservice.PingInterval)
	}
	args := &GetArgs{}
	args.Key = key
	var reply GetReply
	for true {
		if ck.Primary == "" {
			ck.tick()
			time.Sleep(viewservice.PingInterval)
		}
		ok := call(ck.Primary, "PBServer.Get", args, &reply)
		if !ok {
			ck.tick()
			time.Sleep(viewservice.PingInterval)
		}
		if reply.Err == ErrNoKey {
			return ""
		}
		if reply.Err == OK{
			break
		}
		if reply.Err == ErrWrongServer {
			ck.tick()
			time.Sleep(viewservice.PingInterval)

		}
	}



	return reply.Value
}

//
// send a Put or Append RPC
//
func (ck *Clerk) PutAppend(key string, value string, op string) {

	// Your code here.
	for ck.Primary == "" {
		ck.tick()
		time.Sleep(viewservice.PingInterval)
	}

	args := &PutAppendArgs{}
	args.Key = key
	args.Value = value
	args.Type = op
	args.Primary = ck.Primary
	args.Id = nrand()
	var reply PutAppendReply
	for true {
		if ck.Primary == "" {
			ck.tick()
			args.Primary = ck.Primary
			time.Sleep(viewservice.PingInterval)
		}
		ok := call(ck.Primary, "PBServer.PutAppend", args, &reply)
		if !ok || reply.Err == ErrWrongServer {
			ck.tick()
			args.Primary = ck.Primary
			time.Sleep(viewservice.PingInterval)
		}else  {
			break
		}
	}
}

//
// tell the primary to update key's value.
// must keep trying until it succeeds.
//
func (ck *Clerk) Put(key string, value string) {
	ck.PutAppend(key, value, "Put")
}

//
// tell the primary to append to key's value.
// must keep trying until it succeeds.
//
func (ck *Clerk) Append(key string, value string) {
	ck.PutAppend(key, value, "Append")
}
