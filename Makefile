CC=g++
CFLAGS=-Wall	-Werror	-g	#-O2 or -O3
LDFLAGS=-lm	-lpthread

all: pinBlock #pinMalloc pinCFT btraceFunc shadowStack


pinBlock: 
	$(CC) $(CFLAGS) -o pinBlock pinBlock.cpp $(LDFLAGS)

#pinMalloc:
#	$(CC) $(CFLAGS) -o pinMalloc pinMalloc.cpp $(LDFLAGS)

#pinCFT:
#	$(CC) $(CFLAGS) -o pinCFT pinCFT.cpp $(LDFLAGS)

#btraceFunc:
#	$(CC) $(CFLAGS) -o btraceFunc btraceFunc.cpp $(LDFLAGS)

#stackPivot:
#	$(CC) $(CFLAGS) -o stackPivot stackPivot.cpp $(LDFLAGS)

#shadowStack:
#	$(CC) $(CFLAGS) -o shadowStack  shadowStack.cpp $(LDFLAGS)



.PHONY : clean

clean:
	rm -f pinBlock pinMalloc pinCFT btraceFunc stackPivot shadowStack
