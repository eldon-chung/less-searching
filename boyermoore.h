#pragma once

#include <stdlib.h>
#include <string.h>

#define MAX_BM_SHIFT(a, b) ((a < b) ? b : a)


struct boyermoore_tables {
  int *bc_table;
  int *gs_table;
};

void compile_bm_pattern(struct boyermoore_tables *pcompiled, char *pattern,
                        int pattern_len);
void alloc_boyermoore_tables(struct boyermoore_tables *pcompiled,
                             int pattern_len);
void free_boyermoore_tables(struct boyermoore_tables *pcompiled);