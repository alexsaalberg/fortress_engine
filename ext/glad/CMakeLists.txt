list(APPEND GLAD_SOURCES "glad.c")

#Expand source files to their full path
prepend_filepath("GLAD_SOURCES" "${CMAKE_CURRENT_LIST_DIR}/src")

debug_print_sourcevar("GLAD_SOURCES")

#Add files to executable
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${GLAD_SOURCES})

#Make files able to be found by others via e.g. #include "glad/glad.h" 
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

source_group("ext" FILES ${GLAD_SOURCES})
