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

function(add_target target_name)
    add_executable(${target_name} ${target_name}.c)
    target_link_libraries(${target_name} tinystr)
    target_compile_options(${target_name} PRIVATE -Wall -Werror)
endfunction(add_target target_name)

# add targets
add_target(test_1)
add_target(test_2)
add_target(test_3)
add_target(test_4)
add_target(test_5)
add_target(test_6)
add_target(test_7)
