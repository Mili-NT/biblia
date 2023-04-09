#include <ctype.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "biblia_data.h"
#include "biblia_ref.h"



biblia_ref* newref()
{
    return calloc(1, sizeof(biblia_ref));
}

void freeref(biblia_ref *ref)
{
    if (ref) {
        free(ref->search_str);
        regfree(&ref->search);
        free(ref);
    }
}


static bool bookequal(const char *a, const char *b, bool short_match)
{
    for (size_t i = 0, j = 0; ; ) {
        if ((!a[i] && !b[j]) || (short_match && !b[j])) {
            return true;
        } else if (a[i] == ' ') {
            i++;
        } else if (b[j] == ' ') {
            j++;
        } else if (tolower(a[i]) != tolower(b[j])) {
            return false;
        } else {
            i++;
            j++;
        }
    }
}

static bool book_matches(const book *book, const char *s)
{
    return bookequal(book->name, s, false) ||
        bookequal(book->abbr, s, false) ||
        bookequal(book->name, s, true);
}

static int book_fromname(const char *s)
{
    for (int i = 0; i < books_length; i++) {
        const book *book = &books[i];
        if (book_matches(book, s)) {
            return book->number;
        }
    }
    return 0;
}

static int scanbook(const char *s, int *n)
{
    int i;
    int mode = 0;
    for (i = 0; s[i]; i++) {
        if (s[i] == ' ') {
            continue;
        } else if (('a' <= s[i] && s[i] <= 'z') || ('A' <= s[i] && s[i] <= 'Z')) {
            mode = 2;
        } else if ('0' <= s[i] && s[i] <= '9' && 0 <= mode && mode <= 1) {
            mode = 1;
        } else {
            break;
        }
    }
    *n = i;
    return mode >= 1;
}

int parseref(biblia_ref *ref, const char *ref_str)
{
    // 1. <book>
    // 2. <book>:?<chapter>
    // 3. <book>:?<chapter>:<verse>
    // 3a. <book>:?<chapter>:<verse>[,<verse>]...
    // 4. <book>:?<chapter>-<chapter>
    // 5. <book>:?<chapter>:<verse>-<verse>
    // 6. <book>:?<chapter>:<verse>-<chapter>:<verse>
    // 7. /<search>
    // 8. <book>/search
    // 9. <book>:?<chapter>/search

    ref->type = 0;
    ref->book = 0;
    ref->chapter = 0;
    ref->chapter_end = 0;
    ref->verse = 0;
    ref->verse_end = 0;
    for (int i = 0; i < MAX_TRANSLATIONS; i++) {
        strcpy(ref->translations[i], "");
    }
    ref->translation_count = 0;
    intset_free(ref->verse_set);
    ref->verse_set = NULL;
    free(ref->search_str);
    ref->search_str = NULL;
    regfree(&ref->search);

    int n = 0;
    // ref_str = 1cor 7:3 kjv,vul,grb
    /* Book name parsing */
    if (scanbook(ref_str, &n) == 1) {
        // 1, 2, 3, 3a, 4, 5, 6, 8, 9
        char *bookname = strndup(ref_str, n);
        ref->book = book_fromname(bookname);
        free(bookname);
        /* Removes the found chapter from ref_str */
        ref_str = &ref_str[n];
        // ref_str = 7:3 kjv,vul,grb
    /* If the first character is the search operator, initiate search */
    } else if (ref_str[0] == '/') {
        // 7
        goto search;
    } else {
        return 1;
    }
    /* This performs the same parsing but on the chapter number */
    if (sscanf(ref_str, ": %u %n", &ref->chapter, &n) == 1 || sscanf(ref_str, "%u %n", &ref->chapter, &n) == 1) {
        // 2, 3, 3a, 4, 5, 6, 9
        /* Removed from ref_str again */
        ref_str = &ref_str[n];
        // ref_str = :3 kjv,vul,grb
    } else if (ref_str[0] == '/') {
        // 8
        goto search;
    } else if (ref_str[0] == '\0') {
        // 1
        ref->type = biblia_ref_EXACT;
        return 0;
    } else {
        return 1;
    }
    /* Finally parses the verse number */
    if (sscanf(ref_str, ": %u %n", &ref->verse, &n) == 1) {
        // 3, 3a, 5, 6
        /* Ref_str reduced again */
        ref_str = &ref_str[n];
        // ref_str = kjv,vul,grb
    } else if (sscanf(ref_str, "- %u %n", &ref->chapter_end, &n) == 1) {
        // 4
        if (ref_str[n] != '\0') {
            return 1;
        }
        ref->type = biblia_ref_RANGE;
        return 0;
    } else if (ref_str[0] == '/') {
        // 9
        goto search;
    } else if (ref_str[0] == '\0') {
        // 2
        ref->type = biblia_ref_EXACT;
        return 0;
    } else {
        return 1;
    }
    /* Check if translations were provided */
    if (strlen(ref_str) != 0) {
        char *token;
        int i = 0;
        char* ref_str_cpy = strdup(ref_str);
        token = strtok(ref_str_cpy, ",");
        /* Parses through the comma separated translation string and stores it in the translations array */
        while (token != NULL && i < MAX_TRANSLATIONS) {
            strcpy(ref->translations[i], token);
            ref->translation_count++;
            i++;
            token = strtok(NULL, ",");
        }
        free(ref_str_cpy);
    }

    unsigned int value;
    int ret = sscanf(ref_str, "- %u %n", &value, &n);
    if (ret == 1 && ref_str[n] == '\0') {
        // 5
        ref->verse_end = value;
        ref->type = biblia_ref_RANGE;
        return 0;
    } else if (ret == 1) {
        // 6
        ref->chapter_end = value;
        ref_str = &ref_str[n];
    } else if (ref_str[0] == '\0') {
        // 3
        ref->type = biblia_ref_EXACT;
        return 0;
    } else if (sscanf(ref_str, ", %u %n", &value, &n) == 1) {
        // 3a
        ref->verse_set = intset_new();
        intset_add(ref->verse_set, ref->verse);
        intset_add(ref->verse_set, value);
        ref_str = &ref_str[n];
        while (true) {
            if (sscanf(ref_str, ", %u %n", &value, &n) != 1) {
                break;
            }
            intset_add(ref->verse_set, value);
            ref_str = &ref_str[n];
        }
        if (ref_str[0] != '\0') {
            return 1;
        }
        ref->type = biblia_ref_EXACT_SET;
        return 0;
    } else {
        return 1;
    }

    if (sscanf(ref_str, ": %u %n", &ref->verse_end, &n) == 1 && ref_str[n] == '\0') {
        // 6
        ref->type = biblia_ref_RANGE_EXT;
        return 0;
    } else {
        return 1;
    }

search:
    ref->type = biblia_ref_SEARCH;
    if (regcomp(&ref->search, &ref_str[1], REG_EXTENDED|REG_ICASE|REG_NOSUB) != 0) {
        return 2;
    }
    ref->search_str = strdup(&ref_str[1]);
    return 0;
}
