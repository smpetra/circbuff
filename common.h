// common.h    common utilities
// Created:    11/09/2017
// Author:     Petra <germfreeadolescents@gmail.com>
#pragma once

#include <stdio.h>
#include <assert.h>

typedef unsigned char BOOL;

#define MIN(x, y) ({ typeof (x) _x = (x); typeof (y) _y = (y); _x < _y ? _x : _y; })

#ifdef DEBUG
#define debuglog(fmt, ...) fprintf(stderr, __DATE__ ":" __TIME__ "  " fmt, ##__VA_ARGS__)
#else
#define debuglog(fmt, ...)
#endif
#define ASSERT(x) assert(x)
