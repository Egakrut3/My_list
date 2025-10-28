#ifndef OPTION_MANAGER_H
#define OPTION_MANAGER_H

#include "Common.h"

#define UNKNOWN_OPTION              1'000
#define NOT_ENOUGH_OPTION_ARGUMENTS 1'001

struct Config
{
    bool is_help;

    bool is_valid;
};

errno_t Config_Ctor(Config *config_ptr, size_t argc, char const *const *argv);

void Config_Dtor(Config *config_ptr);

#endif
