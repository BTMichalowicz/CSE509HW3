#include "pin.H"

#include<iostream>
#include<fstream>
#include<map>
#include"header.h"
#include<sstream>
#include<bits/stdc++.h>
using namespace std;
using std::hex;
using std::cout;
using std::dec;;
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;
ofstream outFile; //Put it all into an output file

typedef enum{
  SyscallCallbackType_PIN_AddSyscallEntryFunction = 1,
  SyscallCallbackType_INS_InsertCall = 2
}SyscallCallbackType;
/**No outfile. Gonna print off function names for this*/

KNOB<string>KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "btraceApp.out", "specify trace file name");

bool syscall_encountered = false;
int num_threads = 0;
int num_calls = 0;
int num_rets = 0;
int num_instrumented = 0;
int syscalls_found = 0;

map<int,int> process_map;

PIN_LOCK pinLock;
/** General plan: Set up for every syscall put in, ensure to print out each instruction with pintools, ala strace.
 * Use dynamic argument printing in EAX, EBX, ECX, EDX
 */

string get_identifier(THREADID tid){
  stringstream buff;
  buff << PIN_GetTid()<<":" <<PIN_GetPid();
  return buff.str();
}
string syscall_decode(int syscallNum){
  switch(syscallNum){
    case SYS_read: return "read";
    case SYS_write: return "write";
    case SYS_readv: return "readv";
    case SYS_writev: return "writev";
    case SYS_open: return "open";
    case SYS_close: return "close";
    case SYS_openat: return "openat";
    case SYS_creat: return "creat";
    case SYS_getdents: return "getdents";
    case SYS_uname: return "uname";
    case SYS_getdents64: return "getdents64";
    case SYS_statfs64: return "statfs64";
    case SYS_fstatfs64: return "fstatfs64";
    case SYS_fstat64: return "fstat64";
    case SYS_access: return "access";
    case SYS_rename: return "rename";
    case SYS_mmap: return "mmap";
    case SYS_mmap2: return "mmap2";
    case SYS_execve: return "execve";
    case SYS_fadvise64: return "fadvise64";
    case SYS_brk: return "brk";
    case SYS_munmap: return "munmap";
    case SYS_mremap: return "mremap";
    case SYS_ioctl: return "ioctl";
    case SYS_mprotect: return "mprotect";
    case SYS_rt_sigaction: return "rt_sigaction";
    case SYS_rt_sigprocmask: return "rt_sigprocmask";
    case SYS_rt_sigpending: return "rt_sigpending";
    case SYS_rt_sigtimedwait: return "rt_sigtimedwait";
    case SYS_rt_sigreturn: return "rt_sigreturn";
    case SYS_clone: return "clone";
    case SYS_set_tid_address: return "set_tid_address";
    case SYS_get_robust_list: return "get_robust_list";
    case SYS_exit: return "exit(2)";
    case SYS_wait4: return "wait4";
    case SYS_stat64: return "stat64";
    case SYS_getuid32: return "getuid32";
    case SYS_getgid32: return "getgid32";
    case SYS_faccessat: return "faccessat";
    case SYS_getegid32: return "getegid32";
    case 243: return "set_thread_area";
    case 119: return "sigreturn";
    case 362: return "connect";
    case 363: return "listen";
    case SYS_geteuid32: return "euid32";
    case 102: return "socketcall";
    case 252: return "exit(2)";
    case 364: return "accept4";
    case 359: return "socket";
    case 360: return "socketpair";
    case 373: return "shutdown";
    case 140: return "llseek";
    case SYS_pipe2: return "pipe2";
    case SYS_futex: return "futex";
    case SYS_waitpid: return "waitpid";
    case SYS_poll: return "poll";
    case 277: return "mq_open";
    case SYS_pipe: return "pipe";
    case SYS_chdir: return "chdir";
    case SYS_fchdir: return "fchdir";
    case SYS_chroot: return "chroot";
    case SYS_fchmod: return "fchmod"; 
    case SYS_getpid: return "getpid";
    case SYS_getppid: return "getppid";
    case SYS_getuid: return "getuid";
    case SYS_geteuid: return "geteuid";
    case SYS_getgid: return "getgid";
    case SYS_ptrace: return "ptrace";
    case SYS_setpriority: return "setpriority";
    case SYS_getpriority: return "getpriority";
    case SYS_setuid: return "setuid";
    case 311: return "set_robust_list";
    case SYS_fork: return "fork";
    case SYS_fcntl: return "fcntl";
    case SYS_fcntl64: return "fcntl64";
    case SYS_link: return "link";
    case SYS_unlink: return "unlink";
    case SYS_fsync: return "fsync";
    case SYS_dup: return "dup";
    case SYS_dup2: return "dup2";
    case SYS_dup3: return "dup3";

    default:
	  stringstream ss;
	  ss << syscallNum;
	  return ss.str();

  }
  
}
void SyscallBefore(CONTEXT *ctxt, ADDRINT inst_ptr,THREADID tid/*, INT32 num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3,ADDRINT arg4, ADDRINT arg5*/){
  //syscall_encountered=true;
  //outFile << "Syscall found!\n";
  //
  //
  
      INT32 num =  PIN_GetContextReg(ctxt, REG_EAX);
      ADDRINT arg0 = PIN_GetContextReg(ctxt, REG_EBX);
      ADDRINT arg1 = PIN_GetContextReg(ctxt, REG_ECX);
      ADDRINT arg2 = PIN_GetContextReg(ctxt, REG_EDX);
      ADDRINT arg3 = PIN_GetContextReg(ctxt, REG_ESI);
      ADDRINT arg4 = PIN_GetContextReg(ctxt, REG_EDI);
      ADDRINT arg5 = PIN_GetContextReg(ctxt, REG_EBP);

#if defined(TARGET_LINUX) && defined(TARGET_IA32)
      if(num==SYS_mmap){
	ADDRINT * mmapArgs = reinterpret_cast<ADDRINT *>(arg0);
	arg0 = mmapArgs[0];
	arg1 = mmapArgs[1];
	arg2 = mmapArgs[2];
	arg3 = mmapArgs[3];
	arg4 = mmapArgs[4];
	arg5 = mmapArgs[5];
      }
#endif
  PIN_GetLock(&pinLock,  PIN_GetTid());
  outFile << "\n0x" <<hex << inst_ptr;
  outFile << ": "<< syscall_decode(num);
  //TODO: use the syscall number to print the appropriate number of arguments
  switch(num){
    case SYS_open:
    case SYS_stat:
    case SYS_access:
    case SYS_lstat:
    case SYS_creat:
    case SYS_chdir:
    case SYS_execve:
    case SYS_rename:
    case SYS_chroot:
    case SYS_unlink:
      outFile << "( arg0: \"" << (char*)(*(&arg0)) << "\"";
      break;
    default:

      outFile << "( arg0: " << hex << arg0;
      break;
  }

  switch(num){
    case SYS_openat:
    case SYS_rename:
    case SYS_renameat:
    case SYS_unlinkat:
      outFile<< ", arg1: " << (char*)(*(&arg1)) << "\n";
      break;
    default:
      outFile << ", arg1: " << hex << arg1;
      break;
  }
  outFile << ", arg2: " << hex << arg2;
  outFile << ", arg3: " << hex << arg3;
  outFile << ", arg4: " << hex << arg4;
  outFile << ", arg5: " << hex << arg5 << " )" <<endl;
  //outFile << "; idk: "  <<idk<<endl;
  //outFile << arg0 << endl;

  //TODO
  //
  num_calls++;
  syscall_encountered = true;
  process_map[PIN_GetTid()] += 1;
  PIN_ReleaseLock(&pinLock);
}

