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

struct opt_globals {
    char* optarg;
    int optind;
    int optopt;
    int opterr;
    int optreset;
};

void init_globals(struct opt_globals* globals) {
    globals->optarg = "";
    globals->optind = 1;
    globals->optopt = '\0';
    globals->opterr = 1;
    globals->optreset = 1;
}

void push_globals(struct opt_globals* globals) {
    globals->optarg = optarg;
    globals->optind = optind;
    globals->optopt = optopt;
    globals->opterr = opterr;
    globals->optreset = optreset;
}

void pop_globals(struct opt_globals* globals) {
    optarg = globals->optarg;
    optind = globals->optind;
    optopt = globals->optopt;
    opterr = globals->opterr;
    optreset = globals->optreset;
}

int main(int argc, char const* _argv[]) {
    struct string_list args;
    init_string_list(&args, 2);

    // ロングオプションの戻り値とショートオプションの戻り値が等しい場合
    // (同一のオプション文字を指定した場合)
    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}};
    const char* optstring = "h";

    // パース前のグローバル変数を取得・保持しておく
    struct opt_globals globals;
    init_globals(&globals);
    pop_globals(&globals);

    char** argv;
    int result;
    int longindex;

    // 1. ロングオプションを渡した場合
    longindex = -1;
    optreset = 1;
    push_globals(&globals);
    set_string_list(&args, 1, "--help");
    argv = char_repr_string_list(&args);
    result = getopt_long((int)args.count, argv, optstring, options, &longindex);

    assert(result == 'h');   // この 'h' は、struct option.valの値
    assert(longindex == 0);  // options[0]にマッチしたので、longindexが更新される

    free(argv);
    pop_globals(&globals);

    // 2. ショートオプションを渡した場合
    longindex = -1;
    optreset = 1;
    push_globals(&globals);
    set_string_list(&args, 1, "-h");
    argv = char_repr_string_list(&args);
    result = getopt_long((int)args.count, argv, optstring, options, &longindex);

    assert(result == 'h');    // この 'h' は、optstring内の値
    assert(longindex == -1);  // ロングオプションとはマッチしなかったので、longindexは更新されない

    free(argv);
    pop_globals(&globals);

    // 3. 両方渡した場合
    longindex = -1;
    optreset = 1;
    push_globals(&globals);
    set_string_list(&args, 1, "-h");
    ADD_ARGUMENT(&args, "--help");
    argv = char_repr_string_list(&args);

    result = getopt_long((int)args.count, argv, optstring, options, &longindex);
    assert(result == 'h');    // この 'h' は、optstring内の値
    assert(longindex == -1);  // ロングオプションとはマッチしなかったので、longindexは更新されない

    result = getopt_long((int)args.count, argv, optstring, options, &longindex);
    assert(result == 'h');   // この 'h' は、struct option.valの値
    assert(longindex == 0);  // options[0]にマッチしたので、longindexが更新される

    free(argv);
    pop_globals(&globals);

    free_string_list(&args);
    return 0;
}