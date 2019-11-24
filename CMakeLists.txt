cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0048 NEW)

project(umdoc VERSION 0.1.7)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

add_subdirectory(Ext)
add_subdirectory(Src)
add_subdirectory(Doc)
add_subdirectory(Examples)

install(FILES NOTICE LICENSE DESTINATION .)

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)
