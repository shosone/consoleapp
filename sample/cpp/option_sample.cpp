#include "../../src/option.h"
#include <iostream>
#include <cctype>
#include <algorithm>

using namespace std;

/* option */
enum {
    /* high priority */
    VERSION,
    HELP,
    REAL,
    OPERATION,
    BASE,
    /* low priority */
};

/* usr definition err */
enum {
    /* high priority */
    INVALID_OPERATION = 500,
    NAN,
    /* low priority */
};

/* operations */
enum {
    ADD = 1,
    SUB,
    MUL,
    DIV,
};

void printHelp(void);
void printVersion(void);
int operationChecker(char **contents, int content_num);
int baseChecker(char **contents, int content_num);

int main(int argc, char *argv[]){

    double (*operation)(double, double) = [](double a, double b){return a + b;};
    double (*str2num)(const char *)     = [](const char *str){return (double)atoi(str);};
    int    (*isnumstr)(int)             = isdigit;
    int    ret            = OPTION_SUCCESS;
    double sum            = 0;
    int    optless_num    = 0;
    char **optless        = NULL;

    /* registering option properties */
    /*             priority   format1      format2        min num of contents  max..  contents checker*/
    regOptProperty(HELP,      "-h",        "--help",      0,                   0,     NULL);
    regOptProperty(VERSION,   "-v",        "--version",   0,                   0,     NULL);
    regOptProperty(REAL,      "--as-real", "hogera",      0,                   0,     NULL);
    regOptProperty(OPERATION, "-o",        "--operation", 1,                   1,     operationChecker);
    regOptProperty(BASE,      "-b",        "--base",      1,                   1,     baseChecker);

    /* When the API of liboption.a returns OPTION_FAILURE, the following error occurs.
     *
     *     typedef enum{
     *         OPTION_SHORT_FORM_IS_NULL = CONSOLEAPP_LOGIC_ERRNO_BASE,
     *         OPTION_MIN_BIGGER_THAN_MAX, 
     *         OPTION_PROP_GP_IS_NULL, 
     *         OPTION_SAME_PRIORITY,
     *         OPTION_SAME_SHORT_LONG_FORMAT,
     *     }option_logic_errno_t;
     *
     * Then, gloval valiable option_errno has been set.
     * And you can earn the string of error message by optionErrno2msg(option_errno).  */
    if(groupingOpt(argc, argv, &optless_num, &optless) == OPTION_FAILURE){
        cerr << "option error: " << optionErrno2msg(option_errno) << endl;
        ret = option_errno;
        goto free_and_exit;
    }


    /* In liboption.a, write down all the results of functions of content checker
     * and sort the results in descending order of priority.
     * Then you can retrieve the result one by one with popOptErrcode().
     * And popOptErrcode() finally return OPTION_SUCCESS to inform you that all the results have been retrieved. */
    {
        bool exit_flag = false;
        for(int err = popOptErrcode(); err != OPTION_SUCCESS; err = popOptErrcode()){
            switch(err){
                case INVALID_OPERATION:
                    cerr << "option error: invalid operation specified with -o(--operation)" << endl;
                    ret = INVALID_OPERATION;
                    exit_flag = true;
                    break;

                case NAN:
                    cerr << "option error: not a number specified with -b(--base)" << endl;
                    ret = NAN;
                    exit_flag = true;
                    break;

                case ADD:
                    operation = [](double a, double b){return a + b;};
                    break;

                case SUB:
                    operation = [](double a, double b){return a - b;};
                    break;

                case MUL:
                    operation = [](double a, double b){return a * b;};
                    break;

                case DIV:
                    operation = [](double a, double b){return a / b;};
                    break;
            }
        }
        if(exit_flag) goto free_and_exit;
    }

    /* Now, it was guaranteed that there was no error in options, but be careful that it is "in options".
     * the variable optless may contain errors. 
     * However, in many applications options other than -h and -v will be options to coordinate control over the entire program.
     * So at this timing you should be able to create the following switch block. 
     * The option groups retrieved by popOptGroup() comes from the option with higher priority registered with regOptProperty().
     * Although it is complicated, the smaller the value, the higher the priority. */
    for(opt_group_t *grp_p = popOptGroup(); grp_p != NULL; grp_p = popOptGroup()){
        switch(grp_p -> priority){
            case HELP:
                printHelp();
                break;

            case VERSION:
                printVersion();
                break;

            case REAL:
                isnumstr = [](int ch){return (int)(isdigit(ch) || ch == '.');};
                str2num  = atof;
                break;

            case OPERATION:
                /* nothing to do in this sample.
                 * because operationChecker() and above block does all works. */
                break;

            case BASE:
                sum = atoi(grp_p -> contents[0]);
                break;
        }
    }

    /* error check of optless */
    for(int i=0; i<optless_num; i++){
        string str = string(optless[i]);
        for(unsigned int j=0; j<str.length(); j++){
            if(!isnumstr(str[j])){
                cerr << "err: not a number" << endl;
                ret = NAN;
                goto free_and_exit;
            }
        }
    }

    if(optless_num > 0){
        for(int i=0; i<optless_num; i++){
            sum = operation(sum, str2num(optless[i]));
        }
        cout << "result: " << sum << endl;
    }

    return 0;

free_and_exit:

    /* release dynamic memory reserved by liboption.a */
    endOptAnalization();
    return ret;
}

void printHelp(void){
    cout << R"(Description: calculate the sum of the enterd numerical values.
Usage: sample_option
    -h           print this help
    -v           print version of liboption.a
    -o <a|s|m|d> specifying operation
    -b <num>     set initial value of formula
    --as-real    treat inputed numbers as real numbers

[othere format]
    --help     same as -h
    --version  same as -v
    --base     same as -b
    --operaton same as -o
    )" << endl;
}

void printVersion(void){
    cout << "liboption.a v" << CONSOLEAPP_OPTION_VERSION << endl;
    cout << R"(Copyright (c) This software is provided under the MIT license.
See "https://opensource.org/licenses/MIT" for details.
    )" << endl;
}

int operationChecker(char **contents, int dontcare){
    string str = string(contents[0]);
    if(str == "a"){
        return ADD;
    }
    else if(str == "s"){
        return SUB;
    }
    else if(str == "m"){
        return MUL;
    }
    else if(str == "d"){
        return DIV;
    }else{
        return INVALID_OPERATION;
    }
}

int baseChecker(char **contents, int dontcare){
    for(unsigned int i=0; i<string(contents[0]).length(); i++){
        if(!isdigit(contents[0][i])){
            return NAN;
        }
    }
    return OPTION_SUCCESS;
}
