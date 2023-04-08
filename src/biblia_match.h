#pragma once

#include "biblia_config.h"
#include "biblia_ref.h"

typedef struct {
    int start;
    int end;
} kjv_range;

typedef struct {
    int current;
    int next_match;
    kjv_range matches[2];
} kjv_next_data;

int
kjv_next_verse(const kjv_ref *ref, const biblia_config *config, kjv_next_data *next);
