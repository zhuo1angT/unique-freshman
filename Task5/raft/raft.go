package raft

//
// this is an outline of the API that raft must expose to
// the service (or tester). see comments below for
// each of these functions for more details.
//
// rf = Make(...)
//   create a new Raft server.
// rf.Start(command interface{}) (index, term, isleader)
//   start agreement on a new log entry
// rf.GetState() (term, isLeader)
//   ask a Raft for its current term, and whether it thinks it is leader
// ApplyMsg
//   each time a new entry is committed to the log, each Raft peer
//   should send an ApplyMsg to the service (or tester)
//   in the same server.
//

import (
	"bytes"
	"encoding/gob"
	"math/rand"
	"runtime"
	"sort"
	"strconv"
	"sync"
	"sync/atomic"
	"time"

	"../labrpc"
)

// import "bytes"
// import "../labgob"

//
// as each Raft peer becomes aware that successive log entries are
// committed, the peer should send an ApplyMsg to the service (or
// tester) on the same server, via the applyCh passed to Make(). set
// CommandValid to true to indicate that the ApplyMsg contains a newly
// committed log entry.
//
// in Lab 3 you'll want to send other kinds of messages (e.g.,
// snapshots) on the applyCh; at that point you can add fields to
// ApplyMsg, but set CommandValid to false for these other uses.
//
type ApplyMsg struct {
	CommandValid bool
	Command      interface{}
	CommandIndex int
}

//
// A log entry
//
type LogEntry struct {
	Command interface{}
	Term    int
}

// State constant
const (
	Follower  int = 0
	Candidate int = 1
	Leader    int = 2
)

//
// A Go object implementing a single Raft peer.
//
type Raft struct {
	mu        sync.Mutex          // Lock to protect shared access to this peer's state
	peers     []*labrpc.ClientEnd // RPC end points of all peers
	persister *Persister          // Object to hold this peer's persisted state
	me        int                 // this peer's index into peers[]
	dead      int32               // set by Kill()

	// Your data here (2A, 2B, 2C).
	// Look at the paper's Figure 2 for a description of what
	// state a Raft server must maintain.

	// Persistent state on all servers
	currentTerm int
	votedFor    int
	log         []LogEntry

	// Volatile state on all servers
	committedIndex int
	lastApplied    int

	// Volatile state on leaders
	nextIndex  []int
	matchIndex []int

	// other self added states
	state   int           // state
	applyCh chan ApplyMsg // channel

	timer *time.Timer // timer

	grantVoteCh chan bool
	heartBeatCh chan bool
	leaderCh    chan bool

	voteGranted    int
	voteNotGranted int
}

//
// return currentTerm and whether this server
// believes it is the leader.
//
func (rf *Raft) GetState() (int, bool) {
	var term int
	var isleader bool
	// Your code here (2A).
	term = rf.currentTerm
	if rf.state == Leader {
		isleader = true
	} else {
		isleader = false
	}

	return term, isleader
}

//
// save Raft's persistent state to stable storage,
// where it can later be retrieved after a crash and restart.
// see paper's Figure 2 for a description of what should be persistent.
//
func (rf *Raft) persist() {
	// Your code here (2C).
	// Example:
	// w := new(bytes.Buffer)
	// e := labgob.NewEncoder(w)
	// e.Encode(rf.xxx)
	// e.Encode(rf.yyy)
	// data := w.Bytes()
	// rf.persister.SaveRaftState(data)

	w := new(bytes.Buffer)
	e := gob.NewEncoder(w)
	e.Encode(rf.currentTerm)
	e.Encode(rf.votedFor)
	e.Encode(rf.log)
	e.Encode(rf.voteGranted)
	e.Encode(rf.voteNotGranted)
	data := w.Bytes()
	rf.persister.SaveRaftState(data)

}

