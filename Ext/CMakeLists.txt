
add_subdirectory(libnstd)

set_target_properties(nstd nstdCrypto nstdDocument nstdSocket PROPERTIES
    FOLDER "Ext"
)
