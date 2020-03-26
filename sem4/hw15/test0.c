#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>


extern size_t
read_data_and_count(size_t N, int in[N]);

typedef struct {
	int pipe[2];
	pthread_t writer;
	size_t bytes_to_write;
	char symb;
	unsigned seed;
} comm_t;

static unsigned
radint(unsigned max, comm_t *ctx)
{
	double d = rand_r(&ctx->seed);
	d /= RAND_MAX;
	d *= max;
	unsigned uval = d;
	return uval;
}

static void*
writer_thread(void *ptr)
{
	comm_t *ctx = ptr;
	ssize_t cnt;
	char buffer[1];
	memset(buffer, ctx->symb, sizeof(buffer));
	unsigned to_sleep;
	while ((cnt = write(ctx->pipe[1], buffer, sizeof(buffer))) > 0) {
		for (ssize_t i = 0; i < cnt; ++i)
		{
			if (buffer[i] != ctx->symb) {
				abort();
			}
		}
		ctx->bytes_to_write -= cnt;
		if (0 == ctx->pipe[1]) {
			close(ctx->pipe[1]);
			break;
		}
		to_sleep = radint(50, ctx);
		usleep(to_sleep);
	}
}

static void
start_comm(char symb, size_t total_bytes, comm_t *comm) 
{
	socketpair(AF_LOCAL, SOCK_STREAM, 0, comm->pipe);
	comm->bytes_to_write = total_bytes;
	comm->symb = symb;
	comm->seed = time(NULL);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
	pthread_create(&comm->writer, &attr, writer_thread, comm);
	pthread_attr_destroy(&attr);
}

int main(int argc, char *argv[])
{
	size_t N;
	size_t count;
	scanf("%llu", &N);
	scanf("%llu", &count);
	comm_t * comm_threads = calloc(N, sizeof *comm_threads);
	int *in = calloc(N, sizeof *in);
	for (size_t i = 0; i < N; ++i)
	{
		comm_t *comm = &comm_threads[i];
		start_comm('0' + (i % 10), count, comm);
		in[i] = comm->pipe[0];
	}
	size_t total = read_data_and_count(N, in);
	for (size_t i = 0; i < N; ++i)
	{
		comm_t *comm = &comm_threads[i];
		pthread_join(comm->writer, NULL);
	}
	free(in);
	free(comm_threads);
	printf("%llu\n", total);
}