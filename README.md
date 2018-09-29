# consoleapp
Consoleapp is c library for console applications.

## consoleapp/option
Functions that check error of option of console application and grouping contents attached to option are summarized.
### dependancy
T.B.D.
### struct reference
```c
/* プログラムで使用できるオプションの情報を保持する構造体 */
typedef struct _sFLAG_PROPERY{
    char *short_form;                                           /* オプションの短縮形式. 例えば"-v" */
    char *long_form;                                            /* オプションの詳細形式. 例えば"--version" */
    int  (*contents_checker)(char **contents, int content_num); /* オプションに付属するcontentsの正しさを調べるコールバック関数 */
    int  content_num_min;                                       /* オプションに付属するcontentsの最小数 */
    int  content_num_max;                                       /* オプションに付属するcontentsの最大数 */
    int  appeared_yet;                                          /* 同じオプションがすでに指定されたかチェックするためのメモとして用いる */
}sFLAG_PROPERY;
```
```c
/* sFLAG_PROPERYのエントリを保持するための構造体 */
typedef struct _sFLAG_PROPERY_DB{
    int            prop_num; /* sFLAG_PROPERYの配列のサイズ */
    sFLAG_PROPERY *props;    /* sFLAG_PROPERYの配列 */
}sFLAG_PROPERY_DB;
```
```c
/* プログラム実行時に指定した各オプションの情報を保持するための構造体 */
typedef struct _sOPT_GROUP{
    char  *flag;          /* 例えば "gcc -Wall -O3 -I ./hoge ./geho -o ./foo bar1.c bar2.c" の -Wall, -O3, -I, -o がそれぞれ別のsOPT_GROUPのフラグに割り当てられる */
    int    content_num;   /* 例えば flag が -I なら2になる */
    char **contents;      /* 例えば flag が -I なら ./hoge, ./geho になる */
    int    err_code;      /* 対応するsFLAG_PROPERYに登録されているcontents_checker関数の引数にcontent_numとcontentsを適用した際の戻り値  */
}sOPT_GROUP;
```
```c
/* sOPT_GROUPのエントリを保持するための構造体 */
typedef struct _sOPT_GROUP_DB{
    int         grp_num;      /* sOPT_GROUPの配列のサイズ */
    sOPT_GROUP *grps;         /* sOPT_GROUPの配列 */
    int         flagless_num; /* 対応するオプションが無いコンテンツの数 */
    char      **flagless;     /* 対応するオプションが無いコンテンツ. 例えば, gcc -o hoge hoge.c geho.c のhoge.cとgeho.c */
}sOPT_GROUP_DB;
```
### function reference
```c
extern sFLAG_PROPERY_DB* /* 生成されたsFLAG_PROPERY_DBのメモリ領域のポインタ */
genFlagPropDB(     
        int prop_num); /* 登録するsFLAG_PROPERYの数 */
```
```c
extern int /* 0:success, 1:short_formがNULL, 2:out of memory, 3:content_num_minがcontent_num_maxより大きい */
addFlagProp2DB( /* sFLAG_PROPERY_DBのエントリを追加する関数 */
        sFLAG_PROPERY_DB *db,             /* [out] 登録先(genFlagPropDBで作成したsFLAG_PROPERY_DB) */
        char             *short_form,     /* [in] オプションの短縮形式 */
        char             *long_form,      /* [in] オプションの詳細形式 */
        int              content_num_min, /* オプションに付属するコンテンツの最少数 */
        int              content_num_max, /* オプションに付属するコンテンツの最大数 */
        int             (*contents_checker)(char **contents, int content_num)); /* オプションのコンテンツをチェックするコールバック関数 */
```
```c
extern void
freeFlagPropDB( /* sFLAG_PROPERY_DBのメンバのメモリ領域を再帰的に開放する関数 */
        sFLAG_PROPERY_DB *db); /* [in] 開放するsFLAG_PROPERY_DB */
```
```c
extern int /* 0:success, 1:out of memory, 2:flag_prop_db is null, 3:duplicate same flag, 4:too many contents, 5:too little contents */
groupingOpt( /* オプション情報が登録されたsFLAG_PROPERY_DBをもとにmainの引数で取得したargcとargvをグループに分類してsOPT_GROUP_DBのエントリに登録する関数 */
        sFLAG_PROPERY_DB *flag_prop_db, /* [in] オプション情報が登録されたsFLAG_PROPERY_DB */
        int               argc,         /* mainの引数で受け取ったプログラムの引数の数(プログラム名含む) */
        char            **argv,         /* [in] mainの引数で受け取ったプログラムの引数(プログラム名含む) */
        sOPT_GROUP_DB   **opt_grp_db);  /* [out] グルーピングされたオプション情報 */
```
```c
extern void
freeOptGroupDB( /* sOPT_GROUP_DBのメンバのメモリ領域を再帰的に解放 */
        sOPT_GROUP_DB *opt_group_db); /* [in] 開放するsOPT_GROUP_DB */
```
### sample code
This it a part of "sample/sample.c".
```c
int main(int argc, char *argv[]){

    sFLAG_PROPERY_DB *flag_prop_db = genFlagPropDB(4);
    sOPT_GROUP_DB    *opt_grp_db   = NULL;
    int               groupingOpt_ret;

    regFlagProp(flag_prop_db);
    groupingOpt_ret = groupingOpt(flag_prop_db, argc, argv, &opt_grp_db);

    switch(groupingOpt_ret){
        case 0: /* success */
            break;

        case 1:
            fprintf(stderr, "error: out of memory\n");
            exit(1);

        case 2:
            fprintf(stderr, "error: flag_prop_db is null\n");
            exit(1);

        case 3:
            fprintf(stderr, "error: duplicate same flag\n");
            exit(1);

        case 4:
            fprintf(stderr, "error: too many contents\n");
            exit(1);

        case 5:
            fprintf(stderr, "error: too little contents\n");
            exit(1);

        default:
            fprintf(stderr, "there is a bug! (line: %d)\n", __LINE__);
            exit(100);
    }

    for(int i=0; i<opt_grp_db->grp_num; i++){
        switch(opt_grp_db->grps[i].err_code){
            case 0: /* success */
                break;

            case 1: 
                fprintf(stderr, "error: the history size \"%s\" specified with the option \"%s\" is an invalid value\n", opt_grp_db->grps[i].contents[0], opt_grp_db->grps[i].flag);
                exit(2);
                break;

            default:
                fprintf(stderr, "error: there is a bug! (line: %d)\n", __LINE__);
                exit(100);
        }
    }

    for(int i=0;i<opt_grp_db->grp_num;i++){
        char *flag       = opt_grp_db -> grps[i].flag;
        char **contents  = opt_grp_db -> grps[i].contents;
        int  content_num = opt_grp_db -> grps[i].content_num;

        if(strcmp(flag, "-h") == 0 || strcmp(flag, "--help") == 0){
            printUsage();
        }
        else if(strcmp(flag, "-v") == 0 || strcmp(flag, "--version") == 0){
            printVersion();
        }
        else if(strcmp(flag, "-p") == 0 || strcmp(flag, "--print") == 0){
            for(int i=0; i<content_num; i++){
                printf("%s\n", contents[i]);
            }
        }
        else if(strcmp(flag, "-i") == 0 || strcmp(flag, "--interactive") == 0){
            interactive(atoi(contents[0]));
        }
    }

    return 0;
}
```

