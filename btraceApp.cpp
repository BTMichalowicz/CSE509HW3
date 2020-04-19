#include "pin.H"
#include "./header.h"
#include<iostream>
#include<fstream>
#include<map>
#include<unistd.h>
/*#include<sys/man.h>
#include<sys/syscall.h>
#include<sys/user.h>
#include<sys/mman.h>
#include<sys/reg.h>*/
using std::hex;
using std::cout;
using std::dec;;
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;

/**No outfile. Gonna print off function names for this*/


/** General plan: Set up for every syscall put in, ensure to print out each instruction with pintools, ala strace.
 * Use dynamic argument printing in EAX, EBX, ECX, EDX
 */

VOID Instr(INS ins, VOID* v){
  if(INS_IsSyscall(ins)){
	 cerr << 4*EAX <<endl;
	 cerr << 4*EBX << endl;
	 cerr << 4*ECX <<endl;
	 cerr << 4*EDX <<endl;
	 
  }
}


VOID Fini(INT32 code, VOID* v){

  cerr << "Success" << endl;
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


  INS_AddInstrumentFunction(Instr, 0);
  PIN_AddFiniFunction(Fini, 0);

  /**Shouldn't return*/
  PIN_StartProgram();

  return 0;
}
