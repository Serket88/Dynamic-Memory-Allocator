/* 
Jessica Fang
Colin Banigan
*/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "ackerman.h"
#include "my_allocator.h"

int main(int argc, char ** argv) {

    // input parameters (basic block size, memory length)
    const unsigned int M_DEFAULT = 512000;
	const int B_DEFAULT = 128;
	extern char *optarg;
	extern int optind;
	int debug, c, err = 0;
	int b_flag = 0;
	int s_flag = 0;
	int s_arg, b_arg;
	static char usage[] = "usage: %s [-b <blocksize>] [-s <memsize>]\n";

	while((c = getopt(argc, argv, "b:s:")) != -1)
		switch(c){
			case 'b':
				if(b_flag == 1){
					err = 999;
					break;
				}
				b_flag = 1;
				b_arg = atoi(optarg);
				break;
				
			case 's':
				if(s_flag == 1){
					err = 666;
					break;
				}
				s_flag = 1;
				s_arg = atoi(optarg);
				break;
				
			case '?':
				err = 1;
				break;
		}
	
	if (err > 0) {
		printf(usage, argv[0]);
		exit(1);
	}
	
	printf("b_flag = %d\n", b_flag);
	printf("s_flag = %d\n", s_flag);
	printf("b_arg = %d\n", b_arg);
    printf("s_arg = %d\n", s_arg);
    
    // init_allocator(basic block size, memory length)
    if(b_flag == 1 && s_flag == 1){
		printf("created a memory allocator with memory size %d and %d bytes per node.\n", s_arg, b_arg);
		init_allocator(s_arg,b_arg);
	}else if(b_flag == 1){
		printf("created a memory allocator with memory size %d and %d bytes per node.\n", M_DEFAULT, b_arg);
		init_allocator(b_arg,M_DEFAULT);
	}else if(s_flag == 1){
		printf("created a memory allocator with memory size %d and %d bytes per node.\n", s_arg, B_DEFAULT);
		init_allocator(B_DEFAULT,s_arg);
	}else{
		printf("created a memory allocator with memory size %d and %d bytes per node.\n", M_DEFAULT, B_DEFAULT);
		init_allocator(B_DEFAULT,M_DEFAULT);
	}

    ackerman_main();

    // release_allocator()
    release_allocator();
}
