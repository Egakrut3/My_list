/*! \file */

#ifndef COLORED_PRINTF_H
#define COLORED_PRINTF_H

/*!
 *Contains colors than can be used to write in concole
 */
enum Color {
    BLACK,   ///<Black color
    RED,     ///<Red color
    GREEN,   ///<Green color
    YELLOW,  ///<Yellow color
    BLUE,    ///<Blue color
    MAGENTA, ///<Magenta color
    CYAN,    ///<Cyan color
    WHITE,   ///<White color
};

int colored_printf(enum Color foreground_color, enum Color background_color,
                   char const *format, ...);

int colored_error_printf(enum Color foreground_color, enum Color background_color,
                         char const *format, ...);

#endif
