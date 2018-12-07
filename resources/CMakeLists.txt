list(APPEND VERTEX_SHADERS "mainVert.glsl" "simple_vert.glsl" "voxel_normalCalc_vert.glsl")
list(APPEND FRGMNT_SHADERS "mainFrag.glsl" "simple_frag.glsl" "voxel_normalCalc_frag.glsl")

prepend_filepath("VERTEX_SHADERS" "${CMAKE_CURRENT_LIST_DIR}/shaders/")
prepend_filepath("FRGMNT_SHADERS" "${CMAKE_CURRENT_LIST_DIR}/shaders/")

#print these vars if boolval in Helpers.cmake is set
debug_print_sourcevar("VERTEX_SHADERS")
debug_print_sourcevar("FRGMNT_SHADERS")

source_group("glsl" FILES ${VERTEX_SHADERS})
source_group("glsl" FILES ${FRGMNT_SHADERS})
