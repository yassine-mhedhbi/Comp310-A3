# Comp310-A3
Assignment 3 for comp 310: Operating systems. (Phase 1)


### The TODO List short notes:


###### 1) .....
###### 2) From the outside (host), we can see the parent processes and the processes created and running after the unshare command with mounting the proc systemfile. From the inside (new container), we can only see the processes created and running inside the new namespace.  

###### 3) proc is the process info pseudo-filesystem that the kernel uses to provide status information about the status of the system, hence it is necessary to give new namespace access to the proc subsystem so certain commands like ps and top can work.

###### 4) I Couldn't see the processes running in the different namespace nor found out about its existence. also the inode number for Pid_for_children is different. Unless by coincident there were processes from the different containers with same pid, You can't kill a process running in a different container unless you do it from the host.

###### 5) done and check notes.

###### 6) no it doesn't, the UTS namesapce isolates the hostname and domain

###### 7) Yes, there is no reason to not be able to. The step before chroot only creates a new root filesystem.

###### 8) Yes, we can.

###### 9) No, chroot changes the apparent root directory for the current process and its children. However, if they are nested, they we can from the parent to the child, we can create delete files from the parent and see them in the child root.


