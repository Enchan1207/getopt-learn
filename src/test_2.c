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
    // + オペランドを含む
    const char* optstring = "i:o::v";
    ADD_ARGUMENT(&args, "-i");
    ADD_ARGUMENT(&args, "option_argument_i");
    ADD_ARGUMENT(&args, "-o");
    ADD_ARGUMENT(&args, "option_argument_o");
    ADD_ARGUMENT(&args, "-v");
    ADD_ARGUMENT(&args, "operand_1");
    ADD_ARGUMENT(&args, "operand_2");
    ADD_ARGUMENT(&args, "operand_3");
    char** argv = char_repr_string_list(&args);

    int result = getopt((int)args.count, argv, optstring);
    while (result > 0) {
        // 戻り値 = 見つかったオプション
        char optchar = (char)result;
        printf("opt: %c\n", optchar);

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
    }

    // パースが終了すると、getoptの戻り値は-1となる
    assert(result == -1);

    printf("optind:%d\n", optind);

    // オペランドは順番に入る
    for (int i = optind; i < args.count; i++) {
        char buf[32];
        sprintf(buf, "operand_%d", i - optind + 1);

        assert(strcmp(argv[i], buf) == 0);
    }

    free(argv);
    free_string_list(&args);
    return 0;
}