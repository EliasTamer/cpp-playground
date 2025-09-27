# this is the main blueprint to building our project
# it creates 2 subfolders inside the build folder, they inherit the settings from here.

cmake_minimum_required(VERSION 3.19)

project(Playground CXX)

set(CMAKE_CXX_STANDARD 23)

# adding 2 folders inside the build folder
add_subdirectory(LogsHandler)
add_subdirectory(CreditCardValidator)
