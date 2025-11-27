#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#define MEM_SIZE 15

int main(int argc, char *argv[])
{
	char *addr;
	int fd;

	if (argc < 2) {
        fprintf(stderr, "Usage error: %s file [new-value]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDWR);

	if (fd == -1) {
		fprintf(stderr, "open error");
		exit(EXIT_FAILURE);
	}

	addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (addr == MAP_FAILED) {
		fprintf(stderr, "mmap error");
		exit(EXIT_FAILURE);
	}

	if (close(fd) == -1) { /* No longer need 'fd' */
		fprintf(stderr, "close error");
		exit(EXIT_FAILURE);
	}

	/* Secure practice: output at most MEM_SIZE bytes */
    printf("Current string=%.*s\n", MEM_SIZE, addr);

	if (argc > 2) { /* Update contents of region */
		if (strlen(argv[2]) >= MEM_SIZE) {
			fprintf(stderr, "'new-value' too large\n");
			exit(EXIT_FAILURE);
		}

		memset(addr, 0, MEM_SIZE); /* Zero out region */
		strncpy(addr, argv[2], MEM_SIZE - 1);

		if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
			fprintf(stderr, "msync");
			exit(EXIT_FAILURE);
		}

		printf("Copied \"%s\" to shared memory\n", argv[2]);
	}

	exit(EXIT_SUCCESS);
}
