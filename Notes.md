# <html><p style="text-align:center;">Developing Notes</p></html>

## Goals:
1. Add a new translation
2. Rewrite interactive mode
3. Pretty rendering
## Thoughts
- How will comparison output be rendered? Allowing an arbitrary number of translations will dramatically change UI
- Don't even think about UI
## Notes
- TSV Format: Book Name, Book Abbreviation, Book Number, Chapter Number, Verse Number, Verse

## Usages:
To compare 1 corinthians 13:7 in X translations:
- biblica 1 Corinthians 13:7 {kjv, vul}
- biblica 1cor 13:7 {kjv, vul, osb}

To list books from X translations:
- biblica -l {kjv, vul, osb, nev}