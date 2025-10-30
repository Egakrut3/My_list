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

    CHECK_FUNC(My_calloc, (void **)&list_ptr->buffer, list_ptr->capacity, sizeof(*list_ptr->buffer));

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

    #undef FINAL_CODE
    #define FINAL_CODE

    *err_ptr = 0;

    if (!list_ptr->is_valid) {
        *err_ptr |= LIST_INVALID;
    }

    if (list_ptr->capacity < LIST_MIN_CAPACITY) {
        *err_ptr |= LIST_SMALL_CAPACITY;
    }

    if (!list_ptr->buffer) {
        *err_ptr |= LIST_INVALID_BUFFER;
    }

    enum State {
        UNDEFINED_STATE,
        EMPTY_STATE,
        USED_STATE,
    };
    State *state_buffer = nullptr;
    CHECK_FUNC(My_calloc, (void **)&state_buffer, list_ptr->capacity, sizeof(*state_buffer));
    #undef FINAL_CODE
    #define FINAL_CODE  \
        free(state_buffer);

    for (size_t i = list_ptr->first_empty; i != MY_LIST_UNAVIABLE_IDX; i = list_ptr->buffer[i].next) {
        if (i >= list_ptr->capacity or state_buffer[i] != UNDEFINED_STATE) {
            *err_ptr |= LIST_INVALID_STRUCTURE;
            break;
        }
        state_buffer[i] = EMPTY_STATE;
    }

    if (state_buffer[0] != UNDEFINED_STATE or
        list_ptr->buffer[0].next >= list_ptr->capacity or
        list_ptr->buffer[list_ptr->buffer[0].next].prev != 0) {
        *err_ptr |= LIST_INVALID_STRUCTURE;
    }
    state_buffer[0] = USED_STATE;
    for (size_t i = list_ptr->buffer[0].next; i != 0; i = list_ptr->buffer[i].next) {
        if (state_buffer[i] != UNDEFINED_STATE or
            list_ptr->buffer[i].next >= list_ptr->capacity or
            list_ptr->buffer[list_ptr->buffer[i].next].prev != i) {
            *err_ptr |= LIST_INVALID_STRUCTURE;
            break;
        }
        state_buffer[i] = USED_STATE;
    }

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

    CLEAR_RESOURCES();
    return 0;
}

