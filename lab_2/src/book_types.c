#define _POSIX_C_SOURCE 200809L
#include "book_types.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "io.h"

error copy_str(char **dest, const char *src)
{
    if (src == NULL)
    {
        *dest = NULL;
        return 0;
    }
    *dest = malloc(strlen(src) + 1);
    if (!*dest)
    {
        return ALLOC_ERROR;
    }
    strcpy(*dest, src);
    return 0;
}
error deep_copy(book *dest, book *src)
{
    delete_inner_book_info(dest);
    dest->type = src->type;
    dest->pages = src->pages;
    error rc = copy_str(&dest->author, src->author);
    if (rc)
    {
        return rc;
    }
    rc = copy_str(&dest->title, src->title);
    if (rc)
    {
        return rc;
    }
    rc = copy_str(&dest->publisher, src->publisher);
    if (rc)
    {
        return rc;
    }
    dest->lit_info = src->lit_info;
    if (src->type == TECHNICAL)
    {
        rc = copy_str(&dest->lit_info.tech.industry, src->lit_info.tech.industry);
        if (rc)
        {
            return rc;
        }
    }
    return 0;
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

error scan_book(FILE *f, book *b)
{
    size_t size = 0;
    b->author = NULL;
    b->publisher = NULL;
    b->title = NULL;
    b->type = 0;
    interface_printf("Введите автора книги\n");
    error rc = scan(f, &b->author, &size);
    if (rc)
    {
        return rc;
    }
    interface_printf("Введите название книги\n");
    rc = scan(f, &b->title, &size);
    if (rc)
    {
        if (rc == EOF)
        {
            rc = INPUT_ERROR;
        }
        return rc;
    }
    interface_printf("Введите издателя книги\n");
    rc = scan(f, &b->publisher, &size);
    if (rc)
    {
        if (rc == EOF)
        {
            rc = INPUT_ERROR;
        }
        return rc;
    }
    interface_printf("Введите количество страниц в книге\n");
    rc = fscanf(f, "%d", &b->pages);
    if (rc != 1)
    {
        return INPUT_ERROR;
    }
    interface_printf("Введите тип книги: 1 - художественная, 2 - техническая, 3 - детская\n");
    rc = fscanf(f, "%d", (int *)&b->type);
    if (rc != 1)
    {
        return INPUT_ERROR;
    }
    if (!valid_lit_type(b->type)){
        return INVALID_LIT_TYPE;
    }
    switch (b->type)
    {
    case FICTIONAL:
        interface_printf("Ввелите тип художественной литературы: 1 - роман, 2 - пьеса, 3 - поэзия\n");
        rc = fscanf(f, "%d", (int *)&b->lit_info.fic.type);
        if (rc != 1)
        {
            return INPUT_ERROR;
        }
        if (!valid_fiction_lit(b->lit_info.fic.type)){
            return INVALID_FICTION_LIT_TYPE;
        }
        break;
    case KIDS:
        interface_printf("Введите тип детской книги: 1 - стих, 2 - сказка и минимальный возраст\n");
        rc = fscanf(f, "%d %d", (int *)&b->lit_info.kids.type, &b->lit_info.kids.min_age);
        if (rc != 2)
        {
            return INPUT_ERROR;
        }
        if (!valid_kids_lit(b->lit_info.kids.type)){
            return INVALID_KIDS_LIT_TYPE;
        }
        break;
    case TECHNICAL:
        b->lit_info.tech.industry = NULL;
        interface_printf("Введите индустрию, описанную в данной книге\n");
        rc = scan(f, &b->lit_info.tech.industry, &size);
        if (rc)
        {
            if (rc == EOF)
            {
                rc = INPUT_ERROR;
            }
            return rc;
        }
        interface_printf("Введите, является ли книга отечественной (1) или переведенной (2) и год ее публикации\n");
        rc = fscanf(f, "%d %d", (int *)&b->lit_info.tech.origin, &b->lit_info.tech.year_published);
        if (rc != 2)
        {
            return INPUT_ERROR;
        }
        if (!valid_tech_lit_origin(b->lit_info.tech.origin)){
            return INVALID_TECH_LIT_ORIGIN;
        }
        break;
    default:
        return INVALID_LIT_TYPE;
    }
    return 0;
}

void print_book(book *b)
{
    printf("Название: %s, автор: %s, издательство: %s, %d страниц\n", b->title, b->author, b->publisher, b->pages);
    if (b->type == TECHNICAL)
    {
        printf("Техническая литература. Отрасль %s, опубликована в %d году, ", b->lit_info.tech.industry, b->lit_info.tech.year_published);
        if (b->lit_info.tech.origin == LOCAL)
        {
            printf("отечественная\n");
        }
        else
        {
            printf("переведенная\n");
        }
    }
    else if (b->type == KIDS)
    {
        printf("Детская литература. Минимальный возраст %d, жанр - ", b->lit_info.kids.min_age);
        if (b->lit_info.kids.type == POEM)
        {
            printf("стих\n");
        }
        else
        {
            printf("сказка\n");
        }
    }
    else if (b->type == FICTIONAL)
    {
        printf("Художественная литература. Жанр - ");
        if (b->lit_info.fic.type == NOVEL)
        {
            printf("роман\n");
        }
        else if (b->lit_info.fic.type == PLAY)
        {
            printf("пьеса\n");
        }
        else if (b->lit_info.fic.type == POETRY)
        {
            printf("поэзия\n");
        }
    }
}

void init_default_book(book *b) {
    b->author = NULL;
    b->publisher = NULL;
    b->title = NULL;
    b->pages = 0;
    b->lit_info.fic.type = NOVEL;
    b->type = FICTIONAL;    
}
