#pragma once

#include <regex.h>

#include "intset.h"

#define biblia_ref_SEARCH 1
#define biblia_ref_EXACT 2
#define biblia_ref_EXACT_SET 3
#define biblia_ref_RANGE 4
#define biblia_ref_RANGE_EXT 5
#define MAX_TRANSLATIONS 4

typedef struct biblia_ref {
    int type;
    unsigned int book;
    unsigned int chapter;
    unsigned int chapter_end;
    unsigned int verse;
    unsigned int verse_end;
    char translations[MAX_TRANSLATIONS][6];
    unsigned int translation_count;
    intset *verse_set;
    char *search_str;
    regex_t search;
} biblia_ref;

biblia_ref *newref();

void freeref(biblia_ref *ref);

int parseref(biblia_ref *ref, const char *ref_str);
