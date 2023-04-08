# Biblica

Biblica is a CLI program written in C for biblical study. Inspired by and forked from
[kjv](https://github.com/layeh/kjv), I wanted to ~~bloat~~ extend the program to include
several translations and a few new features.

Namely:
1. Compare verses between translations (in the same window no less).
The kjv program is extremely well-designed and works with basically any text with verses,
so I wanted to include as many translations as is feasible into one program instead of
making a separate script for each.
2. Add plaintext output for easy piping into other processes or files
3. Potentially allow for historical or theological commentary to be attached to sections/verses.
This is going to a bit tricky due to the relatively unstructured format of commentary. If I could
ever successfully parse the beast that is the Orthodox Study Bible PDF, that will be my first source.
4. A nice TUI if I can design one
5. Other minor features as I develop hyper-specific needs for them.
## Credits:
- Original program: https://github.com/layeh/kjv
- Septuagint & Greek New Testament TSV: https://github.com/LukeSmithxyz/grb
## Usage

    usage: biblica [flags] [reference...]

    Flags:
      -A num  show num verses of context after matching verses
      -B num  show num verses of context before matching verses
      -C      show matching verses in context of the chapter
      -e      highlighting of chapters and verse numbers
              (default when output_text is a TTY)
      -p      output_text to less with chapter grouping, spacing, indentation,
              and line wrapping
              (default when output_text is a TTY)
      -P      Output to plain text
      -l      list books
      -h      show help

    Reference:
        <Book>
            Individual book
        <Book>:<Chapter>
            Individual chapter of a book
        <Book>:<Chapter>:<Verse>[,<Verse>]...
            Individual verse(s) of a specific chapter of a book
        <Book>:<Chapter>-<Chapter>
            Range of chapters in a book
        <Book>:<Chapter>:<Verse>-<Verse>
            Range of verses in a book chapter
        <Book>:<Chapter>:<Verse>-<Chapter>:<Verse>
            Range of chapters and verses in a book

        /<Search>
            All verses that match a pattern
        <Book>/<Search>
            All verses in a book that match a pattern
        <Book>:<Chapter>/<Search>
            All verses in a chapter of a book that match a pattern

## Build

Biblica can be built by cloning the repository and then running make:

    git clone https://github.com/Mili-NT/biblia.git
    cd biblica
    make

## License

Public domain
