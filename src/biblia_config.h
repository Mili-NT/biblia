#pragma once

#include <stdbool.h>

typedef struct {
    bool highlighting;
    bool pretty;
    bool plaintext;

    int maximum_line_length;

    int context_before;
    int context_after;
    bool context_chapter;
} biblia_config;