//
// restore previously persisted state.
//
func (rf *Raft) readPersist(data []byte) {

	// Your code here (2C).
	// Example:
	// r := bytes.NewBuffer(data)
	// d := labgob.NewDecoder(r)
	// var xxx
	// var yyy
	// if d.Decode(&xxx) != nil ||
	//    d.Decode(&yyy) != nil {
	//   error...
	// } else {
	//   rf.xxx = xxx
	//   rf.yyy = yyy

	r := bytes.NewBuffer(data)
	d := gob.NewDecoder(r)
	d.Decode(&rf.currentTerm)
	d.Decode(&rf.votedFor)
	d.Decode(&rf.log)
	d.Decode(&rf.voteGranted)
	d.Decode(&rf.voteNotGranted)
	if data == nil || len(data) < 1 { // bootstrap without any state?
		return
	}

	//fmt.Printf("----Server %d, Read Persist ---- term = %d, votedFor = %d, log = %v\n", rf.me, rf.currentTerm, rf.votedFor, rf.log)

}

//
// example RequestVote RPC arguments structure.
// field names must start with capital letters!
//
type RequestVoteArgs struct {
	// Your data here (2A, 2B).
	Term         int
	CandidateID  int
	LastLogIndex int
	LastLogTerm  int
}

//
// example RequestVote RPC reply structure.
// field names must start with capital letters!
//
type RequestVoteReply struct {
	// Your data here (2A).
	Term        int
	VoteGranted bool
}

//
// example RequestVote RPC handler.
//
func (rf *Raft) RequestVote(args *RequestVoteArgs, reply *RequestVoteReply) {
	// Your code here (2A, 2B).

	//fmt.Printf("Server %d: log is %v\n", rf.me, rf.log)

	var newer bool

	if args.Term > rf.currentTerm {
		rf.votedFor = -1
	}

	if len(rf.log) == 0 || args.LastLogTerm > rf.log[len(rf.log)-1].Term {
		newer = true
	} else if args.LastLogTerm == rf.log[len(rf.log)-1].Term && len(rf.log) <= args.LastLogIndex+1 {
		newer = true
	}

	if newer == true && (rf.votedFor == -1 || rf.votedFor == args.CandidateID) {
		reply.VoteGranted = true
	} else {
		reply.VoteGranted = false
	}

	var votedFor int
	if reply.VoteGranted {
		votedFor = args.CandidateID
	} else {
		votedFor = -1
	}
	rf.votedFor = votedFor

	if args.Term < rf.currentTerm {
		reply.VoteGranted = false
	} else if args.Term > rf.currentTerm {
		rf.currentTerm = args.Term
		if rf.state != Follower {
			rf.convertToFollower(rf.currentTerm, votedFor)
		}
	}

	reply.Term = rf.currentTerm

	rf.persist()

	if reply.VoteGranted == true {
		go func() {
			select {
			case <-rf.grantVoteCh:
			default:
			}
			rf.grantVoteCh <- true
		}()
	}
}

//
// example code to send a RequestVote RPC to a server.
// server is the index of the target server in rf.peers[].
// expects RPC arguments in args.
// fills in *reply with RPC reply, so caller should
// pass &reply.
// the types of the args and reply passed to Call() must be
// the same as the types of the arguments declared in the
// handler function (including whether they are pointers).
//
// The labrpc package simulates a lossy network, in which servers
// may be unreachable, and in which requests and replies may be lost.
// Call() sends a request and waits for a reply. If a reply arrives
// within a timeout interval, Call() returns true; otherwise
// Call() returns false. Thus Call() may not return for a while.
// A false return can be caused by a dead server, a live server that
// can't be reached, a lost request, or a lost reply.
//
// Call() is guaranteed to return (perhaps after a delay) *except* if the
// handler function on the server side does not return.  Thus there
// is no need to implement your own timeouts around Call().
//
// look at the comments in ../labrpc/labrpc.go for more details.
//
// if you're having trouble getting RPC to work, check that you've
// capitalized all field names in structs passed over RPC, and
// that the caller passes the address of the reply struct with &, not
// the struct itself.
//
func (rf *Raft) sendRequestVote(server int, args *RequestVoteArgs, reply *RequestVoteReply) bool {
	ok := rf.peers[server].Call("Raft.RequestVote", args, reply)
	return ok
}

