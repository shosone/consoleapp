#include <stdio.h>
#include "./common.h"

static const char *conapp_bug_report_msg[] = {
    "there may be a bug in liboption.a (;_;): unexpected constant value in switch statement.",
};

void
_conappBugReport(
    conapp_bugcode_t errno)
{
    fprintf(stderr, conapp_bug_report_msg[(errno)], NULL);
    fprintf(stderr, "\n");
}

