
set(sources
    Generator.cpp
    Generator.hpp
    HtmlGenerator.cpp
    HtmlGenerator.hpp
    InputData.hpp
    Main.cpp
    OutputData.cpp
    OutputData.hpp
    Parser.cpp
    Parser.hpp
    Reader.cpp
    Reader.hpp
    TexGenerator.cpp
    TexGenerator.hpp
    PlainGenerator.cpp
    PlainGenerator.hpp
)

add_executable(umdoc ${sources})

target_link_libraries(umdoc
    PRIVATE libnstd::Document
)
target_compile_definitions(umdoc
    PRIVATE "VERSION=\"${PROJECT_VERSION}\""
)
target_compile_features(umdoc
    PRIVATE cxx_override
)

set_target_properties(umdoc PROPERTIES
    FOLDER "Src"
)

source_group("" FILES ${sources})

install(TARGETS umdoc DESTINATION .)

add_subdirectory(Tests)
