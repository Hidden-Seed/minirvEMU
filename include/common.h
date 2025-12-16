#ifndef __COMMON_H__
#define __COMMON_H__

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t addr_t;
typedef addr_t word_t;
#define FMT_ADDR "0x%08" PRIx32
#define FMT_WORD FMT_ADDR

#include <macro.h>
#include <utils.h>

#endif