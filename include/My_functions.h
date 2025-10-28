#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

#include "Common_declarations.h"

errno_t My_calloc(void **dest, size_t num, size_t size);

errno_t My_realloc(void **dest, void *prev_ptr, size_t size);

errno_t My_fread(void *__restrict buffer, size_t size, size_t num,
                 FILE *__restrict stream);

errno_t My_fwrite(const void *__restrict buffer, size_t size, size_t num,
                  FILE *__restrict stream);

errno_t My_sscanf_s(size_t count, char const *__restrict buffer,
                    char const *__restrict format, ...);

errno_t My_scanf_s(size_t const count, char const *__restrict const format, ...);

#endif
