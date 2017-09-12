// main.c      code demonstrating circular buffer funcionality
// Created:	   11/09/2017
// Author:     Petra <germfreeadolescents@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "circbuff.h"

#define TEST_FAILED 2 // exit status for test failure

static BOOL verbose = 0;

// Not a real compare, we want to ignore last few bytes,
// because we didn't read them in the first place.
static BOOL _compare_files(FILE *input, FILE *output)
{
	rewind(input);
	rewind(output);

	uint32_t buf1[256], buf2[256];
	size_t ret1, ret2;
	do {
		ret1 = fread(buf1, 4, 256, input);
		ret2 = fread(buf2, 4, 256, output);
		if (ret1 != ret2 || memcmp(buf1, buf2, 4 * ret1)) return 0;
	} while (ret1 > 0);

	return 1;
}

// Reads random number of dwords from input file, and writes read data into the circular buffer.
// Write is performed either as a sequence of pushes or a single write operation.
// Reads random number of dwords from circular buffer and writes it into the output file.
// Read is performed either as a sequence of pops or a single read operation.
// Repeats those operations until there are no more dwords in the input file.
// Compares input and output files.
int main(int argc, char **argv)
{
	struct circbuff cb;
	FILE *input, *output;
	uint32_t buf[MAX_CIRCBUFF_SIZE];

	if (argc < 2) {
		fprintf(stderr, "Error, too few arguments\nUsage: %s <test_file> [-v]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (argc > 2 && !strcmp(argv[2], "-v"))
		verbose = 1;

	circbuff_clear(&cb);
	if (!circbuff_empty(&cb)) {
		printf("Test failed, buffer is not empty after clear\n");
		exit(TEST_FAILED);
	}

	if (!(input = fopen(argv[1], "r"))) {
		fprintf(stderr, "Error, cannot open %s for reading: %s (%d)\n", argv[1], strerror(errno), errno);
		exit(EXIT_FAILURE);
	}

	if (!(output = tmpfile())) {
		fprintf(stderr, "Error, cannot create tmp file: %s (%d)\n", strerror(errno), errno);
		exit(EXIT_FAILURE);
	}

	BOOL done = 0;

	// ignore few end bytes if the size of input file is not divisible by 4
	do {
		// choose how many elements to push/write: random[1, MAX_CIRCBUFF_SIZE], or as much as it can fit in the buffer.
		unsigned write_count = MIN((unsigned)rand() % MAX_CIRCBUFF_SIZE + 1, circbuff_free(&cb));

		// read elements from the input file
		write_count = fread(buf, 4, write_count, input);
		if (write_count < 1)
			done = 1;

		// chose if we are going to push one by one element or write all at once
		if (rand() & 1) {
			// use push
			if (verbose) printf("push %u\n", write_count);

			unsigned i;
			for (i = 0; i < write_count; i++) {
				if (!circbuff_push(&cb, buf[i])) {
					printf("Test failed, buffer should not be full\n");
					exit(TEST_FAILED);
				}
			}
		} else {
			// use write
			if (verbose) printf("write %u\n", write_count);

			if (circbuff_write(&cb, buf, write_count) != write_count) {
				printf("Test failed, data should fit the buffer\n");
				exit(TEST_FAILED);
			}
		}

		// choose how many elements to pop/read: random[1, MAX_CIRCBUFF_SIZE], or as much as it is available.
		// If we are done reading the input file, we want to read them all.
		unsigned read_count = done ? MAX_CIRCBUFF_SIZE : (unsigned)rand() % MAX_CIRCBUFF_SIZE + 1;
		read_count = MIN(read_count, circbuff_curr_size(&cb));

		// chose if we are going to pop elements one by one or read them all at once
		if (rand() & 1) {
			// use pop
			if (verbose) printf("pop %u\n", read_count);

			unsigned i;
			for (i = 0; i < read_count; i++) {
				if (!circbuff_pop(&cb, buf + i)) {
					printf("Test failed, buffer should not be empty\n");
					exit(TEST_FAILED);
				}
			}
		} else {
			// use read
			if (verbose) printf("read %u\n", read_count);
			unsigned ret;
			if ((ret = circbuff_read(&cb, buf, read_count)) != read_count) {
				printf("Test failed, there should be enough data in the buffer: read %d\n", ret);
				exit(TEST_FAILED);
			}
		}

		// write to the output file
		if (fwrite(buf, 4, read_count, output) != read_count) {
			fprintf(stderr, "Error, cannot write to output file: %s (%d)\n", strerror(errno), errno);
			exit(EXIT_FAILURE);
		}
	} while (!done);

	if (!circbuff_empty(&cb)) {
		printf("Test failed, buffer should be empty\n");
		exit(TEST_FAILED);
	}

	if (!_compare_files(input, output)) {
		printf("Test failed, data mismatch\n");
		exit(TEST_FAILED);
	}

	if (verbose) printf("---------------\n");
	printf("Test passed!\n");

	fclose(output);
	fclose(input);

	exit(EXIT_SUCCESS);
}
