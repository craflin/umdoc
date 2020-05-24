
set(DISABLE_ADD_SUBDIRECTORY_TEST true)
macro(add_subdirectory)
    if(NOT DISABLE_ADD_SUBDIRECTORY_TEST OR NOT ("${ARGV0}" STREQUAL "test"))
        _add_subdirectory(${ARGV})
    endif()
endmacro()

add_subdirectory(libnstd)

set_target_properties(nstd nstdCrypto nstdDocument nstdSocket PROPERTIES
    FOLDER "Ext"
)
