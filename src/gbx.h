#ifndef __GBX_H_INCLUDED
#define __GBX_H_INCLUDED 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include "minilzo/minilzo.h"
#include "globals.h"

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

struct gbx_file {
    char* headdata;
    char* reftabdata;
    char* bodydata;
    int32_t gbxclass;
    uint32_t headsize;
    uint32_t reftabsize;
    uint32_t bodysize;
    uint32_t numnodes;
    uint32_t numexnodes;
    uint16_t version;
    char type;
    char headcompr;
    char bodycompr;
    char unknown;
};

typedef struct gbx_file gbx_file;

gbx_file* read_gbx_file(char* path);
void write_gbx_file(char* path, gbx_file* gbx, bool compress);
void free_gbx_file(gbx_file* gbx);
int read_reftab_dir(FILE* ingbxfile, int count);
int read_ref_files(FILE* ingbxfile, int count, uint16_t gbxver);

#endif /* already included */
