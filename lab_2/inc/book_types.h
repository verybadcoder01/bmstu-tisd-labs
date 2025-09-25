#ifndef BOOK_TYPES_H__
#define BOOK_TYPES_H__

#include "stdbool.h"
#include "errors.h"
#include "stdio.h"

typedef enum
{
    LOCAL = 1,
    TRANSLATED = 2
} tech_lit_origin_t;
#define valid_tech_lit_origin(x) (x == LOCAL || x == TRANSLATED)

typedef enum
{
    NOVEL = 1,
    PLAY = 2,
    POETRY = 3
} fiction_lit_t;
#define valid_fiction_lit(x) (x == NOVEL || x == PLAY || x == POETRY)

typedef enum
{
    POEM = 1,
    FAIRY_TALE = 2
} kids_lit_t;
#define valid_kids_lit(x) (x == POEM || x == FAIRY_TALE)

typedef struct
{
    char *industry;
    tech_lit_origin_t origin;
    int year_published;
} technical_literature;

typedef struct
{
    fiction_lit_t type;
} fiction_literature;

typedef struct
{
    int min_age;
    kids_lit_t type;
} kids_literature;

typedef enum {
    FICTIONAL = 1,
    TECHNICAL = 2,
    KIDS = 3
} lit_type;
#define valid_lit_type(x) (x == FICTIONAL || x == TECHNICAL || x == KIDS)

typedef union
{
    fiction_literature fic;
    technical_literature tech;
    kids_literature kids;
} literature_data;

typedef struct
{
    literature_data lit_info;
    lit_type type;
    char *author;
    char *title;
    char *publisher;
    int pages;
} book;

error copy_str(char **dest, const char *src);

error deep_copy(book *dest, book* src);

void delete_inner_book_info(book *b);

void delete_book(book *b);

void print_book(book *b);

error scan_book(FILE *f, book *b);

void init_default_book(book *b);

#endif