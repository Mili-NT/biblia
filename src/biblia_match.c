#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <stdlib.h>

#include "biblia_data.h"
#include "biblia_match.h"
#include "intset.h"

static bool verse_matches(const biblica_ref *ref, const verse *verse)
{
    switch (ref->type) {
        case biblica_ref_SEARCH:
            return (ref->book == 0 || ref->book == verse->book) &&
                (ref->chapter == 0 || verse->chapter == ref->chapter) &&
                regexec(&ref->search, verse->text, 0, NULL, 0) == 0;

        case biblica_ref_EXACT:
            return ref->book == verse->book &&
                (ref->chapter == 0 || ref->chapter == verse->chapter) &&
                (ref->verse == 0 || ref->verse == verse->verse);

        case biblica_ref_EXACT_SET:
            return ref->book == verse->book &&
                (ref->chapter == 0 || verse->chapter == ref->chapter) &&
                intset_contains(ref->verse_set, verse->verse);

        case biblica_ref_RANGE:
            return ref->book == verse->book &&
                ((ref->chapter_end == 0 && ref->chapter == verse->chapter) ||
                    (verse->chapter >= ref->chapter && verse->chapter <= ref->chapter_end)) &&
                (ref->verse == 0 || verse->verse >= ref->verse) &&
                (ref->verse_end == 0 || verse->verse <= ref->verse_end);

        case biblica_ref_RANGE_EXT:
            return ref->book == verse->book &&
                (
                    (verse->chapter == ref->chapter && verse->verse >= ref->verse && ref->chapter != ref->chapter_end) ||
                    (verse->chapter > ref->chapter && verse->chapter < ref->chapter_end) ||
                    (verse->chapter == ref->chapter_end && verse->verse <= ref->verse_end && ref->chapter != ref->chapter_end) ||
                    (ref->chapter == ref->chapter_end && verse->chapter == ref->chapter && verse->verse >= ref->verse && verse->verse <= ref->verse_end)
                );

        default:
            return false;
    }
}

#define DIRECTION_BEFORE (-1)
#define DIRECTION_AFTER 1

static int chapter_bounds(int i, int direction, int maximum_steps)
{
    assert(direction == DIRECTION_BEFORE || direction == DIRECTION_AFTER);

    int steps = 0;
    for ( ; 0 <= i && i < verses_length; i += direction) {
        bool step_limit = (maximum_steps != -1 && steps >= maximum_steps) ||
                          (direction == DIRECTION_BEFORE && i == 0) ||
                          (direction == DIRECTION_AFTER && i + 1 == verses_length);
        if (step_limit) {
            break;
        }

        const verse *current = &verses[i], *next = &verses[i + direction];
        if (current->book != next->book || current->chapter != next->chapter) {
            break;
        }
        steps++;
    }
    return i;
}

static int get_next_match(const biblica_ref *ref, int i)
{
    for ( ; i < verses_length; i++) {
        const verse *verse = &verses[i];
        if (verse_matches(ref, verse)) {
            return i;
        }
    }
    return -1;
}

static void next_addrange(next_data *next, range range) {
    if (next->matches[0].start == -1 && next->matches[0].end == -1) {
        next->matches[0] = range;
    } else if (range.start < next->matches[0].end) {
        next->matches[0] = range;
    } else {
        next->matches[1] = range;
    }
}

int next_verse(const biblica_ref *ref, const biblia_config *config, next_data *next)
{
    if (next->current >= verses_length) {
        return -1;
    }

    if (next->matches[0].start != -1 && next->matches[0].end != -1 && next->current >= next->matches[0].end) {
        next->matches[0] = next->matches[1];
        next->matches[1] = (range){-1, -1};
    }

    if ((next->next_match == -1 || next->next_match < next->current) && next->next_match < verses_length) {
        int next_match = get_next_match(ref, next->current);
        if (next_match >= 0) {
            next->next_match = next_match;
            range bounds = {
                .start = chapter_bounds(next_match, DIRECTION_BEFORE,
                                        config->context_chapter ? -1 : config->context_before),
                .end =
                chapter_bounds(next_match, DIRECTION_AFTER, config->context_chapter ? -1 : config->context_after) + 1,
            };
            next_addrange(next, bounds);
        } else {
            next_match = verses_length;
        }
    }

    if (next->matches[0].start == -1 && next->matches[0].end == -1) {
        return -1;
    }

    if (next->current < next->matches[0].start) {
        next->current = next->matches[0].start;
    }

    return next->current++;
}
