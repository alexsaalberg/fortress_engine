# target_sources was added in 3.1
cmake_minimum_required(VERSION 3.1)

# Functions and Macros
include("Helpers.cmake")

# Name of the project
project(FortressEngine)

# glfw.dylib is in usr/local/lib
# If absent, "ld: library not found for -lglfw"
link_directories("/usr/local/lib")

# Set the executable. 
# Sources are set via target_sources in add_subdirectorie(s)
add_executable(${CMAKE_PROJECT_NAME} "") 

# Run the code in these CMake files
include("src/CMakeLists.txt")
include("ext/CMakeLists.txt")
include("resources/CMakeLists.txt")

# Libraries (could probably add to sub_directory CMakeLists.txt(s))

# Add GLFW
# Get the GLFW environment variable.
# There should be a CMakeLists.txt in the specified directory.
message(STATUS "")
message(STATUS "=====> START GLFW <=====")
set(GLFW_DIR "$ENV{GLFW_DIR}")
if(GLFW_DIR)
	message(STATUS "GLFW environment variable found")

	option(GLFW_BUILD_EXAMPLES "GLFW_BUILD_EXAMPLES" OFF)
	option(GLFW_BUILD_TESTS "GLFW_BUILD_TESTS" OFF)
	option(GLFW_BUILD_DOCS "GLFW_BUILD_DOCS" OFF)
	if(CMAKE_BUILD_TYPE MATCHES Release)
		add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/release)
	else()
		add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/debug)
	endif()

	target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLFW_DIR}/include)
	target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})
else()
	message(STATUS "GLFW environment variable `GLFW_DIR` not found, GLFW3 must be installed with the system")

	find_package(PkgConfig)
	if (PKGCONFIG_FOUND)
		message(STATUS "PkgConfig found")
		pkg_search_module(GLFW REQUIRED glfw3)
		message(STATUS "PkgConfig has found glfw3")
		target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
		target_link_libraries(${CMAKE_PROJECT_NAME} ${GLFW_LIBRARIES})
		print_var("GLFW_INCLUDE_DIRS")
		print_var("GLFW_LIBRARIES")
	else()
		message(STATUS "No PkgConfig found")
		find_package(glfw3)
		if (GLFW_FOUND)
			target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
			target_link_libraries(${CMAKE_PROJECT_NAME} glfw)
		else()
			message(STATUS "glfw3 package does not exist")
			message(STATUS "glfw3 must be installed through a package manager that automatically set up path, like vcpkg")
		endif()
	endif()
endif()

message(STATUS "=====> END GLFW <=====")
message(STATUS "")

message(STATUS "=====> START GLM <=====")
# Add GLM
# Get the GLM environment variable. Since GLM is a header-only library, we
# just need to add it to the include directory.
set(GLM_INCLUDE_DIR "$ENV{GLM_INCLUDE_DIR}")
if(GLM_INCLUDE_DIR)
	message(STATUS "GLM include dir found with env var 'GLM_INCLUDE_DIR': ${GLM_INCLUDE_DIR}")
	target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIR})
else()
	# If the GLM_INCLUDE_DIR environment variable is not set, we assume
	# the user has installed GLM properly on their system
	find_path(GLM_INCLUDE_DIR_FINDPATH "glm/glm.hpp")
	if(GLM_INCLUDE_DIR_FINDPATH)
		message(STATUS "GLM include dir found with find_path(): ${GLM_INCLUDE_DIR_FINDPATH}")
		target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIR_FINDPATH})
	else()
		message(STATUS "GLM include dir not found, GLM must be installed with the system")
	endif()
endif()
message(STATUS "=====> END GLFW <=====")
message(STATUS "")

message(STATUS "=====> START BULLET <=====")
find_package(bullet)
if(BULLET_FOUND)
	message(STATUS "Bullet Library Files: ${BULLET_LIBRARIES}")
	target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${BULLET_INCLUDE_DIRS})
	target_link_libraries(${CMAKE_PROJECT_NAME} ${BULLET_LIBRARIES})
	message(STATUS "FOUND BULLET YAY!")
endif()
message(STATUS "=====> END BULLET <=====")
message(STATUS "")

# OS specific options and libraries
if(WIN32)
	# c++0x is enabled by default.
	# -Wall produces way too many warnings.
	# -pedantic is not supported.
	target_link_libraries(${CMAKE_PROJECT_NAME} opengl32.lib)
	target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC NOMINMAX _CRT_SECURE_NO_WARNINGS)
else()
	# Enable all pedantic warnings.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic")

	if(APPLE)
		# Add required frameworks for GLFW.
		target_link_libraries(${CMAKE_PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	else()
		# Link the Linux OpenGL library
		target_link_libraries(${CMAKE_PROJECT_NAME} "GL" "dl")
	endif()
endif()

#set(CMAKE_SUPPRESS_REGENERATION true) #surpress generation of ZERO_CHECK
