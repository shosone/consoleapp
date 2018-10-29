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

#ifndef OPTION_ERRMSG_H
#define OPTION_ERRMSG_H

#ifdef _WIN32
#include "./win32.h"
#endif

#include "./common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* An error message will be forcibly displayed on the console when the following error occurs. */
typedef enum{
    OPTION_SHORT_FORM_IS_NULL = CONSOLEAPP_LOGIC_ERRNO_BASE,
    OPTION_MIN_BIGGER_THAN_MAX, 
    OPTION_PROP_GP_IS_NULL, 
    OPTION_SAME_PRIORITY,
    OPTION_SAME_SHORT_LONG_FORMAT,
}option_logic_errno_t;

/* When the API of liboption.a returns OPTION_FAILURE, the following error occurs.
 * Then, gloval valiable option_errno has been set.
 * And you can earn the string of error message by optionErrno2msg(option_errno). */
typedef enum{
    OPTION_DUPLICATE_SAME_OPT = CONSOLEAPP_RUNTIME_ERRNO_BASE,
    OPTION_TOO_MANY_CONTENTS, 
    OPTION_TOO_LITTLE_CONTENTS, 
}option_runtime_errno_t;

extern int option_errno;

extern char*
optionErrno2msg(
         int errno);

#ifdef __cplusplus
}
#endif
#endif 
