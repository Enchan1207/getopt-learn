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
    // 実行引数のエミュレート
    struct string_list args;
    init_string_list(&args, 0);
    ADD_ARGUMENT(&args, _argv[0]);

    // 引数をオプショナルで要求するオプションに対し、オプションのみを渡した場合
    const char* optstring = "o::";
    ADD_ARGUMENT(&args, "-o");
    char** argv = char_repr_string_list(&args);

    // パース結果は'?'となるが…
    int result = getopt((int)args.count, argv, optstring);
    assert((char)result == '?');

    // グローバル変数optoptに発見したオプションが格納される
    assert(optopt == 'o');

    free(argv);
    free_string_list(&args);
    return 0;
}