#include "Common.h"
#include "Option_manager.h"
#include "My_list.h"

int main(int const argc, char const *const *const argv) {
    assert(argc > 0); assert(argv); assert(*argv);

    #undef FINAL_CODE
    #define FINAL_CODE

    Config cur_config = {};
    #undef FINAL_CODE
    #define FINAL_CODE              \
        Config_Dtor(&cur_config);
    MAIN_CHECK_FUNC(Config_Ctor, &cur_config, argc, argv);

    #undef FINAL_CODE
    #define FINAL_CODE              \
        Config_Dtor(&cur_config);   \
        My_list_Dtor(&cur_list);
    LIST_CREATE(cur_list, 5, MAIN_CHECK_FUNC);

    size_t pos1 = 0,
           pos2 = 0,
           pos3 = 0,
           pos4 = 0,
           pos5 = 0;
    MAIN_CHECK_FUNC(My_list_insert, &cur_list, &pos1, 0, 1);
    MAIN_CHECK_FUNC(My_list_insert, &cur_list, &pos2, pos1, 2);
    MAIN_CHECK_FUNC(My_list_insert, &cur_list, &pos3, pos2, 3);
    MAIN_CHECK_FUNC(My_list_insert, &cur_list, &pos4, pos3, 4);
    MAIN_CHECK_FUNC(My_list_insert, &cur_list, &pos5, pos4, 5);
    MAIN_CHECK_FUNC(My_list_erase, &cur_list, pos2);
    //MAIN_CHECK_FUNC(My_list_insert, &cur_list, &pos2, pos3, 20);
    My_list_vizual_dump(&cur_list);

    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;
}