/*
void SyscallAfter(ADDRINT ret, ADDRINT num){
  if (syscall_encountered){
    outFile << "Return value: " << ret << endl << num << endl<<endl;
    //outFile << "; ERRNO: " << errno << endl <<endl;
    num_rets++;
    syscall_encountered = false;

  }
}
*/
void ProcessRet(CONTEXT * ctxt, THREADID tid){
  if (syscall_encountered){
    PIN_GetLock(&pinLock,  PIN_GetTid());
    outFile << "EAX Content: " << PIN_GetContextReg(ctxt, REG_EAX) << endl;
    //outFile << "Return value: " << PIN_GetSyscallReturn(ctxt, SYSCALL_STANDARD_IA32_LINUX) << endl;
    int err = PIN_GetSyscallErrno(ctxt, SYSCALL_STANDARD_IA32_LINUX);
    if(err!=0){
      outFile << "Error Number: " << dec << err << endl;

    }
    num_rets++;
    syscall_encountered = false;
    PIN_ReleaseLock(&pinLock);
  }
}

VOID Tracer(TRACE trace, VOID* v){
  for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl)){

      BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)ProcessRet, IARG_CONST_CONTEXT, IARG_THREAD_ID, IARG_END);
      num_instrumented++;
    

    for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins=INS_Next(ins)){
	if(INS_IsSyscall(ins)){
	  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SyscallBefore), IARG_CONST_CONTEXT, IARG_INST_PTR, IARG_THREAD_ID, IARG_END);
	  

          syscalls_found++;
	  break;
	}
    }
 }
}


VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v){
  PIN_GetLock(&pinLock, PIN_GetTid());
  //string identifier = get_identifier(threadid);
  cout << "THREAD FINISHED, tid: "<< threadid <<"; pid: "<<getpid()<<"; count: "<<process_map[PIN_GetTid()] <<endl;

  //cout << "****size of map: " << process_map.size() << endl;
  //process_map.erase(identifier);
  //flag_map.erase(identifier);

  //check to see if the maps are empty. If so, close everything down
  //if(process_map.empty() && flag_map.empty()){
  //  cout << "TOTAL CALLS FOUND: " << syscall_total<<endl;
  //  outFile.close();
  //}
  num_threads--;
  if( num_threads == 0){
    cout << "ALL THREADS DONE"<<endl;
  }
  PIN_ReleaseLock(&pinLock);
}

VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v){
  PIN_GetLock(&pinLock, PIN_GetTid());
  
  string identifier = get_identifier(threadid);

  //stringstream buff;
  //cout << "THREAD STARTED, tid: " << threadid << "; pid: "<<getpid()<<endl;
  //buff << threadid <<":" <<getpid();
  //cout << "Identifier" << buff.str()<<endl;
  cout << "THREAD STARTED WITH ID: " << identifier << endl;
  //process_map[threadid] = 0;
  //process_map[threadid] = false;
  num_threads++;
  process_map[PIN_GetTid()] = 0;

  cout << "num_threads: " <<num_threads<<endl;
  PIN_ReleaseLock(&pinLock);

}

VOID Fini(INT32 code, VOID* v){

  outFile << "Success" << endl;
  outFile << dec;
  outFile << "Number of calls: " << num_calls<<endl;
  outFile << "Number of returns: " << num_rets<<endl;
  outFile << "Blocks instrumented: " << num_instrumented<<endl;
  outFile << "Syscalls found: " << syscalls_found<<endl;
  outFile.close(); //Ben don't forget to close files!!!
}


INT32 Usage(VOID){
  cerr << "In conjunction with Pin, this tool performs system call interception without incurring too much overhead a la ptrace(2)" << endl;
  cerr << endl << KNOB_BASE::StringKnobSummary() << endl;

  return -1;
}



int main(int argc, char** argv){
  PIN_InitLock(&pinLock);
  /**Start with symbols to set up*/
  PIN_InitSymbols();
  if(PIN_Init(argc, argv)){
    return Usage(); //Create if failure occurs
  }

  outFile.open(KnobOutputFile.Value().c_str());
  outFile.setf(ios::showbase);
  
  TRACE_AddInstrumentFunction(Tracer,0);
  PIN_AddThreadStartFunction(ThreadStart,0);
  PIN_AddThreadFiniFunction(ThreadFini,0);
  //INS_AddInstrumentFunction(Instr, 0);
  /*PIN_AddSyscallEntryFunction(SyscallEntry,0);
    PIN_AddSyscallExitFunction(SyscallExit,0);*/
  //PIN_AddFiniFunction(Fini, 0);

  /**Shouldn't return*/
  PIN_StartProgram();

  return 0;
}
