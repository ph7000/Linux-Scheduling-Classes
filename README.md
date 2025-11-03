Note: For simplicity, I have included only three kernel files (one unexisting file: oven.c, and two modified files: sched.h, and core.c, which are key scheduling system design orchestrators for Linux). 

## Kernel scheduling development

Through orchestrating computation and I/O, balancing interactive and CPU-heavy tasks, and managing preemption and interrupts, _scheduling_ provides a crucial role in any operating system. Here I have implemented a simple scheduling class, called oven, which schedules tasks in a shortest job first (SJF) and round-robin (RR) combined policy!

This represents a scheduling regime, which, although rigorous, is optimized for a simple set of tasks (here, the exponential-time algorithm: calculating the fibonacci sequence).

Once the kernel is built and compiled with the oven class, we can simply call `make` and then `./fibonacci n` where n is a positive integer, to use the oven class's policy.


Please note that kernel development is best done with VMWare or an equivalent tool; also necessary is setting up the Linux kernel on the VM -- a great guide is [here](https://w4118.github.io/guides/kernel-compilation.html)!
