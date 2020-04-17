#include "pin.H"

#include <iostream>
#include<fstream>
#include<map>
#include<unistd.h>
using std::hex;
using std::dec;;
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;

ofstream outFile; /*Setup for the outfile*/

/*Main sets of calls, branches, direct/indirect calls*/
INT64 directControlFlow =0,branch=0;
INT64 syscalls=0, funCalls=0;
INT64 returns=0;

/*Command Line Switched*/
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", 
	 "o", "pinCtrl.out", "specify trace file name");

INT32 Usage(){
  cerr<< "This pintool is meant to collect counters for:" <<endl;
  cerr<< "1) In/direct Control Flows" << endl;
  cerr<< "2) Any and all types of branches, such as return statements" <<endl;
  cerr << KNOB_BASE::StringKnobSummary() << endl;
  return -1;
}

VOID directCount(ADDRINT dummy){
  directControlFlow++;
}
VOID branchCount(ADDRINT dummy){
  ++branch;
}
VOID syscallCount(ADDRINT dummy){
  ++syscalls;
}
VOID funCallCount(ADDRINT dummy){
  funCalls++;
}
VOID returnCount(ADDRINT dummy){
  returns++;
}

VOID Fini(INT32 code, VOID* v){
  outFile << "Direct Control Transfer Count: " << directControlFlow << endl;
  outFile << "(Indirect CFT) Branch Count: " << branch << endl;
  outFile << "(Indirect CFT) Syscall Count: " << syscalls << endl;
  outFile << "(Indirect CFT) Func. Call Count: " << funCalls << endl;
  outFile << "Return statement count: " <<returns << endl;
  outFile.close();
}

VOID Instrumentation(INS instr, VOID* v){
  if(INS_IsDirectControlFlow(instr)){

	 INS_InsertCall(instr, IPOINT_TAKEN_BRANCH, (AFUNPTR)directCount, IARG_ADDRINT, &directControlFlow, IARG_END);
	 return;

  }else if(INS_IsIndirectControlFlow(instr)){

	 if(INS_IsRet(instr)){
		INS_InsertCall(instr, IPOINT_BEFORE, (AFUNPTR) returnCount, IARG_ADDRINT, &returns, IARG_END);
		return;
		

	 }else if(INS_IsCall(instr)){
		INS_InsertCall(instr, IPOINT_BEFORE, (AFUNPTR) funCallCount, IARG_ADDRINT, &funCalls, IARG_END);
		return;

	 }else{
		INS_InsertCall(instr, IPOINT_BEFORE, (AFUNPTR) branchCount, IARG_ADDRINT, &branch, IARG_END);
		return;

	 }

	 
  }else if(INS_IsSyscall(instr)){
	 INS_InsertPredicatedCall(instr, IPOINT_BEFORE, (AFUNPTR)syscallCount, IARG_ADDRINT, &syscallCount, IARG_END);
	 return;
  }
}


int main(int argc, char** argv){
  PIN_InitSymbols();
  if(PIN_Init(argc, argv)){
	 return Usage();
  }
  outFile.open(KnobOutputFile.Value().c_str());
  outFile << dec;
  outFile.setf(ios::showbase);

  INS_AddInstrumentFunction(Instrumentation, 0);
  PIN_AddFiniFunction(Fini,0);

  PIN_StartProgram();

  return 0;
}