errno_t My_list_dump(My_list const *const list_ptr, FILE *const out_stream,
                     Position_info const from_where,
                     size_t const tab_count, bool const need_extra_info) {
    assert(list_ptr); assert(out_stream);
    if (need_extra_info) {
        ON_DEBUG(
        assert(list_ptr->var_info.position.file_name); assert(list_ptr->var_info.position.function_name);
        assert(list_ptr->var_info.name);
        )
        assert(from_where.file_name); assert(from_where.function_name);
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

    errno_t verify_err = 0;
    CHECK_FUNC(My_list_verify, list_ptr, &verify_err);

    if (need_extra_info) {
        fprintf_s(out_stream, "%scalled at file %s, line %zu in \"%s\" function: ", tab_str,
                  from_where.file_name, from_where.line, from_where.function_name);

        if (!verify_err) {
            fprintf_s(out_stream, "No error");
        }

        if (verify_err & LIST_INVALID) {
            fprintf_s(out_stream, "List is invalid    ");
        }

        if (verify_err & LIST_SMALL_CAPACITY) {
            fprintf_s(out_stream, "List capacity is too small    ");
        }

        if (verify_err & LIST_INVALID_BUFFER) {
            fprintf_s(out_stream, "List has invalid buffer    ");
        }

        if (verify_err & LIST_INVALID_STRUCTURE) {
            fprintf_s(out_stream, "List has invalid structure    ");
        }

        if (verify_err & LIST_BUFFER_CANARY_SPOILED) {
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
    if (verify_err & LIST_INVALID_BUFFER) {
        fprintf_s(out_stream, "%s\tInvalid buffer\n", tab_str);
    }
    else {
        //TODO - how to dump it
    }
    fprintf_s(out_stream, "%s\t}\n", tab_str);

    fprintf_s(out_stream, "%s\tfirst_empty = %zu\n", tab_str, list_ptr->first_empty);

    fprintf_s(out_stream, "%s\tis_valid = %d\n", tab_str, list_ptr->is_valid);

    fprintf_s(out_stream, "%s}\n", tab_str);

    CLEAR_RESOURCES();
    return 0;
}

errno_t My_list_visual_dump(My_list const *const list_ptr, FILE *const out_stream,
                            Position_info const from_where) {
    assert(list_ptr); assert(out_stream);
    ON_DEBUG(
    assert(list_ptr->var_info.position.file_name); assert(list_ptr->var_info.position.function_name);
    assert(list_ptr->var_info.name);
    )
    assert(from_where.file_name); assert(from_where.function_name);

    #undef FINAL_CODE
    #define FINAL_CODE

    errno_t verify_err = 0;
    CHECK_FUNC(My_list_verify, list_ptr, &verify_err);

    fprintf_s(out_stream, "<pre>\n");

    fprintf_s(out_stream, "called at file %s, line %zu in \"%s\" function: ",
                          from_where.file_name, from_where.line, from_where.function_name);

    if (!verify_err) {
        fprintf_s(out_stream, "No error");
    }

    if (verify_err & LIST_INVALID) {
        fprintf_s(out_stream, "List is invalid    ");
    }

    if (verify_err & LIST_SMALL_CAPACITY) {
        fprintf_s(out_stream, "List capacity is too small    ");
    }

    if (verify_err & LIST_INVALID_BUFFER) {
        fprintf_s(out_stream, "List has invalid buffer    ");
    }

    if (verify_err & LIST_INVALID_STRUCTURE) {
        fprintf_s(out_stream, "List has invalid structure    ");
    }

    if (verify_err & LIST_BUFFER_CANARY_SPOILED) {
        fprintf_s(out_stream, "List buffer canary spoiled    ");
    }

    fprintf_s(out_stream, "\nlist<" LIST_ELEM_STR ">[%p]"
                 ON_DEBUG(" \"%s\" declared in file %s, line %zu in \"%s\" function")
                          " {\n",
                          list_ptr
               ON_DEBUG(, list_ptr->var_info.name,
                          list_ptr->var_info.position.file_name, list_ptr->var_info.position.line,
                          list_ptr->var_info.position.function_name));

    fprintf_s(out_stream, "\tcapacity = %zu\n", list_ptr->capacity);

    fprintf_s(out_stream, "\tbuffer[%zu] = [%p]\n", list_ptr->capacity, list_ptr->buffer);
    if (verify_err & LIST_INVALID_BUFFER) {
        fprintf_s(out_stream, "\tInvalid buffer\n");
    }
    else {
        FILE *dot_stream = nullptr;
        #define DOT_FILE_PATH "./Visual_html/DOT_file.txt"
        CHECK_FUNC(fopen_s, &dot_stream, DOT_FILE_PATH, "w");

        char const background_color[]     = "white",
                   empty_arrow_color[]    = "violet",
                   toward_arrow_color[]   = "green",
                   backward_arrow_color[] = "red",
                   first_empty_color[]    = "violet",
                   empty_color[]          = "yellow",
                   host_color[]           = "orange",
                   common_color[]         = "white"; //TODO - 

        fprintf_s(dot_stream, "digraph {\n");
        fprintf_s(dot_stream, "\trankdir = LR\n");
        fprintf_s(dot_stream, "\tnode [shape = plaintext]\n");
        fprintf_s(dot_stream, "\tbgcolor = \"%s\"\n", background_color);

        //TODO - is %zd a good practice?
        if (verify_err & LIST_INVALID_STRUCTURE) {
            fprintf_s(dot_stream, "\tnode0 [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\">"
                                           "<TR><TD COLSPAN=\"2\" PORT=\"top\" "
                                                   "BGCOLOR=\"%s\">HOST</TD></TR>"
                                           "<TR><TD PORT=\"prev\">TAIL = %zd</TD>"
                                               "<TD PORT=\"next\">HEAD = %zd</TD></TR>"
                                           "<TR><TD COLSPAN=\"2\" PORT=\"bottom\">"
                                                "CANARY = " LIST_ELEM_FRM "</TD></TR>"
                                           "</TABLE>>]\n",
                                  host_color,
                                  list_ptr->buffer[0].prev, list_ptr->buffer[0].next,
                                  list_ptr->buffer[0].val);
            for (size_t i = 1; i < list_ptr->capacity; ++i) {
                if (i == list_ptr->first_empty) {
                    fprintf_s(dot_stream, "\tnode%zu [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\">"
                                                     "<TR><TD COLSPAN=\"2\" PORT=\"top\" "
                                                             "BGCOLOR=\"%s\">"
                                                         "FIRST EMPTY</TD></TR>"
                                                     "<TR><TD COLSPAN=\"2\">idx = %zu</TD></TR>"
                                                     "<TR><TD PORT=\"prev\">prev = %zd</TD>"
                                                         "<TD PORT=\"next\">next = %zd</TD></TR>"
                                                     "<TR><TD COLSPAN=\"2\" PORT=\"bottom\">"
                                                         "val = " LIST_ELEM_FRM "</TD></TR>"
                                                     "</TABLE>>]\n",
                                          i,
                                          first_empty_color,
                                          i,
                                          list_ptr->buffer[i].prev, list_ptr->buffer[i].next,
                                          list_ptr->buffer[i].val);
                }
                else {
                    fprintf_s(dot_stream, "\tnode%zu [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\">"
                                                     "<TR><TD PORT=\"top\" COLSPAN=\"2\">"
                                                         "idx = %zu</TD></TR>"
                                                     "<TR><TD PORT=\"prev\">prev = %zd</TD>"
                                                         "<TD PORT=\"next\">next = %zd</TD></TR>"
                                                     "<TR><TD PORT=\"bottom\" COLSPAN=\"2\">"
                                                         "val = " LIST_ELEM_FRM "</TD></TR>"
                                                     "</TABLE>>]\n",
                                          i, i,
                                          list_ptr->buffer[i].prev, list_ptr->buffer[i].next,
                                          list_ptr->buffer[i].val);
                }
            }
        }
        else {
            if (list_ptr->first_empty != MY_LIST_UNAVIABLE_IDX) {
                fprintf_s(dot_stream, "\tnode%zu [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                                                               "BGCOLOR=\"%s\">"
                                                 "<TR><TD COLSPAN=\"2\" PORT=\"top\" BGCOLOR=\"%s\">"
                                                     "FIRST EMPTY</TD></TR>"
                                                 "<TR><TD COLSPAN=\"2\">idx = %zu</TD></TR>"
                                                 "<TR><TD PORT=\"prev\">prev = %zd</TD>"
                                                     "<TD PORT=\"next\">next = %zd</TD></TR>"
                                                 "<TR><TD COLSPAN=\"2\" PORT=\"bottom\">"
                                                     "CANARY = " LIST_ELEM_FRM "(Poison)</TD></TR>"
                                                 "</TABLE>>]\n",
                                      list_ptr->first_empty,
                                      empty_color, first_empty_color,
                                      list_ptr->first_empty,
                                      list_ptr->buffer[list_ptr->first_empty].prev,
                                      list_ptr->buffer[list_ptr->first_empty].next,
                                      list_ptr->buffer[list_ptr->first_empty].val);

                for (size_t i = list_ptr->buffer[list_ptr->first_empty].next; i != MY_LIST_UNAVIABLE_IDX;
                            i = list_ptr->buffer[i].next) {
                    fprintf_s(dot_stream, "\tnode%zu [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                                                                   "BGCOLOR=\"%s\">"
                                                     "<TR><TD COLSPAN=\"2\" PORT=\"top\">"
                                                         "idx = %zu</TD></TR>"
                                                     "<TR><TD PORT=\"prev\">prev = %zd</TD>"
                                                         "<TD PORT=\"next\">next = %zd</TD></TR>"
                                                     "<TR><TD COLSPAN=\"2\" PORT=\"bottom\">"
                                                         "CANARY = " LIST_ELEM_FRM "(Poison)</TD></TR>"
                                                     "</TABLE>>]\n",
                                          i,
                                          empty_color,
                                          i,
                                          list_ptr->buffer[i].prev,
                                          list_ptr->buffer[i].next,
                                          list_ptr->buffer[i].val);
                }
            }

            fprintf_s(dot_stream, "\tnode0 [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" BGCOLOR=\"%s\">"
                                           "<TR><TD COLSPAN=\"2\" PORT=\"top\" "
                                                   "BGCOLOR=\"%s\">HOST</TD></TR>"
                                           "<TR><TD PORT=\"prev\">TAIL = %zd</TD>"
                                               "<TD PORT=\"next\">HEAD = %zd</TD></TR>"
                                           "<TR><TD COLSPAN=\"2\" PORT=\"bottom\">"
                                                "CANARY = " LIST_ELEM_FRM "</TD></TR>"
                                           "</TABLE>>]\n",
                                  common_color, host_color,
                                  list_ptr->buffer[0].prev, list_ptr->buffer[0].next,
                                  list_ptr->buffer[0].val);
            for (size_t i = list_ptr->buffer[0].next; i != 0; i = list_ptr->buffer[i].next) {
                fprintf_s(dot_stream, "\tnode%zu [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                                                               "BGCOLOR=\"%s\">"
                                                 "<TR><TD PORT=\"top\" COLSPAN=\"2\">"
                                                     "idx = %zu</TD></TR>"
                                                 "<TR><TD PORT=\"prev\">prev = %zd</TD>"
                                                     "<TD PORT=\"next\">next = %zd</TD></TR>"
                                                 "<TR><TD PORT=\"bottom\" COLSPAN=\"2\">"
                                                     "val = " LIST_ELEM_FRM "</TD></TR>"
                                                 "</TABLE>>]\n",
                                      i,
                                      common_color,
                                      i,
                                      list_ptr->buffer[i].prev, list_ptr->buffer[i].next,
                                      list_ptr->buffer[i].val);
            }
        }

        for (size_t i = 0; i < list_ptr->capacity - 1; ++i) {
            fprintf_s(dot_stream, "\tnode%zu -> node%zu [color = %s]\n", i, i + 1, background_color);
        }

        if (verify_err & LIST_INVALID_STRUCTURE) {
            for (size_t i = 0; i < list_ptr->capacity; ++i) {
                if (list_ptr->buffer[i].next != MY_LIST_UNAVIABLE_IDX) {
                    fprintf_s(dot_stream, "\tnode%zu:next -> node%zu:top"
                                          "[color = %s, constraint = false]\n",
                                          i, list_ptr->buffer[i].next, toward_arrow_color);
                }

                if (list_ptr->buffer[i].prev != MY_LIST_UNAVIABLE_IDX) {
                    fprintf_s(dot_stream, "\tnode%zu:prev -> node%zu:bottom"
                                          "[color = %s, constraint = false]\n",
                                          i, list_ptr->buffer[i].prev, backward_arrow_color);
                }
            }
        }
        else {
            for (size_t i = list_ptr->first_empty; i != MY_LIST_UNAVIABLE_IDX and
                            list_ptr->buffer[i].next != MY_LIST_UNAVIABLE_IDX;
                        i = list_ptr->buffer[i].next) {
                fprintf_s(dot_stream, "\tnode%zu:next -> node%zu:top"
                                      "[color = %s, constraint = false]\n",
                                      i, list_ptr->buffer[i].next, empty_arrow_color);
            }

            fprintf_s(dot_stream, "\tnode0:next -> node%zu:top"
                                  "[color = %s, constraint = false]\n",
                                  list_ptr->buffer[0].next, toward_arrow_color);
            fprintf_s(dot_stream, "\tnode0:prev -> node%zu:bottom"
                                  "[color = %s, constraint = false]\n",
                                  list_ptr->buffer[0].prev, backward_arrow_color);
            for (size_t i = list_ptr->buffer[0].next; i != 0; i = list_ptr->buffer[i].next) {
                fprintf_s(dot_stream, "\tnode%zu:next -> node%zu:top"
                                      "[color = %s, constraint = false]\n",
                                      i, list_ptr->buffer[i].next, toward_arrow_color);
                fprintf_s(dot_stream, "\tnode%zu:prev -> node%zu:bottom"
                                      "[color = %s, constraint = false]\n",
                                      i, list_ptr->buffer[i].prev, backward_arrow_color);
            }
        }

        fprintf_s(dot_stream, "}");

        fclose(dot_stream);
        #define DOT_OUTPUT_NAME "DOT_output.png"
        system("dot -Tpng " DOT_FILE_PATH " > ./Visual_html/" DOT_OUTPUT_NAME); //TODO - how to change filename

        fprintf_s(out_stream, "\t<img src=\" " DOT_OUTPUT_NAME "\" width=1500/>\n");
    }

    fprintf_s(out_stream, "\tfirst_empty = %zu\n", list_ptr->first_empty);

    fprintf_s(out_stream, "\tis_valid = %d\n", list_ptr->is_valid);

    fprintf_s(out_stream, "}\n");

    fprintf_s(out_stream, "</pre>\n");

    return 0;
}
