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
INT64 directControlFlow =0, indirectControlFlow=0,branch=0;

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
VOID indirectCount(ADDRINT dummy){
  indirectFlow+=1;
}
VOID branchCount(ADDRINT dummy){
  ++branch;
}

VOID Fini(INT32 code, VOID* v){
  outFile << "Direct Control Transfer Count: " << directControlFlow << endl;
  outFile << "Indirect Control Transfer Count: " << indirectControlFlow << endl;;
  outFile << "Branch Count: " << branch << endl;
  outFile.close();
}

VOID Instrumemtation(INS instr, VOID* v){
  if(INS_IsDirectControlFlow(instr)){
	 INS_Insert
