#pragma once

typedef struct {
    int number;
    char *name;
    char *abbr;
} book;

typedef struct {
    int book;
    int chapter;
    int verse;
    char *text;
} verse;

extern verse verses[];

extern int verses_length;

extern book books[];

extern int books_length;
