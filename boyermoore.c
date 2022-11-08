#include "boyermoore.h"

// #define max(a, b) ((a < b) ? b : a)

void alloc_boyermoore_tables(pcompiled, pattern_len)
    struct boyermoore_tables* pcompiled;
    int pattern_len;
{
    pcompiled->bc_table = (int *) calloc(256, sizeof(int)); 
    // allocating the added 1 int is not a mistake
    pcompiled->gs_table = (int *) calloc(pattern_len + 1, sizeof(int)); 
}

void free_boyermoore_tables(pcompiled)
    struct boyermoore_tables* pcompiled;
{
    if(pcompiled->bc_table != NULL)
        free(pcompiled->bc_table);
    pcompiled->bc_table = NULL;
    if(pcompiled->gs_table != NULL)
        free(pcompiled->gs_table);
    pcompiled->gs_table = NULL;
}

static void gs_preproc(pattern, pattern_len, skip_table)
char *pattern;
int pattern_len;
int *skip_table;
{
    // require: skip_table is of length pattern_len + 1
    // allocating the additional 1 int for z_table is not a mistake
    int z_table[pattern_len + 1];
    for(int idx = 0; idx <= pattern_len; idx++)
    {
        skip_table[idx] = 0;
        z_table[idx] = 0;
    }

    int left_boundary_idx = pattern_len;
    int suffix_left_boundary_idx = pattern_len + 1;
    z_table[pattern_len] = pattern_len + 1;
    while(left_boundary_idx > 0)
    {
        // in case the current char doesn't extend the previously longest known suffix
        while(suffix_left_boundary_idx <= pattern_len && pattern[left_boundary_idx - 1] != pattern[suffix_left_boundary_idx - 1] ){
            // if we had to skip, this is when a alignment shift needs to happen
            // if the skip table is uninitialised, we set this as the shift amount;
            if(skip_table[suffix_left_boundary_idx] == 0)
            {
                skip_table[suffix_left_boundary_idx] = suffix_left_boundary_idx - left_boundary_idx;
            }

            // then we update our suffix_left_boundary_idx to where we "jump" to;
            suffix_left_boundary_idx = z_table[suffix_left_boundary_idx];
        }

        // now we are guaranteed it can match, so we move back
        suffix_left_boundary_idx--;
        left_boundary_idx--;

        // update our z_table to extend off the longest known suffix now
        z_table[left_boundary_idx] = suffix_left_boundary_idx;
    }
    
    // we need to handle the 2 remaining cases:

    int matching_suffix_idx = z_table[0];
    for(int table_idx = 0; table_idx < pattern_len; table_idx++)
    {
        // if it's initialised, then we leave it be
        if(skip_table[table_idx] != 0)
        {
            continue;
        }

        if(table_idx <= matching_suffix_idx)
        {
            // Case (2)
            // if at the current point, there is a prefix of P that matches us, we look up
            // z_table at our index to point us to the longest suffix 
            skip_table[table_idx] = matching_suffix_idx;
        } else 
        {
            // Case (3) 
            // in any remaining case, we should just shift the pattern all the way
            skip_table[table_idx] = table_idx;
        } 

    }
}

static void bc_preproc(pattern, pattern_len, skip_table)
    char *pattern;
    int pattern_len;
    int *skip_table;
{
    // require: skip_table is of length 256
    // initialise the entire skip_table to -1; this is for the chars that do not exist in pattern
    for(int idx = 0; idx < 256; idx++)
    {
        skip_table[idx] = -1;
    }

    // store the latest occurence of each character;
    for(int idx = 0; idx < pattern_len; idx++)
    {
        skip_table[pattern[idx]] = idx;
    }
}

void compile_bm_pattern(pcompiled, pattern, pattern_len)
    struct boyermoore_tables *pcompiled;
    char *pattern;
    int pattern_len;
{
    bc_preproc(pattern, pattern_len, pcompiled->bc_table);
    gs_preproc(pattern, pattern_len, pcompiled->gs_table);
}

// int boyermoore_match(bm_compiled, pattern, pattern_len, buf, buf_len, pfound, pend)
//     struct boyermoore_tables* bm_compiled;
//     char* pattern;
//     int pattern_len;
//     char* buf;
//     int buf_len;
//     char **pfound;
//     char **pend;
// {
//     char *buf_end = buf + buf_len;
//     char *pattern_end = pattern + pattern_len;

//     // we start matching from the end of the pattern
//     char *lp = buf + pattern_len - 1;
//     char *pp = pattern + pattern_len - 1;

//     for( ; buf + pattern_len <= buf_end ; )
//     {
//         // get the chars to match, converting for case insensitivity if needed
//         char cp = *pp;
//         char cl = *lp;

//         if (caseless == OPT_ONPLUS && ASCII_IS_UPPER(cp))
//             cp = ASCII_TO_LOWER(cp);

//         if(cp == cl)
//         {
//             pp--;
//             lp--;

//             if(pp <= pattern - 1)
//             {
//                 if (pfound != NULL)
// 				    *pfound = buf;
// 			    if (pend != NULL)
// 				    *pend = buf + pattern_len;
// 			    return (1);
//             }
//         } else 
//         {
//             // shift our alignment forward based on both rules
//             int pattern_idx = pp - pattern;

//             // get max of both shifts
//             int shift = max(pattern_idx - bm_compiled->bc_table[cl], bm_compiled->gs_table[pattern_idx + 1]);
            
//             // I don't think you need this.
//             shift = max(shift, 1);

//             // shift the alignment by that amount
//             buf += shift;

//             // reset pp and lp
//             pp = pattern + pattern_len - 1;
//             lp = buf + pattern_len - 1;
//         }
//     }
//     return (0);
// }