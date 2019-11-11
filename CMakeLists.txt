cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0048 NEW)

project(umdoc VERSION 0.1.7)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")

add_subdirectory(Ext)
add_subdirectory(Src)
add_subdirectory(Doc)
add_subdirectory(Examples)
