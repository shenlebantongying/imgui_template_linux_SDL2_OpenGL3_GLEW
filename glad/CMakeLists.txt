cmake_minimum_required(VERSION 3.19)
project(glad)

add_library(glad include/glad/glad.h glad.c)
target_include_directories(glad PUBLIC include/)
