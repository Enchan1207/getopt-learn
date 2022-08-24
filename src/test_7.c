//
//
//
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinystr.h>

#define ADD_ARGUMENT(list, arg) add_string_list(list, arg)

#define FATAL_ERROR exit(EXIT_FAILURE)

int main(int argc, char const* _argv[]) {
    struct string_list args;
    init_string_list(&args, 0);
    ADD_ARGUMENT(&args, _argv[0]);

    // オプションを管理する構造体配列
    int help_flag = 0;
    struct option options[] = {
        {"help", no_argument, &help_flag, 'h'}};

    //
    // struct optionの構造は以下の通り

    // - const char* name
    //      ロングオプションの名称. 1文字のオプションはこちらに記述しなくてもよい (むしろ、optstringと重複するのはよくなさそう)
    // - int has_arg
    //      オプションが引数を要求するかどうか. no_argument, required_argument, optional_argument の定数が用意されている.
    // - int* flag
    //      このメンバがNULLでないとき、getopt_long の戻り値は0となり、本来の戻り値はこちらに格納される。
    // - int val
    //      このオプションが指定されたとき、getopt_longの戻り値となる値。
    //

    // 短いオプション
    const char* optstring = "h";

    // 実行引数のエミュレート
    ADD_ARGUMENT(&args, "--help");
    char** argv = char_repr_string_list(&args);

    // パース
    int result = getopt_long((int)args.count, argv, optstring, options, NULL);
    assert(result == 0);       // helpオプションはフラグ変数を持っているので、関数の戻り値は0
    assert(help_flag == 'h');  // フラグ変数にオプションのメンバで指定した値が返る

    // optindを戻してもう一度パース
    set_string_list(&args, 1, "-h");  // ショートオプションとして渡った想定
    optind = 1;
    help_flag = '\0';

    result = getopt_long((int)args.count, argv, optstring, options, NULL);
    assert(result == 'h');      // ショートオプションで渡されているので、戻り値はそのままオプション名
    assert(help_flag == '\0');  // フラグ変数は更新されない

    free(argv);
    free_string_list(&args);
    return 0;
}