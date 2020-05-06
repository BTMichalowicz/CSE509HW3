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

int num_calls = 0;
int num_rets = 0;
int num_instrumented = 0;
int syscalls_found = 0;
/** General plan: Set up for every syscall put in, ensure to print out each instruction with pintools, ala strace.
 * Use dynamic argument printing in EAX, EBX, ECX, EDX
 */

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
    case 362: return "connect";
    case 363: return "listen";
    
    case 364: return "accept4";
    case 359: return "socket";
    case 360: return "socketpair";
    case 373: return "shutdown";
    case 140: return "llseek";
    case SYS_pipe2: return "pipe2";
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

    default:
	  stringstream ss;
	  ss << syscallNum;
	  return ss.str();

  }
  
}
    void SyscallBefore(ADDRINT inst_ptr, INT32 num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3,ADDRINT arg4, ADDRINT arg5){
  //syscall_encountered=true;
  //outFile << "Syscall found!\n";
  outFile << "Num: " <<syscall_decode(num);
  //TODO: use the syscall number to print the appropriate number of arguments  
  outFile << "; Arg0: " <<arg0;
  outFile << "; Arg1: " <<arg1;
  outFile << "; arg2: " <<arg2;
  outFile << "; arg3: " <<arg3;
  outFile << "; arg4: " <<arg4;
  outFile << "; arg5: " <<arg5<<endl;
  //outFile << "; idk: "  <<idk<<endl;
  //outFile << arg0 << endl;

  //TODO
  //
  num_calls++;
  syscall_encountered = true;
}


void SyscallAfter(ADDRINT ret, ADDRINT num){
  if (syscall_encountered){
    outFile << "Return value: " << ret << endl << num << endl;
    //outFile << "; ERRNO: " << errno << endl <<endl;
    num_rets++;
    syscall_encountered = false;

  }
}

void ProcessRet(CONTEXT * ctxt){
  if (syscall_encountered){

    outFile << "EAX Content: " << PIN_GetContextReg(ctxt, REG_EAX) << endl;
    num_rets++;
    syscall_encountered = false;
  }
}

VOID Tracer(TRACE trace, VOID* v){
  for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl)){

    //if(syscall_encountered==true){
      //BBL_InsertCall(bbl,IPOINT_BEFORE, (AFUNPTR)SyscallAfter, IARG_SYSRET_VALUE, IARG_SYSRET_ERRNO, IARG_END);

      //INS_InsertCall(BBL_InsHead(bbl), IPOINT_AFTER, (AFUNPTR)SyscallAfter, IARG_SYSRET_VALUE, IARG_SYSRET_ERRNO, IARG_END);
      BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)ProcessRet, IARG_CONST_CONTEXT, IARG_END);
      //syscall_encountered=false;
      num_instrumented++;
    //}
	//TODO: try passing the CONTEXT variable and getting the value of REG_EAX




    for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins=INS_Next(ins)){
	if(INS_IsSyscall(ins)){
	  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SyscallBefore), 
	      IARG_INST_PTR,
	      IARG_SYSCALL_NUMBER, 
	      IARG_SYSARG_VALUE, 0,
	      IARG_SYSARG_VALUE, 1,
	      IARG_SYSARG_VALUE, 2,
	      IARG_SYSARG_VALUE, 3,
	      IARG_SYSARG_VALUE, 4,
	      IARG_SYSARG_VALUE, 5,
	      IARG_ADDRINT, (ADDRINT)SyscallCallbackType_INS_InsertCall,
	      IARG_END);
	  //syscall_encountered=true;
          syscalls_found++;
	  break;
	}
    }
 }
}

VOID Instr(INS ins, VOID* v){


  if(INS_IsSyscall(ins)){

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SyscallBefore), IARG_INST_PTR, IARG_SYSCALL_NUMBER,
	IARG_SYSARG_VALUE, 0,
	IARG_SYSARG_VALUE, 1,
	IARG_SYSARG_VALUE, 2,
	IARG_SYSARG_VALUE, 3,
	IARG_SYSARG_VALUE, 4,
	IARG_SYSARG_VALUE, 5,
	IARG_END);

    //INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)SyscallAfter, IARG_SYSRET_VALUE, IARG_END);

  }
}


VOID Fini(INT32 code, VOID* v){

  outFile << "Success" << endl;
  outFile << "Number of calls: " << num_calls<<endl;
  outFile << "Number of returns: " << num_rets<<endl;
  outFile << "Blocks instrumented: " << num_instrumented<<endl;
  outFile << "Syscalls found: " << syscalls_found<<endl;
}





INT32 Usage(VOID){
  cerr << "In conjunction with Pin, this tool performs system call interception without incurring too much overhead a la ptrace(2)" << endl;
  cerr << endl << KNOB_BASE::StringKnobSummary() << endl;

  return -1;
}



int main(int argc, char** argv){

  /**Start with symbols to set up*/
  PIN_InitSymbols();
  if(PIN_Init(argc, argv)){
    return Usage(); //Create if failure occurs
  }

  outFile.open(KnobOutputFile.Value().c_str());
  outFile.setf(ios::showbase);
  TRACE_AddInstrumentFunction(Tracer,0);
  //INS_AddInstrumentFunction(Instr, 0);
  /*PIN_AddSyscallEntryFunction(SyscallEntry,0);
    PIN_AddSyscallExitFunction(SyscallExit,0);*/
  PIN_AddFiniFunction(Fini, 0);

  /**Shouldn't return*/
  PIN_StartProgram();

  return 0;
}
