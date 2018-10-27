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

typedef enum{
    OPTION_SHORT_FORM_IS_NULL,
    OPTION_MIN_BIGGER_THAN_MAX, 
    OPTION_PROP_GP_IS_NULL, 
    OPTION_SAME_PRIORITY,
    OPTION_SAME_SHORT_LONG_FORMAT,
}option_api_usage_errcode_t;

typedef enum{
    OPTION_DUPLICATE_SAME_OPT,
    OPTION_TOO_MANY_CONTENTS, 
    OPTION_TOO_LITTLE_CONTENTS, 
}option_end_usr_errcode_t;

extern char *option_errmsg;
extern int   option_errno;

#define _printAPIusageErrMsg(errno) __printAPIusageErrMsg(errno, __func__)

/* use _printAPIusageErrMsg macro instead of __printAPIusageErrMsg */
extern void
__printAPIusageErrMsg(
        option_api_usage_errcode_t errcode,
        const char*                func_name);

extern void
_makeEndUsrErrMsg(
        option_end_usr_errcode_t errcode,
        char *short_form,
        char *long_form);

#endif 
