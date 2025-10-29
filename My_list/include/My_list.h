#ifndef MY_LIST_H
#define MY_LIST_H

#include "Common.h"

typedef double list_elem_t;
#define LIST_ELEM_STR "double"
#define LIST_ELEM_FRM "%lG"

uint64_t const    LIST_BIT_CANARY = 0XFACE'FACE'FACE'FACE;
list_elem_t const LIST_CANARY     = *(list_elem_t const *)&LIST_BIT_CANARY;

struct My_list_node {
    size_t prev,
           next;
    list_elem_t val;
};

size_t const LIST_EXPANSION          = 2; static_assert(LIST_EXPANSION > 1);
size_t const LIST_REDUCTION_CRITERIA = 4; static_assert(LIST_REDUCTION_CRITERIA > 1);
size_t const LIST_REDUCTION          = 2; static_assert(LIST_REDUCTION > 1 and
                                                        LIST_REDUCTION <= LIST_REDUCTION_CRITERIA);
size_t const LIST_MIN_CAPACITY       = 5; static_assert(LIST_MIN_CAPACITY > 1);

size_t const MY_LIST_UNAVIABLE_IDX = -1;

struct My_list {
    ON_DEBUG(Var_info var_info;)
    size_t            capacity;
    My_list_node      *buffer;
    size_t            first_empty;

    bool              is_valid;
};

errno_t My_list_Ctor(My_list *list_ptr, size_t start_capacity
          ON_DEBUG(, Var_info var_info));

#ifdef _DEBUG
#define LIST_CREATE(name, start_capacity, handler)                      \
My_list name = {};                                                      \
handler(My_list_Ctor, &name, start_capacity,                            \
        Var_info{Position_info{__FILE__, __func__, __LINE__}, #name})
#else
#define LIST_CREATE(name, start_capacity, handler)  \
My_list name = {};                                  \
handler(My_list_Ctor, &name, start_capacity)
#endif

void My_list_Dtor(My_list *list_ptr);

#define LIST_INVALID               0B10'000'000'000
#define LIST_SMALL_CAPACITY        0B100'000'000'000
#define LIST_INVALID_BUFFER        0B1'000'000'000'000
#define LIST_INVALID_STRUCTURE     0B10'000'000'000'000
#define LIST_BUFFER_CANARY_SPOILED 0B100'000'000'000'000
errno_t My_list_verify(My_list const *list_ptr, errno_t *err_ptr);

errno_t My_list_dump(My_list const *list_ptr, FILE *out_stream,
                     Position_info from_where,
                     size_t tab_count, bool need_extra_info); //TODO - possible delete

#define LIST_DUMP(out_stream, name, handler)                                                     \
handler(My_list_dump, out_stream, &name, Position_info{__FILE__, __func__, __LINE__}, 0, true)

errno_t My_list_visual_dump(My_list const *list_ptr, FILE *out_stream,
                            Position_info from_where);

errno_t My_list_begin(My_list const *list_ptr, size_t *dest);

errno_t My_list_end(My_list const *list_ptr, size_t *dest);

errno_t My_list_reallocate_up(My_list *list_ptr, size_t new_capacity);

errno_t My_list_insert_before(My_list *list_ptr, size_t *dest,
                              size_t before_what, list_elem_t val);

errno_t My_list_insert_after(My_list *list_ptr, size_t *dest,
                             size_t after_what, list_elem_t val);

#define LIST_TRY_TO_ACCESS_UNAVIABLE_POSITION 2'000
errno_t My_list_erase(My_list *list_ptr, size_t where);

#endif
