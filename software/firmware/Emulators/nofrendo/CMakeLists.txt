set(COMPONENT_SRCDIRS "nes mappers .")
set(COMPONENT_ADD_INCLUDEDIRS "nes mappers .")
set(COMPONENT_REQUIRES "retro-go")
register_component()
rg_setup_compile_options(-Wno-array-bounds)
