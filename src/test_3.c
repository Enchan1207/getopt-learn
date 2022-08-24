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

    // オプションより前にオペランドがくる場合
    const char* optstring = "i:o::v";
    ADD_ARGUMENT(&args, "operand_1");
    ADD_ARGUMENT(&args, "operand_2");
    ADD_ARGUMENT(&args, "operand_3");
    ADD_ARGUMENT(&args, "-i");
    ADD_ARGUMENT(&args, "option_argument_i");
    ADD_ARGUMENT(&args, "-o");
    ADD_ARGUMENT(&args, "option_argument_o");
    ADD_ARGUMENT(&args, "-v");
    char** argv = char_repr_string_list(&args);

    // パースは一瞬で終わる (環境依存 argvを並べ替える実装もある…らしい)
    int result = getopt((int)args.count, argv, optstring);
    assert(result == -1);
    assert(optind == 1);  // 0はprog_name

    free(argv);
    free_string_list(&args);
    return 0;
}