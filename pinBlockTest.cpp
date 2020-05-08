//#include "header.h"
#include "pin.H"
#include "instlib.H"
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

//INSTLIB::FOLLOW_CHILD follow;
ofstream outFile; /*Setup for the outfile*/
static INT64 basicBlockCount=0;

/*Command Line Switched*/
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", 
	 "o", "pinBlock.out", "specify trace file name");



/*Instrumentation Function*/
VOID takeBlock(ADDRINT dummy){
  basicBlockCount++;
}

VOID Instr(TRACE instr, VOID* v){
  
  for (BBL bbl = TRACE_BblHead(instr); BBL_Valid(bbl); bbl = BBL_Next(bbl)){
	 BBL_InsertCall(bbl, IPOINT_ANYWHERE, (AFUNPTR)(takeBlock), IARG_FAST_ANALYSIS_CALL, IARG_END);
         //basicBlockCount++;
  }

}

/*Usage*/
INT32 Usage(){

  cerr << "This pin tool collects data on every basic block executed\n";
  cerr << "Meaning, any control flow jump causes the end of a block, and post-jump";
  cerr << " begins another block\n";

  cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

bool FollowChild(CHILD_PROCESS childProcess, VOID * userData) {
  //bool res;
  int appArgc;
  char const * const * appArgv;
  OS_PROCESS_ID pid = CHILD_PROCESS_GetId(childProcess);
  // Get the command line that child process will be Pinned with, these are the Pin invocation switches
  // that were specified when this (parent) process was Pinned
  CHILD_PROCESS_GetCommandLine(childProcess, &appArgc, &appArgv);
  //int childArgc = 0;
  //char const * childArgv[20];
  
  //TODO: How to alter the argv such that it runs with pin
  //../../../pin -follow_execv -t obj-ia32/pinBlock.so --

  //childArgv[childArgc++] = "../../../pin";
  //childArgv[childArgc++] = "-follow_execv";
  //childArgv[childArgc++] = "-t";
  //childArgv[childArgc++] = "obj-ia32/pinBlock.so";
  //childArgv[childArgc++] = "--";
  //CHILD_PROCESS_SetPinCommandLine(childProcess, childArgc, childArgv);
/*
  int n = 0;
  while(*(*childArgv+n)){
    outFile << *(*childArgv+n);
    n++;
  }
*/
  outFile << pid;
  outFile << endl;
  return true; /* Specify Child process is to be Pinned */
}

void Fini(INT32 code, VOID* v){
  outFile << "Number of Basic Blocks executed: " << basicBlockCount << endl;
  outFile.close();
}



int main(int argc, char** argv){

  /*Set up like pinMalloc*/
  PIN_InitSymbols();
  if(PIN_Init(argc, argv)){
	 return Usage();
  }
  
  //follow.Activate();
  //follow.SetPrefix(argv);

  outFile.open(KnobOutputFile.Value().c_str());
  outFile << dec;
  outFile.setf(ios::showbase);

  TRACE_AddInstrumentFunction(Instr, 0);
  PIN_AddFollowChildProcessFunction (FollowChild, 0);
  PIN_AddFiniFunction(Fini,0);

  /**Let's go!*/
  PIN_StartProgram();

  return 0;
}
