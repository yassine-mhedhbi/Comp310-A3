#define _GNU_SOURCE
#include <sched.h>	
#include <stdio.h>
#include <stdlib.h>

int main(){
	unshare(CLONE_NEWCGROUP | CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNET);
	//unshare(CLONE_NEWCGROUP | CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNET | CLONE_NEWUSER);
	int rc = 0;
	int pid = fork();
	if (pid != 0) {
        	int status;
        	waitpid(-1, &status, 0);
        	return status;
	}	

	printf(">>> My pid: %d\n", getpid());

}
