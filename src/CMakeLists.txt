#
#
#
cmake_minimum_required(VERSION 3.21)

# Fetch tinystr
include(FetchContent)
FetchContent_Declare(
    tinystr
    GIT_REPOSITORY https://github.com/Enchan1207/tinystr.git
    GIT_TAG v0.2.0
)
FetchContent_MakeAvailable(tinystr)

# add targets
add_executable(test_1 test_1.c)
target_link_libraries(test_1 tinystr)
target_compile_options(test_1 PRIVATE -Wall -Werror)
