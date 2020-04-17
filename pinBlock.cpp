//#include "header.h"
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
INT64 basicBlockCount=0;

/*Command Line Switched*/
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", 
	 "o", "pinBlock.out", "specify trace file name");



/*Instrumentation Function*/
VOID takeBlock1(ADDRINT dummy){
  basicBlockCount++;
}
VOID takeBlock2(ADDRINT dummy2){
  basicBlockCount++;
}

VOID Instr(INS instr, VOID* v){
  
  if(INS_IsDirectControlFlow(instr)){
	 //Show that we took a branch and broke into the next block*/
	 INS_InsertCall(instr, IPOINT_TAKEN_BRANCH, (AFUNPTR)takeBlock1, IARG_ADDRINT, &basicBlockCount, IARG_END);
  }
  if(INS_IsIndirectControlFlow(instr)){
	 if(INS_IsRet(instr)){
		return;
	 }
	 INS_InsertCall(instr, IPOINT_BEFORE, (AFUNPTR)takeBlock2,
		  IARG_ADDRINT, &basicBlockCount, IARG_END);
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

void Fini(INT32 code, VOID* v){
  outFile << "Number of Basic Blocks executed" << basicBlockCount << endl;
  outFile.close();
}



int main(int argc, char** argv){

  /*Set up like pinMalloc*/
  PIN_InitSymbols();
  if(PIN_Init(argc, argv)){
	 return Usage();
  }

  outFile.open(KnobOutputFile.Value().c_str());
  outFile << dec;
  outFile.setf(ios::showbase);

  INS_AddInstrumentFunction(Instr, 0);
  PIN_AddFiniFunction(Fini,0);

  /**Let's go!*/
  PIN_StartProgram();


  return 0;
}
