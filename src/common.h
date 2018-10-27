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

#ifndef CONSOLEAPP_COMMON_H
#define CONSOLEAPP_COMMON_H

#ifndef isNull
#define isNull(p)    ((p) == NULL)
#endif

#ifndef isNotNull
#define isNotNull(p) ((p) != NULL)
#endif

#define CONAPP_DEFAULT_ERRNO      -1
#define CONAPP_LOGIC_ERRNO_BASE   50
#define CONAPP_RUNTIME_ERRNO_BASE 100

typedef enum{
    CONAPP_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH,
}conapp_bugcode_t;

void
_conappBugReport(
        conapp_bugcode_t errno);

#endif
