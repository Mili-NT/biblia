
typedef struct {
    int book;
    int chapter;
    int verse;
    char *text;
} verse;

typedef struct {
    int number;
    char *name;
    char *abbr;
} book;

typedef struct {
    char *name;
    char *abbr;
    book *books;
    int num_books;
} translation;

extern verse verses[];

extern int verses_length;

extern book books[];

extern int books_length;
