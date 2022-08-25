# getoptなんもわからん

Cでコマンドラインオプション(いわゆる`argv`)を解析しようとなると`getopt.h`の`getopt()`や`getopt_long()`を使いたくなるのですが、
如何せん動作がよく分からなかったので 自分でなんとなく調べてまとめました。

## 実行引数

Cにおける"最も単純なプログラム" として、このようなものがあります:

```C
#include <stdio.h>

int main(int argc, char const *argv[]) {
    printf("Hello, World!\n");
    return 0;
}
```

いわゆるHello,Worldという類のプログラム。  
コンパイル時に指定しない限りエントリポイントは`main`になる**はず**なので、コマンドを実行する際に渡した引数は `main` 関数の `int argc, char const *argv[]` に渡される**はず**です。
(本当によくわかっていない)

`argc`は実行引数の数、`argv`は実行引数の内容であり…

```C
#include <stdio.h>

int main(int argc, char const *argv[]) {
    printf("%d arguments passed.\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("#%d (at %p): %s\n", i, argv[i], argv[i]);
    }
    return 0;
}
```

こんな感じのプログラムを書いてやれば、

```
% ./test Hello 123 ABCDEF
4 arguments passed.
#0 (at 0x7ff7bd167368): ./test # 実行引数の0番目は、自分自身がどう呼ばれているか が格納される
#1 (at 0x7ff7bd16736f): Hello
#2 (at 0x7ff7bd167375): 123
#3 (at 0x7ff7bd167379): ABCDEF
```

こんな感じの出力が得られます。

`argc`、`argv`を受け取るようにすればあとはよしなにやってくれるので、プログラム実行時に外部から情報を受け取る簡素な手段といえます。  
これにちょっとしたルールを持たせ、どのような情報を渡している/渡されているのかを分かりやすくしたものが**コマンドラインオプション**です。

## コマンドラインオプション

コマンドラインオプションとは `ls -la` や `tail -n 5` など、ハイフンをprefixとして付加した実行引数のことです。

### POSIX (原典?)

UNIXの標準化を行なっている The Open Group によれば、このような引数の渡し方は *Utility Argument Syntax* と呼ばれ、構文は以下のように定義されています。

> 12.1 Utility Argument Syntax
> This section describes the argument syntax of the standard utilities and introduces terminology used throughout POSIX.1-2017 for describing the arguments processed by the utilities.
> 
> Within POSIX.1-2017, a special notation is used for describing the syntax of a utility's arguments. Unless otherwise noted, all utility descriptions use this notation, which is illustrated by this example (see XCU Simple Commands):
> 
> utility_name[-a][-b][-c option_argument]  
>     [-d|-e][-f[option_argument]][operand...]  

(引用: [Utility Conventions - opengroup.org](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html))

ハイフンとアルファベット1文字で構成される引数は **オプション** (option, または**フラグ**) と呼ばれ、その後に引数が続く場合 その引数は **オプション引数** (option argument) となります。  
最後のオプションおよびオプション引数に続けて渡された実行引数は **オペランド** (operand) として処理されます。  
このほかかなり厳しめのガイドラインが12.2項に列挙されており、全部きっちり守ろうとすると結構大変そうです。

…つまり、我々が日頃よく打ち込んでいる

```
gcc -g -o main main.c
```

のようなコマンドは、

 - コマンド `gcc` に対し
    - 実行引数として
        - `-g` を引数を取らないオプション=フラグとして渡し
        - `-o` を引数を取るオプションとして渡し
            - オプション引数として `main` を渡し
        - `main.c` をオペランドとして渡す

という構造になっていることがわかります。 `gcc main.c -o main` でも実行できるのはちょっと不思議ですね(パースはできると思いますが仕様的にはアウト?)。

### GNUによる拡張

先ほどの構文をよく見ると、**ハイフンが二つ続くオプション**(`--version` や `--help`など)が許されなさそうなことがわかります。  
またgccでよく使う `-Wall` などのオプションは、「`W`というオプションに対し`all`というオプション引数が渡っている」ように見えます。

しかし、実際はそうではありません。

`gcc --help` は問題なく動作しますし(gcc (GCC) 11.2.0で確認)、 `gcc -W all test.c` とすることはできません( `W` はオプション引数を取らない扱いのため、gccは `all` がオペランドとして渡されたと認識します)。  
これは一体なんなのか。

> This is not what POSIX specifies; it is a GNU extension.  
> これは POSIX が指定するものではありません。 これは GNU拡張です。

