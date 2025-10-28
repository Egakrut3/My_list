#include "My_list.h"

#undef FINAL_CODE
#define FINAL_CODE

errno_t My_list_Ctor(My_list *const list_ptr, size_t start_capacity
          ON_DEBUG(, Var_info const var_info)) {
    assert(list_ptr); assert(!list_ptr->is_valid); assert(start_capacity > 0);
    ON_DEBUG(
    assert(var_info.position.file_name); assert(var_info.position.function_name);
    assert(var_info.name);
    )

    ON_DEBUG(list_ptr->var_info = var_info;)

    if (start_capacity < LIST_MIN_CAPACITY) {
        start_capacity = LIST_MIN_CAPACITY;
    }
    list_ptr->capacity          = start_capacity;

    CHECK_FUNC(My_calloc, (void **)&list_ptr->buffer, start_capacity, sizeof(*list_ptr->buffer));

    list_ptr->buffer[0].prev = 0;
    list_ptr->buffer[0].next = 0;
    list_ptr->buffer[0].val  = LIST_CANARY;
    for (size_t i = 1; i < list_ptr->capacity - 1; ++i) {
        list_ptr->buffer[i].prev = MY_LIST_UNAVIABLE_IDX;
        list_ptr->buffer[i].next = i + 1;
        list_ptr->buffer[i].val  = LIST_CANARY;
    }
    list_ptr->buffer[list_ptr->capacity - 1].prev = MY_LIST_UNAVIABLE_IDX;
    list_ptr->buffer[list_ptr->capacity - 1].next = MY_LIST_UNAVIABLE_IDX;
    list_ptr->buffer[list_ptr->capacity - 1].val  = LIST_CANARY;

    list_ptr->first_empty = 1;

    list_ptr->is_valid = true;

    return 0;
}

void My_list_Dtor(My_list *const list_ptr) {
    assert(list_ptr);

    free(list_ptr->buffer);
    list_ptr->is_valid = false;
}

errno_t My_list_verify(My_list const *const list_ptr, errno_t *const err_ptr) {
    assert(list_ptr); assert(err_ptr);

    *err_ptr = 0;

    if (!list_ptr->is_valid) {
        *err_ptr |= LIST_INVALID;
    }

    if (list_ptr->capacity < LIST_MIN_CAPACITY) {
        *err_ptr |= LIST_SMALL_CAPACITY;
    }

    if (!list_ptr->buffer) {
        *err_ptr |= LIST_NULL_BUFFER;
    }

    enum State {
        UNDEFINED_STATE,
        EMPTY_STATE,
        USED_STATE,
    };
    State *state_buffer = nullptr;
    CHECK_FUNC(My_calloc, (void **)&state_buffer, list_ptr->capacity, sizeof(*state_buffer));

    for (size_t i = list_ptr->first_empty; i != MY_LIST_UNAVIABLE_IDX; i = list_ptr->buffer[i].next) {
        if (i >= list_ptr->capacity or state_buffer[i] != UNDEFINED_STATE) {
            *err_ptr |= LIST_INVALID_STRUCTURE;
            break;
        }
        state_buffer[i] = EMPTY_STATE;
    }

    if (state_buffer[0] != UNDEFINED_STATE or
        list_ptr->buffer[list_ptr->buffer[0].next].prev != 0) {
        *err_ptr |= LIST_INVALID_STRUCTURE;
    }
    state_buffer[0] = USED_STATE;
    for (size_t i = list_ptr->buffer[0].next; i != 0; i = list_ptr->buffer[i].next) {
        if (i >= list_ptr->capacity or state_buffer[i] != UNDEFINED_STATE or
            list_ptr->buffer[list_ptr->buffer[i].next].prev != i) {
            return LIST_INVALID_STRUCTURE;
        }
        state_buffer[i] = USED_STATE;
    }

    //TODO - is it neccessary
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
    if (list_ptr->buffer[0].val != LIST_CANARY) {
        *err_ptr |= LIST_BUFFER_CANARY_SPOILED;
    }
    #pragma GCC diagnostic pop
    for (size_t i = 0; i < list_ptr->capacity; ++i) {
        switch (state_buffer[i]) {
            case UNDEFINED_STATE:
                *err_ptr |= LIST_INVALID_STRUCTURE;
                break;

            case EMPTY_STATE:
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wfloat-equal"
                if (list_ptr->buffer[i].prev != MY_LIST_UNAVIABLE_IDX or
                    list_ptr->buffer[i].val  != LIST_CANARY) {
                    *err_ptr |= LIST_BUFFER_CANARY_SPOILED;
                }
                #pragma GCC diagnostic pop
                break;

            case USED_STATE:
                break;

            default:
                abort();
        }
    }

    return 0;
}

