#include "pin.H"

#include<iostream>
#include<fstream>
#include<map>
#include"header.h"
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

KNOB<string>KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "btrace.out", "specify trace file name");

bool syscall_encountered = false;

/** General plan: Set up for every syscall put in, ensure to print out each instruction with pintools, ala strace.
 * Use dynamic argument printing in EAX, EBX, ECX, EDX
 */
#if 0
VOID SyscallBefore(ADDRINT ip, ADDRINT num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5, ADDRINT type0){
  /*Plan: Compare decoding of different system calls upon entering them*/
  //SyscallCallbackType type = (SyscallCallbackType)type0;

  ADDRINT* args; //For larger argument values;
  if(num==SYS_mmap){
#if defined(TARGET_LINUX) && defined(TARGET_IA32)

	 /*Mmap syscall*/
	 args = reinterpret_cast<ADDRINT*>(arg0);
	 arg0 = args[0];
	 arg1 = args[1];
	 arg2 = args[2];
	 arg3 = args[3];
	 arg4 = args[4];
	 arg5 = args[5];
	 
	 outFile << "Arg0: " << arg0 << endl;	 
	 outFile << "Arg1: " << arg1 << endl;	
	 outFile << "Arg2: " << arg2 << endl;	
	 outFile << "Arg3: " << arg3 << endl;	
	 outFile << "Arg4: " << arg4 << endl;	
	 outFile << "Arg5: " << arg5 << endl;	
#endif
  }else{
	 switch(num){
		case SYS_open:
		case SYS_access:	 
		case SYS_stat:
		case SYS_statfs:
		case SYS_statfs64:
		  //TODO: Turn arguments into proper types
		  outFile << "Arg0: " << num << endl;
		  outFile << "Arg1: " << arg0 << endl;
		  outFile << "Arg2: " << arg1 << endl;
		  break;
		case SYS_read:
		case SYS_write:
		case SYS_mprotect:
		case SYS_waitpid:
		case SYS_rt_sigaction:
		case SYS_rt_sigprocmask:
		case 359:
		case 362:
		case SYS_open_by_handle_at:
		case SYS_openat:
		  outFile << "Arg0: " << num << endl;
		  outFile << "Arg1: " << arg0 << endl;
		  outFile << "Arg2: " << arg1 << endl;
		  outFile << "Arg3: " << arg2 << endl;
		  break;
		case SYS_brk:
		case SYS_close:
		case SYS_set_thread_area:
		case SYS_wait4:
		case SYS_uname:
		  outFile << "Arg0: " << num << endl;
		  outFile << "Arg1: " << arg0 << endl;
		  break;
		  

		default:
		  outFile << "TODO: identify SYSCALL: " << num << "\n";
	 }
  }

}

VOID SyscallAfter(ADDRINT ip, ADDRINT num, ADDRINT arg0){
  outFile << "Return value: " << arg0 << endl;
}

#endif

void SyscallBefore(ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3,ADDRINT arg4, ADDRINT arg5){
  syscall_encountered=true;

  cout << arg0 << endl;

  //TODO
  //
}

void SyscallAfter(ADDRINT ret){
  cout << ret << endl;
}

VOID Tracer(TRACE trace, VOID* v){
  for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl)){
	 if(syscall_encountered){
		BBL_InsertCall(bbl,IPOINT_BEFORE, (AFUNPTR)SyscallAfter, IARG_ADDRINT, 24, IARG_END);
	 }


	 //TODO
  }
}

VOID Instr(INS ins, VOID* v){
 

  if(INS_IsSyscall(ins)){
	 
	 INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SyscallBefore), IARG_INST_PTR, 
		  IARG_SYSARG_VALUE, 0,
		  IARG_SYSARG_VALUE, 1,
		  IARG_SYSARG_VALUE, 2,
		  IARG_SYSARG_VALUE, 3,
		  IARG_SYSARG_VALUE, 4,
		  IARG_SYSARG_VALUE, 5,
		  IARG_ADDRINT, (ADDRINT)SyscallCallbackType_INS_InsertCall,
		  IARG_END);

	//INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)SyscallAfter, 
	//	  IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);
	 
  }
}


VOID Fini(INT32 code, VOID* v){

  outFile << "Success" << endl;
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
  INS_AddInstrumentFunction(Instr, 0);
  /*PIN_AddSyscallEntryFunction(SyscallEntry,0);
  PIN_AddSyscallExitFunction(SyscallExit,0);*/
  PIN_AddFiniFunction(Fini, 0);

  /**Shouldn't return*/
  PIN_StartProgram();

  return 0;
}
