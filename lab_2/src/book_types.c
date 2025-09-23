#include "book_types.h"
#include "string.h"
#include "stdlib.h"

static error copy_str(char *dest, char *src)
{
    if (!src)
    {
        dest = NULL;
        return 0;
    }
    dest = strdup(src);
    if (!dest)
    {
        return ALLOC_ERROR;
    }
    return 0;
}

error deep_copy(book *dest, book *src)
{
    delete_inner_book_info(dest);
    dest->type = src->type;
    dest->pages = src->pages;
    error rc = copy_str(dest->author, src->author);
    if (rc)
    {
        return rc;
    }
    rc = copy_str(dest->title, src->title);
    if (rc)
    {
        return rc;
    }
    rc = copy_str(dest->publisher, src->publisher);
    if (rc)
    {
        return rc;
    }
    dest->lit_info = src->lit_info;
    if (src->type == TECHNICAL)
    {
        rc = copy_str(dest->lit_info.tech.industry, src->lit_info.tech.industry);
        if (rc)
        {
            return rc;
        }
    }
}

void delete_inner_book_info(book *b)
{
    if (!b)
    {
        return;
    }
    free(b->author);
    b->author = NULL;
    free(b->title);
    b->title = NULL;
    free(b->publisher);
    b->publisher = NULL;
    if (b->type == TECHNICAL)
    {
        free(b->lit_info.tech.industry);
        b->lit_info.tech.industry = NULL;
    }
}

void delete_book(book *b)
{
    if (!b)
    {
        return;
    }
    delete_inner_book_info(b);
    free(b);
    b = NULL;
}

void print_book(book *b) {
    printf("Название: %s, автор: %s, издательство: %s, %d страниц\n", b->title, b->author, b->publisher, b->pages);
    if (b->type == TECHNICAL){
        printf("Техническая литература. Отрасль %s, опубликована в %d году, ", b->lit_info.tech.industry, b->lit_info.tech.year_published);
        if (b->lit_info.tech.origin == LOCAL){
            printf("отечественная\n");
        } else {
            printf("переведенная\n");
        }
    } else if (b->type == KIDS){
        printf("Детская литература. Минимальный возраст %d, жанр - ", b->lit_info.kids.min_age);
        if (b->lit_info.kids.type == POEM){
            printf("стих\n");
        } else {
            printf("сказка\n");
        }
    } else if (b->type == FICTIONAL){
        printf("Художественная литература. Жанр - ");
        if (b->lit_info.fic.type == NOVEL){
            printf("роман\n");
        } else if (b->lit_info.fic.type == PLAY){
            printf("пьеса\n");
        } else if (b->lit_info.fic.type == POETRY){
            printf("поэзия\n");
        }
    }
}