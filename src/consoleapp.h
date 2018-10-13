#ifndef CONSOLE_APP_H
#define CONSOLE_APP_H

#include "option.h"
#include "prompt.h"

#ifndef BUG_REPORT
#include <stdio.h>
#define BUG_REPORT() (fprintf(stderr, "error: there is a bug! (%s, %s, %d)", __FILE__, __FUNCTION__, __LINE__))
#endif

#endif
