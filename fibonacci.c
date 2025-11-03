
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SCHED_OVEN 8
#define SET_SCHED_POLICY SCHED_OTHER

unsigned long long fib(unsigned long long n)
{
	if (n <= 1)
		return n;
	else
		return fib(n - 1) + fib(n - 2);
}

int main(int argc, char **argv)
{
	struct sched_param params = {0};
	int ret;
	unsigned long long n;
	pid_t pid = getpid();
	ret = sched_setscheduler(pid, 8, &params);
	if (ret) {
		fprintf(stderr, "OVEN scheduling policy does not exist\n");
		exit(1);
	}

	if (argc != 2) {
		fprintf(stderr, "Usage: ./fibonacci [number]\n");
		exit(1);
	}

	n = atoi(argv[1]);
	sleep(1);
	unsigned long long result = fib(n);
	printf("Result: fib(%llu) = %llu\n", n, result);
	printf("Exit code (modulo 256): %d\n", (int)(result % 256));
	while(1);
	return fib(n);
}
