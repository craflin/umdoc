
add_executable(umdoc_test
    Test.cpp
    ../Parser.cpp
    ../OutputData.cpp
    ../Generator.cpp
)

target_link_libraries(umdoc_test libnstd)

add_test(NAME umdoc_test COMMAND umdoc_test)
