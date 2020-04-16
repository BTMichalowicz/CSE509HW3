//#include "header.h"
#include "pin.H"

#include <iostream>
#include<fstream>
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;

ofstream outFile; /*Setup for the outfile*/
static unsigned long long totalMalloc=0; /*Keeping track of malloc-ed bytes*/
#if defined(TARGET_MAC)
#define MALLOC "_malloc"
#define FREE "_free"
#else
#define MALLOC "malloc"
#define FREE "free
#endif



/*Set up the Instrumentation Routing*/
VOID Image(IMG img, VOID* v){

  RTN mallocRtn = RTN_FindByName(img, MALLOC);
  if(RTN_Valid(mallocRtn)){
	 /*Valid Malloc*/
	 RTN_Open(mallocRtn);
	 /*Instrument malloc() to print input argument value and return value*/
	 RTN_InsertCall(mallocRtn, IPOINT_BEFORE, (AFUNPTR)Arg1Before,
		  IARG_ADDRINT, MALLOC,
		  IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
		  IARG_END);
	 RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)MallocAfter,
		  IARG_FUNCRET_EXITPOINT_VALUE, IARGEND);
	 RTN_Close(mallocRtn);
  }

  /* Find the Free Function*/
  RTN freeRtn = RTN_FindByName(img, FREE);


}

void Fini(INT32 code, VOID* v){ outFile.close();} /*We're done here*/

INT32 Usage(){
  cerr << "This tool produces traces of calls to malloc and keeps track of the number of bytes malloced" << end;
  cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
  return -1;
}

int main(int argc, char** argv){
  /*Setup*/
  PIN_InitSymbols();
  if(PIN_INIT(argc,argv)){
	 return Usage();
  }

  /*Write to files just in case file descriptors are close*/
  outFile.open(KnobOutputfile.Value().c_str());
  outFile << hex;
  outFile.set(ios::showbase);

  /*Register an image to be called to instrument fxn*/
  IMG_AddInstrumentFunction(Image,0);
  PIN_AddFiniFunction(Fini, 0);

  /**Start*/
  PIN_StartProgram();
  
  return 0;
}
