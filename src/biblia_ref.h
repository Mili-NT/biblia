#pragma once

#include <regex.h>

#include "intset.h"

#define biblica_ref_SEARCH 1
#define biblica_ref_EXACT 2
#define biblica_ref_EXACT_SET 3
#define biblica_ref_RANGE 4
#define biblica_ref_RANGE_EXT 5

typedef struct biblica_ref {
    int type;
    unsigned int book;
    unsigned int chapter;
    unsigned int chapter_end;
    unsigned int verse;
    unsigned int verse_end;
    intset *verse_set;
    char *search_str;
    regex_t search;
} biblica_ref;

biblica_ref *newref();

void freeref(biblica_ref *ref);

int parseref(biblica_ref *ref, const char *ref_str);
