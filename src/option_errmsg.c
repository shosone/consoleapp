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

static const char *_option_api_usage_errmsg[] = {
    "API usage error (%s@libconsoleapp.a): opt_property_t\'s field short_form cannot be NULL. please check 2nd argument of regOptProperty().",
    "API usage error (%s@libconsoleapp.a): opt_property_t\'s filed content_num_min bigger than content_num_max. please check 4th and 5th argument of regOptProperty().",
    "API usage error (%s@libconsoleapp.a): option property information have not registerd.",
    "API usage error (%s@libconsoleapp.a): opt_property_t\'s field priority must be different from each other.",
    "API usage error (%s@libconsoleapp.a): opt_property_t\'s field short_form and long_form must be different from each other.",
    "API usage error (%s@libconsoleapp.a): opt_property_t\'s field priority must note be OPTION_SUCCESS.",
};

static const char *_option_end_usr_errmsg[] = {
    "duplicate same option %s(%s).",
    "the number of contents of option %s(%s) is too many.",
    "the number of contents of option %s(%s) is too little.",
};

char *option_errmsg = NULL;
int   option_errno  = CONAPP_DEFAULT_ERRNO;

void
__printAPIusageErrMsg(
        option_api_usage_errcode_t errno,
        const char*                func_name)
{
    fprintf(stderr, _option_api_usage_errmsg[errno],  func_name);
    fprintf(stderr, "\n");
}

void
_makeEndUsrErrMsg(
        option_end_usr_errcode_t errno,
        char *short_form,
        char *long_form)
{
    option_errno = errno;
    free(option_errmsg);
    option_errmsg = malloc(strlen(_option_end_usr_errmsg[errno]) + strlen(short_form) + strlen(long_form == NULL ? "" : long_form));
    sprintf(option_errmsg, _option_end_usr_errmsg[errno], short_form, long_form == NULL ? " \b\b" : long_form);
}
