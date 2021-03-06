
find_package(LATEX COMPONENTS XELATEX REQUIRED)

function(add_example name input0)

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.pdf"
        COMMAND $<TARGET_FILE:umdoc> "${CMAKE_CURRENT_SOURCE_DIR}/${input0}" -o "${CMAKE_CURRENT_BINARY_DIR}/${name}.pdf"
        DEPENDS $<TARGET_FILE:umdoc> ${input0} ${ARGN}
    )

    add_custom_target(${name} ALL
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}.pdf"
        SOURCES ${input0} ${ARGN}
    )

    set_target_properties(${name} PROPERTIES
        FOLDER "Examples"
    )

    source_group("" FILES ${input0} ${ARGN})

    install(FILES ${input0} ${ARGN}
        DESTINATION "Examples/${name}"
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}.pdf"
        DESTINATION "Examples"
    )

endfunction()

add_example(coverpage
    coverpage/umdoc.xml
    coverpage/cover.pdf
    coverpage/example.md
)

add_example(customenvironment
    customenvironment/umdoc.xml
    customenvironment/style.tex
    customenvironment/example.md
)

add_example(environmentscript
    environmentscript/umdoc.xml
    environmentscript/myscript1
    environmentscript/myscript1.bat
    environmentscript/myscript2
    environmentscript/myscript2.bat
    environmentscript/example.md
)

add_example(headerfooter
    headerfooter/umdoc.xml
    headerfooter/style.tex
    headerfooter/example.md
)

add_example(latexclass
    latexclass/umdoc.xml
    latexclass/exampleclass.cls
    latexclass/example.md
)

add_example(md2pdf
    md2pdf/example.md
)

add_example(tables
    tables/umdoc.xml
    tables/example.md
)

add_example(tablesfigures
    tablesfigures/umdoc.xml
    tablesfigures/example.md
    tablesfigures/circle.png
)

add_example(tablestyles
    tablestyles/umdoc.xml
    tablestyles/example.md
)

add_example(codeblocks
    codeblocks/umdoc.xml
    codeblocks/example.md
)
