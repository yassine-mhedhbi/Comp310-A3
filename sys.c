#define _GNU_SOURCE
#include <sched.h>	
#include <stdio.h>
#include <stdlib.h>

int main(){
	unshare(CLONE_NEWPID | CLONE_NEWNS );
	int rc = 0;
	int pid = fork();
	if (pid != 0) {
        	int status;
        	waitpid(-1, &status, 0);
        	return status;
	}	

	printf(">>> My pid: %d\n", getpid());

}
