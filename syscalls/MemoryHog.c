
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>



int main(){
	int i;
	long sz = 100000;

	mlockall( MCL_FUTURE | MCL_ONFAULT);

	for (i = 0; i< 100000; i++) {
		printf("Allocaion block %d \n", i);
		fflush(stdout);
		char *p = malloc(sz);
		if (p==NULL) {
			printf("error .. \n");
			fflush(stdout);
		} else {
			printf("success ..\n");
			fflush(stdout);

		}

		memset(p, 0, sz);
		usleep(10000);
	}
	return 0;
}