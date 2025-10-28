#include "File_input.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "My_functions.h"

#undef FINAL_CODE
#define FINAL_CODE

errno_t get_path_filesize(char const *const path, __int64 *const filesize)
{
    assert(path); assert(filesize);

    struct _stat64 buffer = {};

    CHECK_FUNC(_stat64, path, &buffer);
    *filesize = buffer.st_size;

    assert(*filesize > 0);

    return 0;
}

errno_t get_opened_filesize(FILE *const cur_file, __int64 *const filesize)
{
    assert(cur_file); assert(filesize);

    struct _stat64 buffer = {};

    CHECK_FUNC(_fstat64, fileno(cur_file), &buffer);
    *filesize = buffer.st_size;

    assert(*filesize > 0);

    return 0;
}

errno_t get_all_content(FILE *const stream, size_t *const filesize_dest, char **const buffer)
{
    assert(stream); assert(buffer);

    size_t filesize = 0;
    get_opened_filesize(stream, (__int64 *)&filesize);
    if (filesize_dest) {
        *filesize_dest = filesize;
    }

    CHECK_FUNC(My_calloc, (void **)buffer, filesize + 1, sizeof(char));
    CHECK_FUNC(My_fread, *buffer, sizeof(char), filesize, stream);

    return 0;
}