errno_t My_list_dump(FILE *const out_stream, My_list const *const list_ptr,
                     errno_t const err, Position_info const from_where,
                     size_t const tab_count, bool const need_extra_info) {
    assert(out_stream); assert(list_ptr);
    if (need_extra_info) {
        ON_DEBUG(
        assert(list_ptr->var_info.position.file_name); assert(list_ptr->var_info.position.function_name);
        assert(list_ptr->var_info.name);
        )
        assert(from_where.file_name);
    }

    #undef FINAL_CODE
    #define FINAL_CODE

    char *tab_str = nullptr;
    CHECK_FUNC(My_calloc, (void **)&tab_str, tab_count + 1, sizeof(*tab_str));
    for (size_t i = 0; i < tab_count; ++i) {
        tab_str[i] = '\t';
    }
    #undef FINAL_CODE
    #define FINAL_CODE  \
        free(tab_str);

    if (need_extra_info) {
        fprintf_s(out_stream, "%scalled at file %s, line %d in \"%s\" function: ", tab_str,
                  from_where.file_name, from_where.line, from_where.function_name);

        if (err & LIST_INVALID) {
            fprintf_s(out_stream, "List is invalid    ");
        }

        if (err & LIST_SMALL_CAPACITY) {
            fprintf_s(out_stream, "List capacity is too small    "); //TODO - I can't use \t there
        }

        if (err & LIST_NULL_BUFFER) {
            fprintf_s(out_stream, "List has null buffer    ");
        }

        if (err & LIST_INVALID_STRUCTURE) {
            fprintf_s(out_stream, "List has invalid structure    ");
        }

        if (err & LIST_BUFFER_CANARY_SPOILED) {
            fprintf_s(out_stream, "List buffer canary spoiled    ");
        }

        fprintf_s(out_stream, "\n%slist<" LIST_ELEM_STR ">[%p]"
                 ON_DEBUG(" \"%s\" declared in file %s, line %zu in \"%s\" function")
                 " {\n", tab_str,
                 list_ptr
                 ON_DEBUG(, list_ptr->var_info.name,
                 list_ptr->var_info.position.file_name, list_ptr->var_info.position.line,
                 list_ptr->var_info.position.function_name));
    }

    fprintf_s(out_stream, "%s\tcapacity = %zu\n", tab_str, list_ptr->capacity);

    fprintf_s(out_stream, "%s\tbuffer[%zu] = [%p] {\n", tab_str, list_ptr->capacity, list_ptr->buffer);
    if (list_ptr->buffer) {
        bool valid_structure = true;
        enum State {
            UNDEFINED_STATE,
            EMPTY_STATE,
            USED_STATE,
        };
        State *state_buffer = nullptr;
        CHECK_FUNC(My_calloc, (void **)&state_buffer, list_ptr->capacity, sizeof(*state_buffer));

        for (size_t i = list_ptr->first_empty; i != MY_LIST_UNAVIABLE_IDX; i = list_ptr->buffer[i].next) {
            if (i >= list_ptr->capacity or state_buffer[i] != UNDEFINED_STATE) {
                valid_structure = false;
                break;
            }
            state_buffer[i] = EMPTY_STATE;
        }

        if (state_buffer[0] != UNDEFINED_STATE or
            list_ptr->buffer[list_ptr->buffer[0].next].prev != 0) {
            valid_structure = false;
        }
        state_buffer[0] = USED_STATE;
        for (size_t i = list_ptr->buffer[0].next; i != 0; i = list_ptr->buffer[i].next) {
            if (i >= list_ptr->capacity or state_buffer[i] != UNDEFINED_STATE or
                list_ptr->buffer[list_ptr->buffer[i].next].prev != i) {
                valid_structure = false;
            }
            state_buffer[i] = USED_STATE;
        }

        for (size_t i = 0; i < list_ptr->capacity; ++i) {
            switch (state_buffer[i]) {
                case UNDEFINED_STATE:
                    valid_structure = false;
                    break;

                case EMPTY_STATE:
                case USED_STATE:
                    break;

                default:
                    abort();
            }
        }

        for (size_t i = 0; i < list_ptr->capacity; ++i) {
            fprintf_s(out_stream, "%s\t\t[%zu] .prev = %zu, .next = %zu, .val = " LIST_ELEM_FRM,
                      tab_str,
                      i, list_ptr->buffer[i].prev, list_ptr->buffer[i].next, list_ptr->buffer[i].val);
            if (valid_structure and state_buffer[i] == EMPTY_STATE) {
                fprintf_s(out_stream, " (Unaccessible)");
            }
            fprintf_s(out_stream, "\n");
        }
    }
    else {
        fprintf_s(out_stream, "%s\tInvalid buffer\n", tab_str);
    }
    fprintf_s(out_stream, "%s\t}\n", tab_str);

    fprintf_s(out_stream, "%s\tfirst_empty = %zu\n", tab_str, list_ptr->first_empty);

    fprintf_s(out_stream, "%s\tis_valid = %d\n", tab_str, list_ptr->is_valid);

    fprintf_s(out_stream, "%s}\n", tab_str);

    CLEAR_RESOURCES();
    return 0;
}
