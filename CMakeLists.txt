cmake_minimum_required(VERSION 3.28)
project(chat-server)

set(UNP_ROOT /home/power/projects/unpv13e)

add_executable(daytimetcpcli daytimetcpcli.c)

# 包含 unp.h 所在目录
target_include_directories(daytimetcpcli PRIVATE ${UNP_ROOT}/lib)

# 链接 libunp.a
target_link_libraries(daytimetcpcli PRIVATE ${UNP_ROOT}/libunp.a)