This is a part of "sample/for_option.c".
```c
int chkOptInteractive(char **contents, int dont_care){
    const int SUCCESS        = 0;
    const int ILLIGAL_NUMBER = 1;

    int num = atoi(contents[0]);
    if(num < 1){
        return ILLIGAL_NUMBER;
    }
    return SUCCESS;
}

void regFlagProp(sFLAG_PROPERY_DB *flag_prop_db){
    addFlagProp2DB(flag_prop_db, "-h", "--help",        0,       0, NULL);
    addFlagProp2DB(flag_prop_db, "-v", "--version",     0,       0, NULL);
    addFlagProp2DB(flag_prop_db, "-p", "--print",       1, INT_MAX, NULL);
    addFlagProp2DB(flag_prop_db, "-i", "--interactive", 1,       1, chkOptInteractive);
}
```
### demo
![option_demo](doc/option_demo.gif)

## consoleapp/interactive
Functions that facilitate the implementation of interactive functions in the console application are summarized.
### dependancy
T.B.D.
### struct reference
T.B.D.
### function reference
T.B.D.
### sample code

## installation
Please read Makefile. Introduction of autotools is under consideration.

![installation](doc/installation.gif)

## contents
<pre>
.
├── Makefile
├── README.md
├── doc
│   ├── AUTHORS.md
│   ├── coding_rule.md
│   └── todo.md
├── sample
│   ├── for_option.c
│   ├── sample
│   └── sample.c
└── src
    ├── interactive.c
    ├── interactive.h
    ├── option.c
    └── option.h

3 directories, 12 files
</pre>
