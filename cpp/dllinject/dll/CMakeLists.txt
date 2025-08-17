cmake_minimum_required(VERSION 3.10) # 最低版本要求
project(MyDLL LANGUAGES CXX) # 项目名称
add_definitions(-DUNICODE=0 -D_UNICODE=0)

# 生成 DLL
add_library(MyDLL SHARED
    dllmain.cpp # 入口函数，用于显示消息框
)