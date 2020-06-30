
add_executable(umdoc_test
    Test.cpp
    ../Parser.cpp
    ../OutputData.cpp
    ../TexGenerator.cpp
    ../Generator.cpp
)

target_link_libraries(umdoc_test
    PRIVATE libnstd::Core
)

target_compile_features(umdoc_test
    PRIVATE cxx_override
)

add_test(NAME umdoc_test COMMAND umdoc_test)

set_target_properties(umdoc_test PROPERTIES
    FOLDER "Src/Tests"
)
