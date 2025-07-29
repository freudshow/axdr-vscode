cmake_minimum_required(VERSION 3.10)
project(axdr C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加源文件
add_library(axdr STATIC
    src/axdr.c
    src/axdr_sequence.c
    src/test_sequence.c)

# 添加测试可执行文件
add_executable(test_axdr
    src/test_axdr.c
    src/test_sequence.c
)

# 添加 test_varint 测试可执行文件
add_executable(test_varint src/test_varint.c)

# 添加 test_varstring 测试可执行文件
add_executable(test_varstring src/test_varstring.c)

# 链接测试程序与库
target_link_libraries(test_axdr axdr)
target_link_libraries(test_varint axdr)
target_link_libraries(test_varstring axdr)

# 添加头文件搜索路径
target_include_directories(axdr PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)
target_include_directories(test_axdr PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)
target_include_directories(test_varint PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)
target_include_directories(test_varstring PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)
