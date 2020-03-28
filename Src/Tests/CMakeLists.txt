
add_executable(umdoc_test
    Test.cpp
    ../Parser.cpp
    ../OutputData.cpp
    ../TexGenerator.cpp
    ../Generator.cpp
)

target_link_libraries(umdoc_test libnstd::Core)

add_test(NAME umdoc_test COMMAND umdoc_test)

set_target_properties(umdoc_test PROPERTIES
    FOLDER "Src/Tests"
)
