list(APPEND IMGUI_FILES "imgui.cpp" "imgui.h" "imgui_demo.cpp" "imgui_draw.cpp" "imgui_impl_glfw_gl3.cpp" "imgui_impl_glfw_gl3.h" "imgui_internal.h" "imgui_memory_editor.h" "stb_rect_pack.h" "stb_textedit.h" "stb_truetype.h")

#Expand source files to their full path
prepend_filepath("IMGUI_FILES" "${CMAKE_CURRENT_LIST_DIR}")

#Make files able to be found by others via e.g. #include "imgui.h" 
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

debug_print_sourcevar("IMGUI_FILES")

#Add files to executable
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${IMGUI_FILES})

#Add files into an IDE folder
source_group("ext" FILES ${IMGUI_FILES})
