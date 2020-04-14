CC=gcc
CFLAGS=-Wall	-Werror	-g	#-O2 or -O3
LDFLAGS=-lm	-lpthread

all: pinBlock pinMalloc pinCFT btraceFunc shadowStack


pinBlock: 
	$(CC) $(CFLAGS) -o pinBlock pinBlock.c $(LDFLAGS)

pinMalloc:
	$(CC) $(CFLAGS) -o pinMalloc pinMalloc.c $(LDFLAGS)

pinCFT:
	$(CC) $(CFLAGS) -o pinCFT pinCFT.c $(LDFLAGS)

btraceFunc:
	$(CC) $(CFLAGS) -o btraceFunc btraceFunc.c $(LDFLAGS)

shadowStack:
	$(CC) $(CFLAGS) -o shadowStack  shadowStack.c $(LDFLAGS)




