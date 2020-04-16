//#include "header.h"
#include "pin.H"

#include <iostream>
#include<fstream>
using std::hex;
using std::dec;;
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;

ofstream outFile; /*Setup for the outfile*/

/*Command Line Switched*/
KOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", 
	 "o", "pinBlock.out", "specify trace file name");



/*Usage*/
static INT32 Usage(){

  cerr << "This pin tool collects data on every basic block executed\n";
  cerr << "Meaning, any control flow jump causes the end of a block, and post-jump";
  cerr << " begins another block\n";

  cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}



int main(int argc, char** argv){







  return 0;
}
