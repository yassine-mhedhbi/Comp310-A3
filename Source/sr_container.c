/**
 *  @title      :   sr_container.c
 *  @author     :   Shabir Abdul Samadh (shabirmean@cs.mcgill.ca)
 *  @date       :   20th Nov 2018
 *  @purpose    :   COMP310/ECSE427 Operating Systems (Assingment 3) - Phase 2
 *  @description:   A template C code to be filled in order to spawn container instances
 *  @compilation:   Use "make container" with the given Makefile
*/

#include "sr_container.h"

/**
 *  The cgroup setting to add the writing task to the cgroup
 *  '0' is considered a special value and writing it to 'tasks' asks for the wrinting
 *      process to be added to the cgroup.
 *  You must add this to all the controls you create so that it is added to the task list.
 *  See the example 'cgroups_control' added to the array of controls - 'cgroups' - below
 **/
struct cgroup_setting self_to_task = {
    .name = "tasks",
    .value = "0"
};

/**
 *  ------------------------ TODO ------------------------
 *  An array of different cgroup-controllers.
 *  One controller has been been added for you.
 *  You should fill this array with the additional controls from commandline flags as described
 *      in the comments for the main() below
 *  ------------------------------------------------------
 **/

/*
 * This is a NULL-terminated list, and entries will be filled as needed.
 */
struct cgroups_control *cgroups[6] = {
    // blkio
    & (struct cgroups_control) {
        .control = CGRP_BLKIO_CONTROL,
        .settings = (struct cgroup_setting *[8]) {

            & (struct cgroup_setting) {
                .name = "blkio.weight",
                .value = "64"
            },
            &self_to_task,             // must be added to all the new controls added
            NULL                       // NULL at the end of the array
        }
    },
    NULL                               // NULL at the end of the array
};


/**
 *  ------------------------ TODO ------------------------
 *  The SRContainer by default suppoprts three flags:
 *          1. m : The rootfs of the container
 *          2. u : The userid mapping of the current user inside the container
 *          3. c : The initial process to run inside the container
 *
 *   You must extend it to support the following flags:
 *          1. C : The cpu shares weight to be set (cpu-cgroup controller)
 *          2. s : The cpu cores to which the container must be restricted (cpuset-cgroup controller)
 *          3. p : The max number of process's allowed within a container (pid-cgroup controller)
 *          4. M : The memory consuption allowed in the container (memory-cgroup controller)
 *          5. r : The read IO rate in bytes (blkio-cgroup controller)
 *          6. w : The write IO rate in bytes (blkio-cgroup controller)
 *          7. H : The hostname of the container
 *
 *   You can follow the current method followed to take in these flags and extend it.
 *   Note that the current implementation necessitates the "-c" flag to be the last one.
 *   For flags 1-6 you can add a new 'cgroups_control' to the existing 'cgroups' array
 *   For 7 you have to just set the hostname parameter of the 'child_config' struct in the header file
 *  ------------------------------------------------------
 **/
