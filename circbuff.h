// circbuff.h  circular buffer header file
// Created:	   11/09/2017
// Author:     Petra <germfreeadolescents@gmail.com>
#pragma once

#include <stdint.h>
#include "common.h"

// Default max buffer size in element count,
// used when the value hasn't been provided in Makefile.
#ifndef MAX_CIRCBUFF_SIZE
#define MAX_CIRCBUFF_SIZE 128
#endif

struct circbuff {
	uint32_t buffer[MAX_CIRCBUFF_SIZE];
	// Indices of next elements that should be read and written respectively
	uint32_t nextread, nextwrite;
	// Flag that keeps track if buffer is empty
	BOOL empty;
};

// Empties the buffer. Call before any other method.
inline void circbuff_clear(struct circbuff *cb);

// Check whether buffer is empty/full.
inline BOOL circbuff_empty(const struct circbuff *cb);
inline BOOL circbuff_full(const struct circbuff *cb);

// Return current/maximum buffer size in number of elements (not bytes).
inline uint32_t circbuff_curr_size(const struct circbuff *cb);
inline uint32_t circbuff_max_size();

// Returns free space in buffer, in number of elements.
inline uint32_t circbuff_free(const struct circbuff *cb);

// Pushes one element into the buffer.
// Returns 0 if buffer is full, 1 otherwise.
BOOL circbuff_push(struct circbuff *cb, uint32_t el);

// Pops one element from the buffer and stores it into *el.
// Returns 0 if buffer is empty, 1 otherwise.
BOOL circbuff_pop(struct circbuff *cb, uint32_t *el);

// Peeks one element from the buffer (does not remove the element).
// Returns 0 if buffer is empty, 1 otherwise.
BOOL circbuff_peek(const struct circbuff *cb, uint32_t *el);

// Writes n elements (or as much as it can fit) from src into the buffer.
// Returns number of elements actually written.
uint32_t circbuff_write(struct circbuff *cb, const uint32_t *src, uint32_t n);

// Reads n elements (or as much as it is available) from buffer and copies them into dst.
// Returns number of elements actually read.
uint32_t circbuff_read(struct circbuff *cb, uint32_t *dst, uint32_t n);