(引用: [GNU Coding Standards 4.8 Standards for Command Line Interfaces - gnu.org](https://www.gnu.org/prep/standards/standards.html#Command_002dLine-Interfaces))

…だそうです。よくわかりませんが、**GNU拡張**なんだそうです。  
まあ、時にはオプションのハイフンを知らず知らずのうちに増やしてしまったり減らしたりして初めて打ち間違いを知ることもあるのでしょう(意味不明)。

### 思ったよりフリーダムなオプション界隈

このほか、 **サブコマンド**という概念も存在します。例えば:

```
git commit -m "commit message"
```

というコマンドは

 - `git` というコマンドの
    - サブコマンド `commit` に対し
        - 引数を取るオプション `-m` を指定して
            - 引数に `"commit message"` を渡す

のように処理されます。 `git --commit` のような形式は認められていません。

はたまた `java` コマンドのように:

```
java -version
```

もっとフリーダムなのは `xcodebuild` でしょうか:

```
% xcodebuild --help
Usage: xcodebuild [-project <projectname>] [[-target <targetname>]...|-alltargets] [-configuration <configurationname>] [-arch <architecture>]... 

Options:
    -usage                                                   print brief usage
    ...
    -maximum-concurrent-test-device-destinations NUMBER      the maximum number of device destinations to test on concurrently
    -maximum-concurrent-test-simulator-destinations NUMBER   the maximum number of simulator destinations to test on concurrently
    -parallel-testing-enabled YES|NO                         overrides the per-target setting in the scheme
    ...
```

もはや**POSIXでもGNU拡張でもありません**。なんだこれは。

…**本当によくわからない**んですが、もしかするとここのあたりは割と寛容というかフリーダムなのかもしれません。  
結局コマンドごとにやりたいことは全く異なってきますし、毎度毎度POSIXやGNUに従って機能性を失っては元も子もないので(dockerやgitにサブコマンドがなかったらと思うとゾッとします)…

## 各言語のオプションパース処理

本題に入る前に3000文字以上書いてしまいもはや**なんの記事かわからなく**なってしまっていますが、もう少しだけ横道にそれます。  
大手のプログラミング言語がどのように実行引数のパースをやってくれるのか、あるいはそもそも**そんな処理自分で書けや**というスタンスなのかをちょっとだけ調べてみました。

### C/C++

イントロダクションでも書きましたが、標準Cライブラリ(libc)で `getopt.h` が用意されています。  
POSIX準拠のパースが行える`getopt`、GNU拡張形式のパースが行える`getopt_long`が関数として用意されています。

(参考: `man 3 getopt`)

### Python

[argparse](https://docs.python.org/ja/3/library/argparse.html)という標準パッケージが用意されています。
`ctypes`を経由して`getopt.h`を使っているのかなーなどとぼんやり思っていましたが(**超絶失礼**)、なんと**パース処理が丸ごとPythonで記述されています**([python/cpython/Lib/argparse.py](https://github.com/python/cpython/blob/bcc4cb0c7d5e0590928e74cae86b0a7938c0f74b/Lib/argparse.py#L1906))。すごい。~~いやまあよく考えればそりゃそうなのかもしれないんですが~~

ちなみに、`getopt.h`の動作に準拠した[getopt](https://docs.python.org/ja/3/library/getopt.html)という**そのものズバリ**な名前のモジュールも用意されています。

### Swift

標準ライブラリには存在しないようですが、公式から[ArgumentParser](https://github.com/apple/swift-argument-parser) というライブラリが提供されています。  
プロトコル`ParsableCommand`に準拠した構造体に、メタ情報やオプション、プログラム実行時の処理などを追加する仕組みになっており、構造体がそのままCLIとして動作する設計になっています。かっこいい。個人的に一番好きです。


他の言語でCLIを書いたことがないの詳しくないのですが(偏った学習範囲)、標準または公式で用意されている場合が多い…んでしょうか?

## 本題

はい。**やっと本題です。**  
今回は一文字のオプションの他に `--help` や `--version` も実装したいので、最終的には`getopt_long`を使うことになるのですが、
その前に一旦`getopt`の使い方を整理しておきます。

### getopt()

関数`getopt()`の構文は次のようになっています:

> SYNOPSIS  
>     extern char *optarg;  
>     extern int optind;  
>     extern int optopt;  
>     extern int opterr;  
>     extern int optreset;
> 
>     int  
>     getopt(int argc, char * const argv[], const char *optstring);
> 

(参考: `man 3 getopt`)

`argc`, `argv`には `main` 関数の引数として渡されたものをそのまま横流しできるようです。もちろん自分で生成してもOK。

`optstring`には 受け取りたいオプションを列挙します。この際、オプションがオプション引数をとる場合は`:`、とっても取らなくても良い場合は`::`を文字の後に付加します。

例えば

 - *input*の意図で `i` 、*output*の意図で `o` 、*verbose*の意図で `v` というオプションを受け取りたい状況で、
 - 入力元は指定してほしいので `i` には引数が欲しいが、出力先は入力元と同じディレクトリに拡張子だけ変えたものを出力するので、引数は指定しなくても良い

という場合は `i:o::v` のようにします。

戻り値は、発見したオプションの文字 またはそれ以上見つからなければ `-1` となります。`optstring` に記述のないオプションが渡った場合は`?`が返ります。

その他グローバル変数など様々な要因で様々な変数が変化しますが、ここでは割愛します(manページがかなり詳細なので)。~~正直ステートフルで扱いづらいようにも感じます…~~

### 使ってみる

実行引数を毎回コンパイルして渡してとやっていると非常に大変なので、文字列配列から`char**`を生成して`getopt`に直接突っ込みます。**`src/`配下がソースファイルだらけ**です。

(文字列変数の処理に 拙作[Enchan1207/tinystr](https://github.com/Enchan1207/tinystr)を使用しています。)

### 1. オプションに定義された引数のみが順番通りに渡った場合 (`src/test_1`)

まずは最も単純なパターンから。

 - optstring: `i:o::v`
 - argv:
     - `(progname)`
     - `-i`
     - `option_argument_i`
     - `-o`
     - `option_argument_o`
     - `-v`

`getopt`の戻り値が `-1` になるまでループすると...

|ループ回数|戻り値|`optind`|`optarg`|
|-|-|-|-|-|
|0|i|3|option_argument_i|
|1|o|5|option_argument_o|
|2|v|6|NULL|

このようになります。  
渡された順に引数が解析され、引数を持つ場合は`optarg`に格納される…といった動作です。

### 2. オペランドを含む場合 (`src/test_2`)

 - optstring: `i:o::v`
 - argv:
     - `(progname)`
     - `-i`
     - `option_argument_i`
     - `-o`
     - `option_argument_o`
     - `-v`
     - `operand_1`
     - `operand_2`
     - `operand_3`

パース処理までは1.と同じ動作となりますが、`getopt`が`-1`を返した段階の`optind`が`6`となります。
つまり、以下のようなコードでオペランドを順番に取得することが可能です:

```C
for (int i = optind; i < args.count; i++) {
    int operand_index = i - optind;
    printf("operand %d: %s\n", operand_index, argv[i]);
}
```

### 3. オプションより前にオペランドがくる場合 (`src/test_3`)

 - optstring: `i:o::v`
 - argv:
     - `operand_1`
     - `operand_2`
     - `operand_3`
     - `(progname)`
     - `-i`
     - `option_argument_i`
     - `-o`
     - `option_argument_o`
     - `-v`

この場合の処理は**処理系依存**ですが、手元の環境では **一度目の`getopt`の呼び出しで`-1`が返ります**。
つまりその後の出力は**全てオペランドとして解釈**されます。

### 4. 引数を要求するオプションに対し、オプションのみを渡した場合 (`src/test_4`)

 - optstring: `i:`
 - argv:
     - `-i`

`<実行ファイル名>: option requires an argument -- i` というエラーが表示され、`getopt`の戻り値は`?`となります。
このエラーはグローバル変数`opterr`を`0`に設定することで非表示にできます。

### 5. 引数をoptionalで要求するオプションに対し、オプションのみを渡した場合 (`src/test_5`)

 - optstring: `o::`
 - argv:
     - `-o`

引数がオプショナルの場合、`getopt`の戻り値は`?`となりますが、**`optopt`の戻り値は`o`となります**。

したがって、オプション引数が設定されたかどうかは `optopt == getopt()` で判断することができます。

### getopt_long()

関数`getopt_long()`の構文は次のようになっています:

> 
> SYNOPSIS  
>     extern char *optarg;  
>     extern int optind;  
>     extern int optopt;  
>     extern int opterr;  
>     extern int optreset;
> 
>     int  
>     getopt_long(int argc, char * const *argv, const char *optstring, const struct option *longopts, int *longindex);
> 

(参考: `man 3 getopt_long`)

基本的にはほぼほぼ`getopt`と変わりません。ロングオプションを考慮しなければ`getopt`と同じように動作します。

`struct option` および `longindex` については、順を追って整理します。

### 6. ロングオプションを受け入れるための構造体を作る (`src/test_6`)

構造体 `option` の構造は以下のようになっています:

 - `const char* name`  
    ロングオプションの名称.
 - `int has_arg`  
    オプションが引数を要求するかどうか.   
    定数 `no_argument`, `required_argument`, `optional_argument` が用意されている.
 - `int* flag`  
    このオプションをフラグ変数として扱うかどうか.
 - `int val`  
    このオプションを発見したときの `getopt_long` の戻り値となる値。

`name`, `has_arg` に関しては`getopt`と同様です。

`flag`はその名の通り、そのオプションをフラグとして扱うかを制御します。  
ここにポインタを渡すと **`getopt_long`の戻り値が0となり**、また本来の戻り値がそちらに格納されます。

`val` はオプションを発見した際の`getopt_long`の戻り値を設定します。

### 7. ロングオプションとショートオプションの戻り値が等しい場合 (`src/test_7`)

ロングオプションの戻り値と同じものが`optstring`にも含まれていた場合、動作は以下のようになります。

 - ショートオプションを発見したとき:
    - 戻り値は`optstring`内のものが返る
    - ロングオプションではないので、 `longindex` は更新されない
 - ロングオプションを発見したとき:
    - 戻り値は `struct option` のメンバ `val` が返る
    - ロングオプションとして指定されているため、 `longindex` が更新される

なお、**両方渡しても問題なく動作します**。