//
// example AppendEntries RPC arguments structure.
// field names must start with capital letters!
//
type AppendEntriesArgs struct {
	Term         int
	LeaderID     int
	PrevLogIndex int
	PrevLogTerm  int
	Entries      []LogEntry
	LeaderCommit int
}

//
// example AppendEntries RPC reply structure.
// field names must start with capital letters!
//
type AppendEntriesReply struct {
	Term          int
	Success       bool
	ConflictIndex int
	ConflictTerm  int
}

//
// example RequestVote RPC handler.
//
func (rf *Raft) AppendEntries(args *AppendEntriesArgs, reply *AppendEntriesReply) {
	rf.mu.Lock()
	defer rf.mu.Unlock()

	if args.Term > rf.currentTerm {
		rf.currentTerm = args.Term
		if rf.state != Follower {
			rf.convertToFollower(rf.currentTerm, rf.votedFor)
		}
	}

	reply.Term = rf.currentTerm

	if args.Term < rf.currentTerm {
		reply.Success = false
	} else if len(rf.log) <= args.PrevLogIndex {
		reply.Success = false
		reply.ConflictIndex = len(rf.log) - 1
		reply.ConflictTerm = -1
	} else if args.PrevLogIndex == -1 {
		reply.Success = true
	} else if rf.log[args.PrevLogIndex].Term != args.PrevLogTerm {
		reply.Success = false

		prevLogTerm := -1
		if args.PrevLogIndex >= 0 {
			prevLogTerm = rf.log[args.PrevLogIndex].Term
		}
		if args.PrevLogTerm != prevLogTerm {
			reply.ConflictTerm = prevLogTerm
			for i := 0; i < len(rf.log); i++ {
				if rf.log[i].Term == prevLogTerm {
					reply.ConflictIndex = i
					break
				}
			}
		}

	} else {
		reply.Success = true
	}

	if reply.Success {
		for i := 0; i < len(args.Entries); i++ {
			if args.PrevLogIndex+i+1 >= len(rf.log) {
				break
			}
			if rf.log[args.PrevLogIndex+i+1].Term != args.Entries[i].Term {
				rf.log = rf.log[:args.PrevLogIndex+i]
				break
			}
		}
		if args.PrevLogIndex < len(rf.log) {
			for i := 0; i < len(args.Entries); i++ {
				if args.PrevLogIndex+i+1 >= len(rf.log) {
					rf.log = append(rf.log, args.Entries[i])
				} else {
					rf.log[args.PrevLogIndex+i+1] = args.Entries[i]
				}
			}
		}

		if args.LeaderCommit > rf.committedIndex {
			if args.LeaderCommit > args.PrevLogIndex+len(args.Entries) {
				rf.committedIndex = args.PrevLogIndex + len(args.Entries)
			} else {
				rf.committedIndex = args.LeaderCommit
			}
			if rf.committedIndex >= len(rf.log) {
				rf.committedIndex = len(rf.log) - 1
			}
		}
		rf.startApplyLogs()
	}

	rf.persist()

	go func() {
		select {
		case <-rf.heartBeatCh:
		default:
		}
		rf.heartBeatCh <- true
	}()

}

//
// example code to send a RequestVote RPC to a server.
// server is the index of the target server in rf.peers[].
// expects RPC arguments in args.
//
func (rf *Raft) sendAppendEntries(server int, args *AppendEntriesArgs, reply *AppendEntriesReply) bool {
	ok := rf.peers[server].Call("Raft.AppendEntries", args, reply)
	return ok
}

