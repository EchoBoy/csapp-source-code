cmake_minimum_required(VERSION 3.13)
project(csapp)

set(CMAKE_CXX_STANDARD 14)

add_executable(csapp "csapp.c" "12-28 基于预线程化的并发echo/pre_mutil_thread_echo.c" "12-28 基于预线程化的并发echo/echo_cnt.c")
