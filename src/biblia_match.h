#pragma once

#include "biblia_config.h"
#include "biblia_ref.h"

typedef struct {
    int start;
    int end;
} range;

typedef struct {
    int current;
    int next_match;
    range matches[2];
} next_data;

int next_verse(const biblia_ref *ref, const biblia_config *config, next_data *next);
