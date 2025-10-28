#include "Option_manager.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

enum Option
{
    HELP_OPTION,
    __OPTION_COUNT,
};

#undef FINAL_CODE
#define FINAL_CODE

static errno_t set_help_config(Config *const config_ptr,
                               char const *const **const str_ptr_ptr, char const *const end_str)
{
    assert(config_ptr); assert(!config_ptr->is_valid);
    assert(str_ptr_ptr); assert(*str_ptr_ptr); assert(**str_ptr_ptr != end_str);
    assert(!strcmp(**str_ptr_ptr, "--help"));

    config_ptr->is_help = true;
    printf("Usage: Test.exe [options] file...\nOptions:\n"
           "\t%-10s %s\n",
           "--help", "Display the information"
           );
    return 0;
}

static char const *const flag_option_arr[__OPTION_COUNT] = {
       "--help",
};

static errno_t (*const set_option_arr[__OPTION_COUNT])(Config *,
                                                       char const *const **, char const *) = {
       &set_help_config,
};


static errno_t select_option_setter(Config *const config_ptr,
                                    char const *const **const str_ptr_ptr, char const *const end_str,
                                    bool *const used_option)
{
    assert(config_ptr); assert(!config_ptr->is_valid);
    assert(str_ptr_ptr); assert(*str_ptr_ptr); assert(**str_ptr_ptr != end_str);
    assert(used_option);

    for (size_t i = 0; i < __OPTION_COUNT; ++i)
    {
        if (strcmp(**str_ptr_ptr, flag_option_arr[i]))
        {
            continue;
        }

        used_option[i] = true;
        CHECK_FUNC(set_option_arr[i], config_ptr, str_ptr_ptr, end_str);

        return 0;
    }

    return UNKNOWN_OPTION;
}

errno_t Config_Ctor(Config *const config_ptr, size_t const argc, char const *const *const argv)
{
    assert(config_ptr); assert(!config_ptr->is_valid); assert(argc > 0); assert(argv);

    char const *const end_str = argv[argc];
    bool used_options[__OPTION_COUNT] = {};
    for (char const *const *str_ptr = argv + 1; *str_ptr != end_str; ++str_ptr)
    {
        CHECK_FUNC(select_option_setter, config_ptr, &str_ptr, end_str, used_options);
    }

    config_ptr->is_valid = true;
    return 0;
}

void Config_Dtor(Config *const config_ptr)
{
    assert(config_ptr);

    config_ptr->is_valid = false;
}
