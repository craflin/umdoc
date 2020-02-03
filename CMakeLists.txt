cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0048 NEW)

project(umdoc VERSION 0.2.0)

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
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-win32")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
    string(TOLOWER "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CPACK_PACKAGE_FILE_NAME)
    set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.txt")
    install(SCRIPT CPackInstall.txt)
endif()

include(CPack)
