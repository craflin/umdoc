
set(sources
    Test.cpp
    ../Parser.cpp
    ../OutputData.cpp
    ../TexGenerator.cpp
    ../Generator.cpp
)


add_executable(umdoc_test ${sources})
add_dependencies(umdoc_test umdoc)

source_group("" FILES ${sources})

target_link_libraries(umdoc_test
    PRIVATE libnstd::Core
)

target_compile_definitions(umdoc_test
    PRIVATE
        DEBUG
        "UMDOC_EXECUTABLE=\"$<TARGET_FILE:umdoc>\""
)

target_compile_features(umdoc_test
    PRIVATE cxx_override
)

add_test(NAME umdoc_test COMMAND umdoc_test)

set_target_properties(umdoc_test PROPERTIES
    FOLDER "Src/Tests"
)