//
// the service using Raft (e.g. a k/v server) wants to start
// agreement on the next command to be appended to Raft's log. if this
// server isn't the leader, returns false. otherwise start the
// agreement and return immediately. there is no guarantee that this
// command will ever be committed to the Raft log, since the leader
// may fail or lose an election. even if the Raft instance has been killed,
// this function should return gracefully.
//
// the first return value is the index that the command will appear at
// if it's ever committed. the second return value is the current
// term. the third return value is true if this server believes it is
// the leader.
//
func (rf *Raft) Start(command interface{}) (int, int, bool) {
	rf.mu.Lock()
	defer rf.mu.Unlock()

	index := -1
	term := rf.currentTerm
	isLeader := (rf.state == Leader)

	// Your code here (2B).
	if isLeader {
		//fmt.Printf("Leader %d: got a new Start task, command: %v\n", rf.me, command)

		index = len(rf.log)
		rf.log = append(rf.log, LogEntry{command, rf.currentTerm})
		rf.persist()
	}

	//fmt.Printf("%d %d %v\n", index, term, isLeader)

	return index, term, isLeader
}

//
// the tester doesn't halt goroutines created by Raft after each test,
// but it does call the Kill() method. your code can use killed() to
// check whether Kill() has been called. the use of atomic avoids the
// need for a lock.
//
// the issue is that long-running goroutines use memory and may chew
// up CPU time, perhaps causing later tests to fail and generating
// confusing debug output. any goroutine with a long-running loop
// should call killed() to check whether it should stop.
//
func (rf *Raft) Kill() {
	atomic.StoreInt32(&rf.dead, 1)
	// Your code here, if desired.
}

func (rf *Raft) killed() bool {
	z := atomic.LoadInt32(&rf.dead)
	return z == 1
}

//
// the service or tester wants to create a Raft server. the ports
// of all the Raft servers (including this one) are in peers[]. this
// server's port is peers[me]. all the servers' peers[] arrays
// have the same order. persister is a place for this server to
// save its persistent state, and also initially holds the most
// recent saved state, if any. applyCh is a channel on which the
// tester or service expects Raft to send ApplyMsg messages.
// Make() must return quickly, so it should start goroutines
// for any long-running work.
//
func Make(peers []*labrpc.ClientEnd, me int,
	persister *Persister, applyCh chan ApplyMsg) *Raft {
	rf := &Raft{}
	rf.mu = sync.Mutex{}
	rf.peers = peers
	rf.persister = persister
	rf.me = me

	// Your initialization code here (2A, 2B, 2C).

	rf.currentTerm = 0
	rf.votedFor = -1
	rf.log = []LogEntry{}
	rf.committedIndex = -1
	rf.lastApplied = -1

	rf.state = Follower
	rf.applyCh = applyCh

	rf.grantVoteCh = make(chan bool)
	rf.heartBeatCh = make(chan bool)
	rf.leaderCh = make(chan bool)

	rf.timer = time.NewTimer(time.Duration(rand.Intn(300)+300) * time.Millisecond)

	// initialize from state persisted before a crash
	rf.readPersist(persister.ReadRaftState())

	go rf.heartBeat()

	return rf
}

func (rf *Raft) startRequestVote() {
	rf.mu.Lock()
	if rf.state != Candidate {
		rf.mu.Unlock()
		return
	}

	lastLogIndex := len(rf.log) - 1
	lastLogTerm := -1

	if lastLogIndex >= 0 {
		lastLogTerm = rf.log[lastLogIndex].Term
	}
	args := RequestVoteArgs{
		Term:         rf.currentTerm,
		CandidateID:  rf.me,
		LastLogIndex: lastLogIndex,
		LastLogTerm:  lastLogTerm,
	}

	nLeader := 0
	rf.mu.Unlock()

	for i := 0; i < len(rf.peers); i++ {

		go func(j int) {
			if j == rf.me {
				return
			}
			reply := RequestVoteReply{}

			ok := rf.sendRequestVote(j, &args, &reply)

			if ok {

				//fmt.Printf("Candidate %d: Request %d's Vote, args = %v, reply = %v\n", rf.me, j, args, reply)

				rf.mu.Lock()
				if reply.Term > rf.currentTerm {
					rf.convertToFollower(reply.Term, -1)
					rf.mu.Unlock()
					return
				}

				if rf.currentTerm != args.Term || rf.state != Candidate {
					rf.mu.Unlock()
					return
				}

				if reply.VoteGranted {
					rf.voteGranted++

					if nLeader == 0 && rf.voteGranted*2 > len(rf.peers) && rf.state == Candidate {
						nLeader++
						rf.convertToLeader()
						go func() {
							select {
							case <-rf.leaderCh:
							default:
							}
							rf.leaderCh <- true
						}()
					}
				} else {
					rf.voteNotGranted++
				}

				rf.persist()
				rf.mu.Unlock()
			} else {
				//rf.runtimeLog=append(rf.runtimeLog,fmt.Sprintf("Candidate %d: sending RequestVote to server %d failed\n", rf.me, j))
			}
		}(i)
	}
}

