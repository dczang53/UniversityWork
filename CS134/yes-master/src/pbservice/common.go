package pbservice

const (
	OK             = "OK"
	ErrNoKey       = "ErrNoKey"
	ErrWrongServer = "ErrWrongServer"
)

type ErrorString struct {
    s string
}

func (e *ErrorString) Error() string {
    return e.s
}

type Err string

// Put or Append
type PutAppendArgs struct {
	Key   string
	Value string
	// You'll have to add definitions here.
	Type  string
	// Field names must start with capital letters,
	// otherwise RPC will break.
	Primary string 
	Id    int64
}

type PutAppendReply struct {
	Err Err
	Seen bool
}

type GetArgs struct {
	Key string
	// You'll have to add definitions here.
}

type GetReply struct {
	Err   Err
	Value string
}


// Your RPC definitions here.

type SendDataArgs struct {
	Primary string
	Backup string
	Data map[string]string
}

type SendDataReply struct {
	Err Err
}