int main(int argc, char **argv)
{
    struct child_config config = {0};
    int option = 0;
    int sockets[2] = {0};
    pid_t child_pid = 0;
    int last_optind = 0;
    bool found_cflag = false;
    char *stack;
    while ((option = getopt(argc, argv, "c:m:u:C:s:p:M:r:w:H:")))
    {
        size_t n, i; // For cgroup_control and settings arrays

        if (found_cflag)
            break;

        switch (option)
        {
        case 'c':
            config.argc = argc - last_optind - 1;
            config.argv = &argv[argc - config.argc];
            found_cflag = true;
            break;

        case 'm':
            config.mount_dir = optarg;
            break;

        case 'u':
            if (sscanf(optarg, "%d", &config.uid) != 1)
            {
                fprintf(stderr, "UID not as expected: %s\n", optarg);
                cleanup_stuff(argv, sockets);
                return EXIT_FAILURE;
            }
            break;

        case 'C':
            // Find cgroup control
            for (n = 0; n < 5; n++) {
                if (cgroups[n] == NULL)
                    break;
                else if (strcmp(cgroups[n]->control, CGRP_CPU_CONTROL) == 0)
                    break;
            }
            // Add it to end of list if it doesn't exist
            if (cgroups[n] == NULL) {
                cgroups[n] = & (struct cgroups_control) {
                                .control = CGRP_CPU_CONTROL,
                                .settings = (struct cgroup_setting *[8]) {
                                    &self_to_task,             // must be added to all the new controls added
                                    NULL                       // NULL at the end of the array
                                }
                            };
                // Make end of list NULL
                cgroups[n+1] = NULL;
            }

            // Find settings spot (or empty spot if it's new)
            for (i = 0; i < 5; i++) {
                if (cgroups[n]->settings[i] == NULL)
                    break;
                else if (strcmp(cgroups[n]->settings[i]->name, "cpu.shares") == 0)
                    break;
            }
            // Add a setting spot to end of list if it doesn't exist
            if (cgroups[n]->settings[i] == NULL) {
                cgroups[n]->settings[i] = & (struct cgroup_setting) {
                                                .name = "cpu.shares",
                                                .value = ""         // placeholder, will be replaced
                                            };
                // Make end of list NULL
                cgroups[n]->settings[i+1] = NULL;
            }
            // Set the setting
            stpcpy(cgroups[1]->settings[i]->value, optarg);

            break;
        case 's':
            // Find cgroup control
            for (n = 0; n < 5; n++) {
                if (cgroups[n] == NULL)
                    break;
                else if (strcmp(cgroups[n]->control, CGRP_CPU_SET_CONTROL) == 0)
                    break;
            }
            // Add it to end of list if it doesn't exist
            if (cgroups[n] == NULL) {
                cgroups[n] = & (struct cgroups_control) {
                                .control = CGRP_CPU_SET_CONTROL,
                                .settings = (struct cgroup_setting *[8]) {
                                    // Include cpuset.{cpus,mems} before self_to_task to avoid errors
                                    // Need to have default values before adding process to cgroup
                                    & (struct cgroup_setting) {
                                        .name = "cpuset.cpus",
                                        .value = "0"                        // placeholder, will be replaced
                                    },
                                    & (struct cgroup_setting) {
                                        .name = "cpuset.mems",
                                        .value = "0"                        // need some default value because it isn't an option
                                    },
                                    &self_to_task,             // must be added to all the new controls added
                                    NULL                       // NULL at the end of the array
                                }
                            };
                // Make end of list NULL
                cgroups[n+1] = NULL;
            }

            // Find settings spot (or empty spot if it's new)
            // For cpuset.cpus
            for (i = 0; i < 5; i++) {
                if (cgroups[n]->settings[i] == NULL)
                    break;
                else if (strcmp(cgroups[n]->settings[i]->name, "cpuset.cpus") == 0)
                    break;
            }
            // Add a setting spot to end of list if it doesn't exist
            if (cgroups[n]->settings[i] == NULL) {
                cgroups[n]->settings[i] = & (struct cgroup_setting) {
                                                .name = "cpuset.cpus",
                                                .value = ""         // placeholder, will be replaced
                                            };
                // Make end of list NULL
                cgroups[n]->settings[i+1] = NULL;
            }
            // Set the setting
            stpcpy(cgroups[1]->settings[i]->value, optarg);

            // For cpuset.mems
            for (i = 0; i < 5; i++) {
                if (cgroups[n]->settings[i] == NULL)
                    break;
                else if (strcmp(cgroups[n]->settings[i]->name, "cpuset.mems") == 0)
                    break;
            }
            // Add a setting spot to end of list if it doesn't exist
            if (cgroups[n]->settings[i] == NULL) {
                cgroups[n]->settings[i] = & (struct cgroup_setting) {
                                                .name = "cpuset.mems",
                                                .value = "0"         // need some default value because it isn't an option
                                            };
                // Make end of list NULL
                cgroups[n]->settings[i+1] = NULL;
            }

            break;
        case 'M':
            // Find cgroup control
            for (n = 0; n < 5; n++) {
                if (cgroups[n] == NULL)
                    break;
                else if (strcmp(cgroups[n]->control, CGRP_MEMORY_CONTROL) == 0)
                    break;
            }
            // Add it to end of list if it doesn't exist
            if (cgroups[n] == NULL) {
                cgroups[n] = & (struct cgroups_control) {
                                .control = CGRP_MEMORY_CONTROL,
                                .settings = (struct cgroup_setting *[8]) {
                                    &self_to_task,             // must be added to all the new controls added
                                    NULL                       // NULL at the end of the array
                                }
                            };
                // Make end of list NULL
                cgroups[n+1] = NULL;
            }

            // Find settings spot (or empty spot if it's new)
            // For memory.limit_in_bytes
            for (i = 0; i < 5; i++) {
                if (cgroups[n]->settings[i] == NULL)
                    break;
                else if (strcmp(cgroups[n]->settings[i]->name, "memory.limit_in_bytes") == 0)
                    break;
            }
            // Add a setting spot to end of list if it doesn't exist
            if (cgroups[n]->settings[i] == NULL) {
                cgroups[n]->settings[i] = & (struct cgroup_setting) {
                                                .name = "memory.limit_in_bytes",
                                                .value = ""         // placeholder, will be replaced
                                            };
                // Make end of list NULL
                cgroups[n]->settings[i+1] = NULL;
            }
            // Set the setting
            stpcpy(cgroups[1]->settings[i]->value, optarg);

            // For memory.kmem.limit_in_bytes
            for (i = 0; i < 5; i++) {
                if (cgroups[n]->settings[i] == NULL)
                    break;
                else if (strcmp(cgroups[n]->settings[i]->name, "memory.kmem.limit_in_bytes") == 0)
                    break;
            }
            // Add a setting spot to end of list if it doesn't exist
            if (cgroups[n]->settings[i] == NULL) {
                cgroups[n]->settings[i] = & (struct cgroup_setting) {
                                                .name = "memory.kmem.limit_in_bytes",
                                                .value = ""         // placeholder, will be replaced
                                            };
                // Make end of list NULL
                cgroups[n]->settings[i+1] = NULL;
            }
            // Set the setting
            stpcpy(cgroups[1]->settings[i]->value, optarg);

            break;
        case 'p':
            // Find cgroup control
            for (n = 0; n < 5; n++) {
                if (cgroups[n] == NULL)
                    break;
                else if (strcmp(cgroups[n]->control, CGRP_PIDS_CONTROL) == 0)
                    break;
            }
            // Add it to end of list if it doesn't exist
            if (cgroups[n] == NULL) {
                cgroups[n] = & (struct cgroups_control) {
                                .control = CGRP_PIDS_CONTROL,
                                .settings = (struct cgroup_setting *[8]) {
                                    &self_to_task,             // must be added to all the new controls added
                                    NULL                       // NULL at the end of the array
                                }
                            };
                // Make end of list NULL
                cgroups[n+1] = NULL;
            }

            // Find settings spot (or empty spot if it's new)
            for (i = 0; i < 5; i++) {
                if (cgroups[n]->settings[i] == NULL)
                    break;
                else if (strcmp(cgroups[n]->settings[i]->name, "pids.max") == 0)
                    break;
            }
            // Add a setting spot to end of list if it doesn't exist
            if (cgroups[n]->settings[i] == NULL) {
                cgroups[n]->settings[i] = & (struct cgroup_setting) {
                                                .name = "pids.max",
                                                .value = ""         // placeholder, will be replaced
                                            };
                // Make end of list NULL
                cgroups[n]->settings[i+1] = NULL;
            }
            // Set the setting
            stpcpy(cgroups[1]->settings[i]->value, optarg);

            break;
        case 'r':
            // Find cgroup control
            // It is hardcoded to be the first
            n = 0;

            // Find settings spot (or empty spot if it's new)
            for (i = 0; i < 5; i++) {
                if (cgroups[n]->settings[i] == NULL)
                    break;
                else if (strcmp(cgroups[n]->settings[i]->name, "blkio.throttle.read_bps_device") == 0)
                    break;
            }
            // Add a setting spot to end of list if it doesn't exist
            if (cgroups[n]->settings[i] == NULL) {
                cgroups[n]->settings[i] = & (struct cgroup_setting) {
                                                .name = "blkio.throttle.read_bps_device",
                                                .value = ""         // placeholder, will be replaced
                                            };
                // Make end of list NULL
                cgroups[n]->settings[i+1] = NULL;
            }
            // Set the setting
            stpcpy(cgroups[1]->settings[i]->value, optarg);

            break;
        case 'w':
            // Find cgroup control
            // It is hardcoded to be the first
            n = 0;

            // Find settings spot (or empty spot if it's new)
            for (i = 0; i < 5; i++) {
                if (cgroups[n]->settings[i] == NULL)
                    break;
                else if (strcmp(cgroups[n]->settings[i]->name, "blkio.throttle.write_bps_device") == 0)
                    break;
            }
            // Add a setting spot to end of list if it doesn't exist
            if (cgroups[n]->settings[i] == NULL) {
                cgroups[n]->settings[i] = & (struct cgroup_setting) {
                                                .name = "blkio.throttle.write_bps_device",
                                                .value = ""         // placeholder, will be replaced
                                            };
                // Make end of list NULL
                cgroups[n]->settings[i+1] = NULL;
            }
            // Set the setting
            stpcpy(cgroups[1]->settings[i]->value, optarg);

            break;
        case 'H':
            config.hostname = optarg;
            break;
        default:
            cleanup_stuff(argv, sockets);
            return EXIT_FAILURE;
        }
        last_optind = optind;
    }

    if (!config.argc || !config.mount_dir){
        cleanup_stuff(argv, sockets);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "####### > Checking if the host Linux version is compatible...");
    struct utsname host = {0};
    if (uname(&host))
    {
        fprintf(stderr, "invocation to uname() failed: %m\n");
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    int major = -1;
    int minor = -1;
    if (sscanf(host.release, "%u.%u.", &major, &minor) != 2)
    {
        fprintf(stderr, "major minor version is unknown: %s\n", host.release);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    if (major != 4 || (minor < 7))
    {
        fprintf(stderr, "Linux version must be 4.7.x or minor version less than 7: %s\n", host.release);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    if (strcmp(ARCH_TYPE, host.machine))
    {
        fprintf(stderr, "architecture must be x86_64: %s\n", host.machine);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    fprintf(stderr, "%s on %s.\n", host.release, host.machine);

    if (socketpair(AF_LOCAL, SOCK_SEQPACKET, 0, sockets))
    {
        fprintf(stderr, "invocation to socketpair() failed: %m\n");
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    if (fcntl(sockets[0], F_SETFD, FD_CLOEXEC))
    {
        fprintf(stderr, "invocation to fcntl() failed: %m\n");
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    config.fd = sockets[1];

    /**
     * ------------------------ TODO ------------------------
     * This method here is creating the control groups using the 'cgroups' array
     * Make sure you have filled in this array with the correct values from the command line flags
     * Nothing to write here, just caution to ensure the array is filled
     * ------------------------------------------------------
     **/
    if (setup_cgroup_controls(&config, cgroups))
    {
        clean_child_structures(&config, cgroups, NULL);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }

    /**
     * ------------------------ TODO ------------------------
     * Setup a stack and create a new child process using the clone() system call
     * Ensure you have correct flags for the following namespaces:
     *      Network, Cgroup, PID, IPC, Mount, UTS (You don't need to add user namespace)
     * Set the return value of clone to 'child_pid'
     * Ensure to add 'SIGCHLD' flag to the clone() call
     * You can use the 'child_function' given below as the function to run in the cloned process
     * HINT: Note that the 'child_function' expects struct of type child_config.
     * ------------------------------------------------------
     **/

    // Make child stack
    stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        fprintf(stderr, "malloc failed: %m\n");
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }

    // Clone
    child_pid = clone(
                        child_function,
                        stack + STACK_SIZE,
                        SIGCHLD | CLONE_NEWCGROUP | CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNET,
                        &config
                    );

    /**
     *  ------------------------------------------------------
     **/
    if (child_pid == -1)
    {
        fprintf(stderr, "####### > child creation failed! %m\n");
        clean_child_structures(&config, cgroups, stack);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    close(sockets[1]);
    sockets[1] = 0;

    if (setup_child_uid_map(child_pid, sockets[0]))
    {
        if (child_pid)
            kill(child_pid, SIGKILL);
    }

    int child_status = 0;
    waitpid(child_pid, &child_status, 0);
    int exit_status = WEXITSTATUS(child_status);

    clean_child_structures(&config, cgroups, stack);
    cleanup_sockets(sockets);
    return exit_status;
}


int child_function(void *arg)
{
    struct child_config *config = arg;
    if (sethostname(config->hostname, strlen(config->hostname)) || \
                setup_child_mounts(config) || \
                setup_child_userns(config) || \
                setup_child_capabilities() || \
                setup_syscall_filters()
        )
    {
        close(config->fd);
        return -1;
    }
    if (close(config->fd))
    {
        fprintf(stderr, "invocation to close() failed: %m\n");
        return -1;
    }
    if (execve(config->argv[0], config->argv, NULL))
    {
        fprintf(stderr, "invocation to execve() failed! %m.\n");
        return -1;
    }
    return 0;
}