func (rf *Raft) startAppendEntries() {
	for {

		if rf.killed() == true {
			return
		}

		rf.mu.Lock()
		if rf.state != Leader {
			rf.mu.Unlock()
			return
		}
		rf.mu.Unlock()
		for i := 0; i < len(rf.peers); i++ {
			go func(j int) {
				if j == rf.me {
					return
				}

				for {
					rf.mu.Lock()
					if rf.state != Leader {
						rf.mu.Unlock()
						return
					}
					prevLogIndex := rf.nextIndex[j] - 1
					prevLogTerm := -1
					var entries []LogEntry

					if prevLogIndex >= 0 {
						prevLogTerm = rf.log[prevLogIndex].Term
					}
					entries = append([]LogEntry{}, rf.log[prevLogIndex+1:]...)

					args := AppendEntriesArgs{
						Term:         rf.currentTerm,
						LeaderID:     rf.me,
						PrevLogIndex: prevLogIndex,
						PrevLogTerm:  prevLogTerm,
						Entries:      entries,
						LeaderCommit: rf.committedIndex,
					}

					reply := AppendEntriesReply{}

					//fmt.Printf("Leader %d: AppendEntries to %d, args = %v\n", rf.me, j, args)

					rf.mu.Unlock()
					ok := rf.sendAppendEntries(j, &args, &reply)

					if ok {
						rf.mu.Lock()
						if reply.Term > rf.currentTerm {
							rf.convertToFollower(reply.Term, -1)
							rf.mu.Unlock()
							return
						}

						if rf.currentTerm != args.Term || rf.state != Leader {
							rf.mu.Unlock()
							return
						}

						if reply.Success == true {
							rf.matchIndex[j] = prevLogIndex + len(entries)
							rf.nextIndex[j] = rf.matchIndex[j] + 1

							copyMatchIndex := make([]int, len(rf.peers))
							copy(copyMatchIndex, rf.matchIndex)
							copyMatchIndex[rf.me] = len(rf.log)
							sort.Ints(copyMatchIndex)
							N := copyMatchIndex[len(rf.peers)/2]

							if N > rf.committedIndex && rf.log[N].Term == rf.currentTerm {
								rf.committedIndex = N
							}

							rf.startApplyLogs()
							rf.mu.Unlock()
							return
						} else {

							hasConflictTerm:= false
							for i := 0; i < len(rf.log); i++ {
								if rf.log[i].Term == reply.ConflictTerm {
									hasConflictTerm = true
								}
								if rf.log[i].Term > reply.ConflictTerm {
									if hasConflictTerm {
										rf.nextIndex[j] = i
									} else {
										rf.nextIndex[j] = reply.ConflictIndex
									}
									break
								}
							}

							if rf.nextIndex[j] < 0 {
								rf.nextIndex[j] = 0
							}
							rf.mu.Unlock()
						}
					} else {
						return
					}
				}
			}(i)
		}

		time.Sleep(time.Duration(100) * time.Millisecond)

	}
}

