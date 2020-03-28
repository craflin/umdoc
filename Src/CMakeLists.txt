
set(sources
    Generator.cpp
    Generator.h
    HtmlGenerator.cpp
    HtmlGenerator.h
    InputData.h
    Main.cpp
    OutputData.cpp
    OutputData.h
    Parser.cpp
    Parser.h
    Reader.cpp
    Reader.h
    TexGenerator.cpp
    TexGenerator.h
    PlainGenerator.cpp
    PlainGenerator.h
)

add_executable(umdoc ${sources})

target_link_libraries(umdoc
    PRIVATE libnstd::Document
)
target_compile_definitions(umdoc
    PRIVATE "VERSION=\"${PROJECT_VERSION}\""
)

set_target_properties(umdoc PROPERTIES
    FOLDER "Src"
)

source_group("" FILES ${sources})

install(TARGETS umdoc DESTINATION .)

add_subdirectory(Tests)
