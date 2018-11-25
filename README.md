# Comp310-A3
Assignment 3 for comp 310: Opearing systems. (Phase 1)


### The TODO List short notes:


###### 1) .....
###### 2) From the outside (host), we can see the parent processes and the processes created and running after the unshare command with mounting the proc systemfile. From the inside (new container), we can only see the processes created and running inside the new namespace.  

###### 3) proc is the process info pseudo-filesystem that the kernel uses to provide status information about the status of the system, hence it is necessary to give new namespace access to the proc subsystem so certain commands like ps and top can work.

###### 4) I Couldn't see the processes running in the different namespace nor found out about its existence. also the inode number for Pid_for_children is different. Unless by coincident there were processes from the different containers with same pid, You can't kill a process running in a different container unless you do it from the host.

###### 5) 
