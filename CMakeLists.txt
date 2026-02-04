cmake_minimum_required(VERSION 3.28)
project(chat-server)

set(UNP_ROOT /home/power/projects/unpv13e)

# 设置为 Debug 模式
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_executable(server src/echo_server.c)
add_executable(client src/echo_client.c)

# 包含 unp.h 所在目录
target_include_directories(server PRIVATE ${UNP_ROOT}/lib)
target_include_directories(client PRIVATE ${UNP_ROOT}/lib)

# 链接 libunp.a
target_link_libraries(server PRIVATE ${UNP_ROOT}/libunp.a)
target_link_libraries(client PRIVATE ${UNP_ROOT}/libunp.a)

