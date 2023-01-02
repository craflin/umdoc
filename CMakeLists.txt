cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0048 NEW)

project(umdoc VERSION 0.3.0)

set(CDEPLOY_NO_DEBUG_BUILD True)
set(CDEPLOY_NO_COMPILER True)

include(CDeploy)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")

if(MSVC)
    string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
    string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT /Os /Oy /O1 /GF /GL")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
    set(CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL "/LTCG")
endif()

add_subdirectory(Ext)
add_subdirectory(Src)
add_subdirectory(Doc)
add_subdirectory(Examples)

install(FILES NOTICE LICENSE DESTINATION .)

if(NOT WIN32)
    list(APPEND CPACK_GENERATOR "DEB")
    set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.txt")
    set(CPACK_STRIP_FILES True)
    install(SCRIPT CPackInstall.txt)
endif()

include(CPack)

if(WIN32)
    deploy_export(umdoc EXECUTABLE
        CONFIGURATION Release
        IMPORTED_LOCATION umdoc.exe
    )
    deploy_export(umdoc EXECUTABLE
        CONFIGURATION Debug
        IMPORTED_LOCATION umdoc.exe
    )
else()
    deploy_export(umdoc EXECUTABLE
        IMPORTED_LOCATION umdoc
    )
endif()

install_deploy_export()
