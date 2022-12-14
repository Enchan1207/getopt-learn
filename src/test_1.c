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

    // 全ての引数に値が順番通り入った状態
    const char* optstring = "i:o::v";
    ADD_ARGUMENT(&args, "-i");
    ADD_ARGUMENT(&args, "option_argument_i");
    ADD_ARGUMENT(&args, "-o");
    ADD_ARGUMENT(&args, "option_argument_o");
    ADD_ARGUMENT(&args, "-v");
    char** argv = char_repr_string_list(&args);

    int result = getopt((int)args.count, argv, optstring);
    unsigned int lcnt = 0;
    while (result > 0) {
        printf("getopt #%d return:%c(%d), optind:%d, optopt:%c(%d), optarg:%s\n",
               lcnt, result, result, optind, optopt, optopt, optarg);

        // 戻り値 = 見つかったオプション
        char optchar = (char)result;

        // グローバル変数optoptも同じ値になる
        assert(optchar == optopt);

        switch (optchar) {
            // オプションに引数が渡った場合、引数の値は optarg から読み出せる
            case 'i':
                assert(strcmp(optarg, "option_argument_i") == 0);
                break;

            case 'o':
                assert(strcmp(optarg, "option_argument_o") == 0);
                break;

            case 'v':
                puts("verbose flag!");
                assert(optarg == NULL);
                break;

            // 他のパターンはありえない
            case '?':
                puts("UNEXPECTED");
                FATAL_ERROR;

            default:
                puts("UNEXPECTED");
                FATAL_ERROR;
        }

        result = getopt((int)args.count, argv, optstring);
        lcnt++;
    }

    // パースが終了すると、getoptの戻り値は-1となる
    assert(result == -1);

    // パース終了時のoptindはargcと等しい
    // (0を飛ばして読みはじめ、argc-1まで読んでパースが終了し、その次にインデックスが進んだところで止まるため)
    assert(optind == args.count);

    free(argv);
    free_string_list(&args);
    return 0;
}