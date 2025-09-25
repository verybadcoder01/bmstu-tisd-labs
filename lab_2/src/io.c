#include "io.h"
#include "ctype.h"
#include "malloc.h"

error scan(FILE *f, char **result, size_t *res_size){
    size_t cap = 32, len = 0;
    char *buf = malloc(cap);
    if (!buf){
        return ALLOC_ERROR;
    }
    int ch;
    while ((ch = fgetc(f)) != EOF && (isspace(ch) || ch == '\n'));
    if (ch == EOF){
        free(buf);
        return EOF;
    }
    buf[len++] = (char)ch;
    while ((ch = fgetc(f)) != EOF && ch != '\n'){
        if (len >= cap - 1){
            cap *= 2;
            char *temp = realloc(buf, cap);
            if (!temp){
                free(buf);
                return ALLOC_ERROR;
            }
            buf = temp;
        }
        buf[len++] = (char)ch;
    }
    buf[len] = '\0';
    if (len + 1 < cap){
        char *temp = realloc(buf, len + 1);
        if (temp){
            buf = temp;
            cap = len + 1;
        }
    }
    *result = buf;
    *res_size = len;
    return 0;
}