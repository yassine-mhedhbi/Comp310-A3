Assignment 2 part 2
===================

By: Yassine Mhedhbi and Michael Wolman

The following are notes on the five parts of the containers implementation. Parts
2, 3, and 4 really don't say anything interesting. Part 1 comments on how we
implemented adding cgroup options, but nothing too interesting, mostly small details.
Part 5 comments on how we implemented syscall blocking, and contains some details
on problems (and solutions) for issues blocking some of the syscalls. At the end
we have a recap of what has been done, mostly for our use.

1 Reading command line arguments
--------------------------------

The `cgroups` array is a NULL-terminated list of cgroup options, with one entry
per cgroup category (each entry contains all the options for that category).

The default is left alone, and so the array contains a spot for `blkio` to start.
As we parse flags, we add entries to the array for the category corresponding to
the flag. We check if the flag exists by looping over the array, and if it doesn't
exist we make it. We then search through the options in the entry for the one we
want to modify, adding it if it hasn't been set, and set it as appropriate.

We originally wanted to hard-code the `cgroups` array to skip the entry search
and only have to search for the option (if it exists), or even to hard-code
the entry and the options and not do any searching. This would have led to shorter,
cleaner code, but wouldn't work with how the included `setup_cgroup_controls()`
function is implemented. Not a big deal. Also would have bundled some of the logic
into a separate function, but didn't want to mess too much with the template given.

Also, note that for the `cpuset` cgroup, we need to set the `cpuset.cpus` and
`cpuset.mems` parameters of the cgroup before we add anything to it (otherwise
the kernel throws errors), and due to how `setup_cgroup_controls()` is implemented,
we have to add them to the options before adding `self_to_task`. This isn't an
issue for any of the other cgroups, which is why the `cpuset` cgroup is coded
slightly differently. This may be a problem if more flags or cgroups are implemented
though, so be warned.

All flags have been tested in a Debian docker container on the cs310 server, both
alone and together.

2 Creating child process
------------------------

Standard `clone` shit. Flags used are:

* `SIGCHLD` - as requested, for signaling when the child dies;
* `CLONE_NEWCGROUP` - put the child in a new cgroup namespace;
* `CLONE_NEWIPC` - put the child in a new IPC namespace;
* `CLONE_NEWNS` - put the child in a new mount namespace;
* `CLONE_NEWPID` - put the child in a new PID namespace;
* `CLONE_NEWUTS` - put the child in a new UTS namespace;
* `CLONE_NEWNET` - put the child in a new network namespace.

3 Changing root
---------------

Just `syscall(SYS_pivot_root, new_root, put_old)`, nothing fancy.

4 Setting capabilities
----------------------

As requested, we disabled the following capabilities, with the rest allowed:
`CAP_AUDIT_CONTROL`, `CAP_AUDIT_READ`, `CAP_AUDIT_WRITE`, `CAP_BLOCK_SUSPEND`,
`CAP_DAC_READ_SEARCH`, `CAP_FSETID`, `CAP_IPC_LOCK`, `CAP_MAC_ADMIN`,
`CAP_MAC_OVERRIDE`, `CAP_MKNOD`, `CAP_SETFCAP`, `CAP_SYSLOG`, `CAP_SYS_ADMIN`,
`CAP_SYS_BOOT`, `CAP_SYS_MODULE`, `CAP_SYS_NICE`, `CAP_SYS_RAWIO`,
`CAP_SYS_RESOURCE`, `CAP_SYS_TIME`, `CAP_WAKE_ALARM`.

Literally a copy-paste of the logic in the handout.

5 Disabling syscalls
--------------------

Again a copy-paste of the logic in the handout. Implemented and tested:

* `ptrace`, blocked for any input and flags;
* `mbind`, blocked for any input and flags;
* `migrate_pages`, blocked for any input and flags;
* `move_pages`, blocked for any input and flags;
* `unshare`, blocked for only for the `CLONE_NEWUSER` flag;
* `clone`, blocked for only for the `CLONE_NEWUSER` flag;
* `chmod`, blocked for only for the `S_ISUID` and `S_ISGID` flags.

Remark: for `clone`, the third argument of the `glibc` library function, and the
third argument as documented basically everywhere, are the flags. However, the
flags are given to the actual `clone` *syscall* as the first argument, as can be
seen in the kernel course (and in the `glibc` `clone` wrapper implementation).
This means we need to use `SCMP_A0` and not `SCMP_A2`, which took a long time to
debug and figure out and is really annoying.

Remark: for `chmod`, we need to block the `chmod`, `fchmod`, and `fchmodat` syscalls.
These are not just different wrappers around the same syscall, but actually
different syscalls, and so if we only block `chmod`, then many programs (like
`chmod`, the coreutils binary) will still work, since they use the other two
syscalls. (In fact, the `chmod` coreutils binary actually uses the `fchmodat`
syscall, so it would still work if only `chmod` is blocked.)

Recap
-----

✅ Setting up cgroups/hostname flags;

✅ Implementing child process logic;

✅ Proper usage of​ pivot_root()​;

✅ Implementing capabilities;

✅ Implementing syscall filtering.

---

✅ Testing all flags, both alone and together;

✅ Testing capabilities using `capsh`.

---

✅ Testing `ptrace`;

✅ Testing `mbind`;

✅ Testing `migrate_pages`;

✅ Testing `move_pages`;

✅ Testing `unshare` without the `CLONE_NEWUSER` flag;

✅ Testing `unshare` with the `CLONE_NEWUSER` flag;

✅ Testing `clone` without the `CLONE_NEWUSER` flag;

✅ Testing `clone` with the `CLONE_NEWUSER` flag;

✅ Testing `chmod` without the `S_ISUID` and `S_ISGID` flags;

✅ Testing `chmod` with the `S_ISUID` or the `S_ISGID` flag;

✅ Testing `fchmod` and `fchmodat` (and the `chmod` binary) the same way as `chmod`.
