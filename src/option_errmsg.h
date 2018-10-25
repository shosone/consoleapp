/* MIT License
 * 
 * Copyright (c) 2018 Sho Sone
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 */

#ifndef CONSOLEAPP_ERRMSG_H
#define CONSOLEAPP_ERRMSG_H

#ifdef OPTION_AUTOMSG

#include <stdio.h>

#define printUsrErrMsg(errcode, short_form, long_form)\
    fprintf(stderr, option_usr_errmsg[errcode], short_form, isNull(long_form) ? " \b\b" : long_form);\
    fprintf(stderr, "\n")

#define printProgramerErrMsg(errcode)\
    fprintf(stderr, option_programer_errmsg[errcode],  __func__);\
    fprintf(stderr, "\n")

#define printDeveloperErrMsg(errcode)\
    fprintf(stderr, option_developer_errmsg[errcode], "");\
    fprintf(stderr, "\n");\
    fprintf(stderr, " version: %s\n", CONSOLEAPP_VERSION);\
    fprintf(stderr, " file: %s\n", __FILE__);\
    fprintf(stderr, " function: %s\n", __func__);\
    fprintf(stderr, " line no: %d\n", __LINE__);\
    fprintf(stderr, " please give us a bug info. (https://github.com/shosone/consoleapp)")

/* use printUsrErrMsg(errcode, option_or_content_name) */
typedef enum{
    OPTION_DUPLICATE_SAME_OPT, 
    OPTION_TOO_MANY_CONTENTS, 
    OPTION_TOO_LITTLE_CONTENTS, 
}option_usr_errcode_t;

/* use printProgramerErrMsg(errcode) */
typedef enum{
    OPTION_SHORT_FORM_IS_NULL,
    OPTION_MIN_BIGGER_THAN_MAX, 
    OPTION_PROP_GP_IS_NULL, 
    OPTION_SAME_PRIORITY,
    OPTION_SAME_SHORT_LONG_FORMAT,
    OPTION_PRIORITY_IS_OPTION_SUCCESS,
}option_programer_errcode_t;

/* use printDeveloperErrMsg(errcode) */
typedef enum{
    OPTION_OUT_OF_MEMORY, 
    OPTION_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH,
}option_subroutine_errcode_t;

static const char *option_usr_errmsg[] = {
    "option error: duplicate same option %s(%s).",
    "option error: the number of contents of option %s(%s) is too many.",
    "option error: the number of contents of option %s(%s) is too little.",
};

static const char *option_programer_errmsg[] = {
    "API usage error (%s@libconsoleapp.a): opt_property_t\'s field short_form cannot be NULL. please check 2nd argument of regOptProperty().",
    "API usage error (%s@libconsoleapp.a): sopt_property_t\'s filed content_num_min bigger than content_num_max. please check 4th and 5th argument of regOptProperty().",
    "API usage error (%s@libconsoleapp.a): option property information have not registerd.",
    "API usage error (%s@libconsoleapp.a): sopt_property_t\'s field priority must be different from the priority of other properties.",
    "API usage error (%s@libconsoleapp.a): opt_property_t\'s field short_form and long_form must be different from the priority of other properties.",
    "API usage error (%s@libconsoleapp.a): opt_property_t\'s field priority must note be OPTION_SUCCESS.",
};

static const char *option_developer_errmsg[] = {
    "there may be a bug in libconsoleapp.a (;_;): out of memory occurred.",
    "there may be a bug in libconsoleapp.a (;_;): unexpected constant value in switch statement.",
};

#else

#define printUsrErrMsg(errcode, short_form, long_form)
#define printProgramerErrMsg(errcode)
#define printDeveloperErrMsg(errcode)

#endif /* OPTION_AUTOMSG */
#endif /* CONSOLEAPP_ERR_MSG_H */
