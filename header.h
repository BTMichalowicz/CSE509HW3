#ifndef __HEADER_H__
#define __HEADER_H__

//Purpose: To include everything so you don't have to!
#define _GNU_SOURCE_
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<stddef.h>
#include<stdint.h>
#include<inttypes.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>
#include<assert.h>
#include<syscall.h>
#include<sys/stat.h>
#include <sys/socket.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/mman.h>
#include<sys/ptrace.h>
#include<sys/syscall.h>
#include<sys/user.h>
#include<sys/reg.h>
#include<syscall.h>

#define sys2(x) sysDecode(x)
#define sysDecode(x) #x

#define FATAL(...) \
    do{ \
        fprintf(stderr, "urlextend trace: " __VA_ARGS__);\
        fputc('\n', stderr); \
        exit(EXIT_FAILURE); \
    }while (0);
/*Above: for fatal elements*/


typedef unsigned int uint;
typedef unsigned char uchar;

/* TODO:
 * Make functions for each part
 */


/*************** Part1: Pintools *********************/


/************** Part 2: Security App ****************/


/************* Part 3: Shadow Stack ****************/


#endif
