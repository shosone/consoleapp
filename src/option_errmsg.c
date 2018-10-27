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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./option_errmsg.h"

/* ============== static global variables ============== */

static const char *_option_api_usage_errmsg[] = {
    "API usage error (%s@libconsoleapp.a): the 2nd argument of regOptProperty() cannot be NULL. please set a string of short form of option",
    "API usage error (%s@libconsoleapp.a): in regOptProperty(), the 4th argument content_num_min bigger than 5th argument content_num_max.",
    "API usage error (%s@libconsoleapp.a): option properties have not been registerd yet.",
    "API usage error (%s@libconsoleapp.a): priorities of option properties registerd by regOptProperty() must be different constant value from each other.",
    "API usage error (%s@libconsoleapp.a): short_form and long_form registerd by regOptProperty() must be different from each other.",
};

static const char *_option_end_usr_errmsg[] = {
    "duplicate same option %s(%s).",
    "the number of contents of option %s(%s) is too many.",
    "the number of contents of option %s(%s) is too little.",
};

static char *_option_errcause1 = NULL;
static char *_option_errcause2 = NULL;

/* ============== publish only in option.c ============== */

void
_printAPIusageErrMsg(
        option_runtime_errno_t errno,
        const char*                func_name)
{
    option_errno = errno;
    fprintf(stderr, _option_api_usage_errmsg[errno],  func_name);
    fprintf(stderr, "\n");
}

void
_makeEndUsrErrMsg(
        option_runtime_errno_t errno,
        char *short_form,
        char *long_form)
{
    option_errno      = errno;
    _option_errcause1 = short_form;
    _option_errcause2 = long_form;
}

/* ============== publications ============== */

int option_errno = CONAPP_DEFAULT_ERRNO;

char*
optionErrno2msg(
        option_runtime_errno_t errno)
{
    static char *errmsg = NULL;
    free(errmsg);

    errno -= CONAPP_RUNTIME_ERRNO_BASE;
    if(errno < 0 || errno >= 3){
        return "unknown error number.";
    }
    else{
        errmsg = malloc(strlen(_option_end_usr_errmsg[errno]) + strlen(_option_errcause1) + strlen(_option_errcause2 == NULL ? "" : _option_errcause2));
        sprintf(errmsg, _option_end_usr_errmsg[errno], _option_errcause1, _option_errcause2 == NULL ? " \b\b" : _option_errcause2); 
    }

    return errmsg;
}
