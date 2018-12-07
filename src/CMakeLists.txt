cmake_minimum_required(VERSION 3.1)

list(APPEND ENGINE_HEADERS "Application.hpp") 
list(APPEND ENGINE_SOURCES "Application.cpp" "main.cpp")

list(APPEND SYSTEM_SOURCES "ChunkSystem.cpp" "InputSystem.cpp" "PickSystem.cpp" "PhysicsSystem.cpp" "RenderSystem.cpp" "SelectionSystem.cpp" "VolumeRenderSystem.cpp" "PlayerSystem.cpp" "ColonistSystem.cpp")
list(APPEND SYSTEM_HEADERS "ChunkSystem.hpp" "InputSystem.hpp" "PickSystem.hpp" "PhysicsSystem.hpp" "RenderSystem.hpp" "SelectionSystem.hpp" "VolumeRenderSystem.hpp" "PlayerSystem.hpp" "ColonistSystem.hpp")

list(APPEND ECS_MANAGEMENT_SOURCES "EntityManager.cpp" "Messaging.cpp" "System.cpp" "ComponentManager.cpp" "ComponentStorage.cpp" "Component.cpp")
list(APPEND ECS_MANAGEMENT_HEADERS "EntityManager.hpp" "Messaging.hpp" "System.hpp" "ComponentManager.hpp" "ComponentStorage.hpp" "Component.hpp" "ComponentOrdering.h")

list(APPEND GRAPHICS_HELPER_SOURCES "Camera.cpp" "Model.cpp" "Shape.cpp" "GLSL.cpp" "Program.cpp" "WindowManager.cpp" "GLMUtils.cpp" "MatrixStack.cpp" "tiny_obj_loader.cpp")
list(APPEND GRAPHICS_HELPER_HEADERS "Camera.hpp" "Model.hpp" "Shape.hpp" "GLSL.hpp" "Program.hpp" "WindowManager.hpp" "GLMUtils.hpp" "MatrixStack.hpp" "tiny_obj_loader.h")

list(APPEND MISC_SOURCES "BulletDraw.cpp" "RegionUtils.cpp" "FastNoise.cpp") 
list(APPEND MISC_HEADERS "BulletDraw.hpp" "RegionUtils.hpp" "FastNoise.h" "PolyVoxOpenGL.hpp" "NoisePager.h" "CastleDef.h")

prepend_listdir("ENGINE_SOURCES" "SYSTEM_SOURCES" "ECS_MANAGEMENT_SOURCES" "GRAPHICS_HELPER_SOURCES" "MISC_SOURCES") 
prepend_listdir("ENGINE_HEADERS" "SYSTEM_HEADERS" "ECS_MANAGEMENT_HEADERS" "GRAPHICS_HELPER_HEADERS" "MISC_HEADERS") 

set(ALL_SOURCES ${ENGINE_SOURCES} ${SYSTEM_SOURCES} ${ECS_MANAGEMENT_SOURCES} ${GRAPHICS_HELPER_SOURCES} ${MISC_SOURCES})
set(ALL_HEADERS ${ENGINE_HEADERS} ${SYSTEM_HEADERS} ${ECS_MANAGEMENT_HEADERS} ${GRAPHICS_HELPER_HEADERS} ${MISC_HEADERS}) 

#Make all filepaths absolute (vs relative)
#debug_print_var("ECS_MANAGEMENT_SOURCES")

debug_print_sourcevar("ALL_SOURCES")
debug_print_sourcevar("ALL_HEADERS")

#Add files to executable
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${ALL_SOURCES} ${ALL_HEADERS})

#source_group("SourceFiles" FILES ${ALL_SOURCES})
#source_group("HeaderFiles" FILES ${ALL_HEADERS})

#Add files to IDE folder (doesn't affect executable)
source_group("Engine" FILES ${ENGINE_SOURCES})

source_group("Systems" FILES ${SYSTEM_SOURCES})# ${SYSTEM_HEADERS})

source_group("ECS_Other" FILES ${ECS_MANAGEMENT_SOURCES})# ${ECS_MANAGEMENT_HEADERS})

source_group("Graphics_Helpers" FILES ${GRAPHICS_HELPER_SOURCES})# ${GRAPHICS_HELPER_HEADERS})
source_group("Other_Files" FILES ${MISC_SOURCES})# ${MISC_HEADERS})
