//#include "header.h"
#include "pin.H"

#include <iostream>
#include<fstream>
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



/*Usage*/
static INT32 Usage(){

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


  return 0;
}
