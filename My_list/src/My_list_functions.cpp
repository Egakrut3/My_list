#include "My_list.h"

#undef FINAL_CODE
#define FINAL_CODE

errno_t My_list_insert(My_list *const list_ptr, size_t *const dest,
                       size_t const before_what, list_elem_t const val) {
    assert(list_ptr); assert(dest);
    ON_DEBUG(
    errno_t verify_err = 0;
    CHECK_FUNC(My_list_verify, list_ptr, &verify_err);
    assert(!verify_err);
    )

    if (list_ptr->first_empty == MY_LIST_UNAVIABLE_IDX) {
        CHECK_FUNC(My_realloc, (void **)&list_ptr->buffer, list_ptr->buffer,
                               (LIST_EXPANSION * list_ptr->capacity) * sizeof(*list_ptr->buffer));

        for (size_t i = list_ptr->capacity; i < LIST_EXPANSION * list_ptr->capacity - 1; ++i) {
            list_ptr->buffer[i].prev = MY_LIST_UNAVIABLE_IDX;
            list_ptr->buffer[i].next = i + 1;
            list_ptr->buffer[i].val  = LIST_CANARY;
        }
        list_ptr->buffer[LIST_EXPANSION * list_ptr->capacity - 1].prev = MY_LIST_UNAVIABLE_IDX;
        list_ptr->buffer[LIST_EXPANSION * list_ptr->capacity - 1].next = MY_LIST_UNAVIABLE_IDX;
        list_ptr->buffer[LIST_EXPANSION * list_ptr->capacity - 1].val  = LIST_CANARY;

        list_ptr->first_empty = list_ptr->capacity;
        list_ptr->capacity *= LIST_EXPANSION;
    }

    size_t next_empty = list_ptr->buffer[list_ptr->first_empty].next;

    list_ptr->buffer[list_ptr->buffer[before_what].prev].next = list_ptr->first_empty;
    list_ptr->buffer[list_ptr->first_empty].prev = list_ptr->buffer[before_what].prev;

    list_ptr->buffer[before_what].prev = list_ptr->first_empty;
    list_ptr->buffer[list_ptr->first_empty].next = before_what;

    list_ptr->buffer[list_ptr->first_empty].val = val;

    *dest = list_ptr->first_empty;
    list_ptr->first_empty = next_empty;

    return 0;
}

errno_t My_list_erase(My_list *const list_ptr, size_t const where) {
    assert(list_ptr);
    ON_DEBUG(
    errno_t verify_err = 0;
    CHECK_FUNC(My_list_verify, list_ptr, &verify_err);
    assert(!verify_err);
    )

    if (where >= list_ptr->capacity or where == 0 or
        list_ptr->buffer[where].next == MY_LIST_UNAVIABLE_IDX or
        list_ptr->buffer[where].prev == MY_LIST_UNAVIABLE_IDX) {
        return LIST_TRY_TO_ACCESS_UNAVIABLE_POSITION;
    }

    list_ptr->buffer[list_ptr->buffer[where].prev].next = list_ptr->buffer[where].next;
    list_ptr->buffer[list_ptr->buffer[where].next].prev = list_ptr->buffer[where].prev;

    list_ptr->buffer[where].next = list_ptr->first_empty;
    list_ptr->first_empty = where;
    list_ptr->buffer[where].prev = MY_LIST_UNAVIABLE_IDX;
    list_ptr->buffer[where].val  = LIST_CANARY;

    return 0;
}
