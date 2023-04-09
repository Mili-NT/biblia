/*
kjv: Read the Word of God from your terminal

License: Public domain
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/ioctl.h>

#include "biblia_config.h"
#include "biblia_data.h"
#include "biblia_ref.h"
#include "biblia_render.h"
#include "strutil.h"

const char* usage = "usage: biblia [flags] [reference...]\n"
    "\n"
    "Flags:\n"
    "  -A num  show num verses of context after matching verses\n"
    "  -B num  show num verses of context before matching verses\n"
    "  -C      show matching verses in context of the chapter\n"
    "  -e      highlighting of chapters and verse numbers\n"
    "          (default when output is a TTY)\n"
    "  -p      output_text to less with chapter grouping, spacing, indentation,\n"
    "          and line wrapping\n"
    "          (default when output is a TTY)\n"
    "  -P      Enables plaintext output.\n"
    "  -l      list books\n"
    "  -h      show help\n"
    "\n"
    "Reference:\n"
    "    <Book>\n"
    "        Individual book\n"
    "    <Book>:<Chapter>\n"
    "        Individual chapter of a book\n"
    "    <Book>:<Chapter>:<Verse>[,<Verse>]...\n"
    "        Individual verse(s) of a specific chapter of a book\n"
    "    <Book>:<Chapter>-<Chapter>\n"
    "        Range of chapters in a book\n"
    "    <Book>:<Chapter>:<Verse>-<Verse>\n"
    "        Range of verses in a book chapter\n"
    "    <Book>:<Chapter>:<Verse>-<Chapter>:<Verse>\n"
    "        Range of chapters and verses in a book\n"
    "\n"
    "    /<Search>\n"
    "        All verses that match a pattern\n"
    "    <Book>/<Search>\n"
    "        All verses in a book that match a pattern\n"
    "    <Book>:<Chapter>/<Search>\n"
    "        All verses in a chapter of a book that match a pattern\n";

int main(int argc, char *argv[])
{
    bool is_atty = isatty(STDOUT_FILENO) == 1;
    biblia_config config = {
        .highlighting = is_atty,
        .pretty = is_atty,
        .plaintext = false,
        .maximum_line_length = 80,

        .context_before = 0,
        .context_after = 0,
        .context_chapter = false,
    };

    bool list_books = false;

    opterr = 0;
    for (int opt; (opt = getopt(argc, argv, "A:B:CeplWhP")) != -1; ) {
        char *endptr;
        switch (opt) {
        case 'A':
            config.context_after = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "Biblia: invalid flag value for -A\n\n%s", usage);
                return 1;
            }
            break;
        case 'B':
            config.context_before = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "Biblia: invalid flag value for -B\n\n%s", usage);
                return 1;
            }
            break;
        case 'C':
            config.context_chapter = true;
            break;
        case 'e':
            config.highlighting = true;
            break;
        case 'p':
            config.pretty = true;
            break;
        case 'P':
            config.plaintext = true;
            break;
        case 'l':
            list_books = true;
            break;
        case 'h':
            printf("%s", usage);
            return 0;
        case '?':
            fprintf(stderr, "Biblia: invalid flag -%c\n\n%s", optopt, usage);
            return 1;
        default:
            /* TODO: Change maybe */
            fprintf(stderr, "nargs");
        }
    }

    if (config.plaintext) {
        config.pretty = false;
    }

    if (list_books) {
        for (int i = 0; i < books_length; i++) {
            book *book = &books[i];
            printf("%s (%s)\n", book->name, book->abbr);
        }
        return 0;
    }
    /* Gets the size of the terminal window
     * if the size of the window is greater than 0, it sets config.maximum_line_length to the size. */
    struct winsize ttysize;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ttysize) == 0 && ttysize.ws_col > 0) {
        config.maximum_line_length = ttysize.ws_col;
    }

    signal(SIGPIPE, SIG_IGN);
    /* If no args are passed, enter interactive mode */
    if (argc == optind) {
        using_history();
        while (true) {
            char *input = readline("Biblia> ");
            if (input == NULL) {
                break;
            }
            add_history(input);
            biblia_ref *ref = newref();
            int success = parseref(ref, input);
            free(input);
            if (success == 0) {
                render(ref, &config);
            }
            freeref(ref);
        }
    } else {
        char *ref_str = str_join(argc-optind, &argv[optind]);
        biblia_ref *ref = newref();
        int success = parseref(ref, ref_str);
        free(ref_str);
        if (success == 0) {
            render(ref, &config);
        }
        freeref(ref);
    }
    return 0;
}
