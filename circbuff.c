// circbuff.c  circular buffer implementation file
// Created:	   11/09/2017
// Author:     Petra <germfreeadolescents@gmail.com>

#include <stdint.h>
#include <string.h>
#include "common.h"
#include "circbuff.h"

inline void circbuff_clear(struct circbuff *cb)
{
	cb->empty = 1;
	cb->nextread = cb->nextwrite = 0;
}

inline BOOL circbuff_empty(const struct circbuff *cb)
{
	return cb->empty;
}

inline BOOL circbuff_full(const struct circbuff *cb)
{
	return cb->nextread == cb->nextwrite && !cb->empty;
}

inline uint32_t circbuff_curr_size(const struct circbuff *cb)
{
	if (cb->empty) return 0;
	return cb->nextwrite > cb->nextread ? (cb->nextwrite - cb->nextread) : (MAX_CIRCBUFF_SIZE - cb->nextread + cb->nextwrite);
}

inline uint32_t circbuff_max_size()
{
	return MAX_CIRCBUFF_SIZE;
}

inline uint32_t circbuff_free(const struct circbuff *cb)
{
	return MAX_CIRCBUFF_SIZE - circbuff_curr_size(cb);
}

BOOL circbuff_push(struct circbuff *cb, uint32_t el)
{
	if (circbuff_full(cb)) {
		debuglog("circbuffer.push(): buffer is full.\n");
		return 0;
	}

	cb->buffer[cb->nextwrite] = el;

	// update write index, wrap around if needed
	cb->nextwrite++;
	cb->nextwrite &= MAX_CIRCBUFF_SIZE - 1;

	// the buffer cannot be empty now, since we just wrote to it
	cb->empty = 0;

	return 1;
}

BOOL circbuff_pop(struct circbuff *cb, uint32_t *el) {
	ASSERT(el);
	if (cb->empty) {
		debuglog("circbuffer.pop(): buffer is empty.\n");
		return 0;
	}

	*el = cb->buffer[cb->nextread];

	// update read index, wrap around if needed
	cb->nextread++;
	cb->nextread &= MAX_CIRCBUFF_SIZE - 1;

	if (cb->nextread == cb->nextwrite)
		cb->empty = 1;

	return 1;
}

BOOL circbuff_peek(const struct circbuff *cb, uint32_t *el)
{
	ASSERT(el);
	if (cb->empty) {
		debuglog("circbuffer.peek(): buffer is empty.\n");
		return 0;
	}

	*el = cb->buffer[cb->nextread];

	return 1;
}

uint32_t circbuff_write(struct circbuff *cb, const uint32_t *src, uint32_t n)
{
	ASSERT(src);
	if (!n) return 0;

	n = MIN(n, circbuff_free(cb));

	if (cb->nextread > cb->nextwrite || n <= MAX_CIRCBUFF_SIZE - cb->nextwrite) {
		memcpy(cb->buffer + cb->nextwrite, src, n * sizeof (uint32_t));
	} else {	
		uint32_t n1 = MAX_CIRCBUFF_SIZE - cb->nextwrite;
		uint32_t n2 = n - n1;
		memcpy(cb->buffer + cb->nextwrite, src, n1 * sizeof (uint32_t));
		memcpy(cb->buffer, src + n1, n2 * sizeof (uint32_t));
	}

	// the buffer cannot be empty now, since we just wrote to it
	cb->empty = 0;

	// update write index, wrap around if needed
	cb->nextwrite += n;
	cb->nextwrite &= MAX_CIRCBUFF_SIZE - 1;

	return n;
}

uint32_t circbuff_read(struct circbuff *cb, uint32_t *dst, uint32_t n)
{
	ASSERT(dst);
	if (!n) return 0;

	n = MIN(n, circbuff_curr_size(cb));

	if (cb->nextread < cb->nextwrite || n <= MAX_CIRCBUFF_SIZE - cb->nextread) {
		memcpy(dst, cb->buffer + cb->nextread, n * sizeof (uint32_t));
	} else {
		uint32_t n1 = MAX_CIRCBUFF_SIZE - cb->nextread;
		uint32_t n2 = n - n1;
		memcpy(dst, cb->buffer + cb->nextread, n1 * sizeof (uint32_t));
		memcpy(dst + n1, cb->buffer, n2 * sizeof (uint32_t));
	}

	// update read index, wrap around if needed
	cb->nextread += n;
	cb->nextread &= MAX_CIRCBUFF_SIZE - 1;

	if (cb->nextread == cb->nextwrite)
		cb->empty = 1;

	return n;
}