// Raft heartbeat
func (rf *Raft) heartBeat() {

	for {

		// fmt.Printf("Server %d : Goroutine ID %v long running \n", rf.me, GetGoroutineID())

		if rf.killed() == true {
			return
		}

		rf.mu.Lock()
		state := rf.state
		rf.mu.Unlock()

		rf.persist()
		//rf.runtimeLog=append(rf.runtimeLog,fmt.Sprintf("Server %d: state = %d\n", rf.me, rf.state))

		heartbeatTimeout := rand.Intn(200) + 200

		if rf.state != Candidate {
			rf.timer.Reset(time.Duration(heartbeatTimeout) * time.Millisecond)
		}
		switch {
		case state == Leader:
			rf.startAppendEntries()
		case state == Candidate:
			go rf.startRequestVote()
			select {
			case <-rf.heartBeatCh:
				rf.mu.Lock()
				rf.convertToFollower(rf.currentTerm, -1)
				rf.mu.Unlock()
			case <-rf.leaderCh:
			case <-rf.timer.C:
				rf.mu.Lock()
				if rf.state == Follower {
					rf.mu.Unlock()
					continue
				}
				if rf.voteNotGranted*2 < len(rf.peers) {
					rf.convertToCandidate()
				} else {
					rf.convertToFollower(rf.currentTerm, -1)
				}
				rf.mu.Unlock()
			}
		case state == Follower:
			rf.mu.Lock()

			select {
			case <-rf.timer.C:
			default:
			}

			rf.mu.Unlock()

			select {
			case <-rf.grantVoteCh:
			case <-rf.heartBeatCh:
			case <-rf.timer.C:
				rf.mu.Lock()
				rf.convertToCandidate()
				rf.mu.Unlock()
			}

		}
	}

}

func (rf *Raft) convertToFollower(term int, voteFor int) {
	rf.currentTerm = term
	rf.state = Follower
	rf.votedFor = voteFor
	rf.voteGranted = 0
	rf.voteNotGranted = 0
	rf.persist()

	//fmt.Printf("Server %d: convert to Follower\n", rf.me)
}

func (rf *Raft) convertToCandidate() {
	rf.currentTerm++
	rf.state = Candidate
	rf.votedFor = rf.me
	rf.voteGranted = 1
	rf.voteNotGranted = 0

	electionTimeout := rand.Intn(200) + 200
	rf.timer.Reset(time.Duration(electionTimeout) * time.Millisecond)
	rf.persist()

	//fmt.Printf("Server %d: convert to Candidate\n", rf.me)
}

func (rf *Raft) convertToLeader() {
	rf.state = Leader
	rf.nextIndex = make([]int, len(rf.peers))
	rf.matchIndex = make([]int, len(rf.peers))
	for i := 0; i < len(rf.peers); i++ {
		rf.nextIndex[i] = len(rf.log)
		rf.matchIndex[i] = -1
	}
	rf.persist()
	//fmt.Printf("Server %d: convert to Leader\n", rf.me)
}

func (rf *Raft) startApplyLogs() {
	for rf.lastApplied < rf.committedIndex && rf.lastApplied+1 < len(rf.log) {
		//fmt.Printf("my id is %d, commitIndex = %d, lastApplied = %d\n", rf.me, rf.committedIndex, rf.lastApplied)
		//fmt.Printf("%v\n", rf.log)

		msg := ApplyMsg{}
		msg.CommandValid = true
		msg.CommandIndex = rf.lastApplied + 1

		msg.Command = rf.log[rf.lastApplied+1].Command

		// fmt.Println(msg)

		rf.applyCh <- msg
		rf.lastApplied++
	}
}

func GetGoroutineID() uint64 {
	b := make([]byte, 64)
	runtime.Stack(b, false)
	b = bytes.TrimPrefix(b, []byte("goroutine "))
	b = b[:bytes.IndexByte(b, ' ')]
	n, _ := strconv.ParseUint(string(b), 10, 64)
	return n
}
