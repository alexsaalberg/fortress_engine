include(FetchContent)
FetchContent_Declare(
  bullet
  GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
  GIT_TAG 2.87
)

MESSAGE(STATUS "Fetching Bullet Physics from github...")
FetchContent_GetProperties(bullet)
if(NOT bullet_POPULATED)
  MESSAGE(STATUS "Populating...")
  FetchContent_Populate(bullet)

  MESSAGE(STATUS "Globbing sources and headers...")
  file(GLOB_RECURSE BULLET_SOURCE_FILES LIST_DIRECTORIES false ${bullet_SOURCE_DIR}/src/*.cpp)
  file(GLOB_RECURSE BULLET_HEADER_FILES LIST_DIRECTORIES false ${bullet_SOURCE_DIR}/src/*.h)
  list(FILTER BULLET_SOURCE_FILES EXCLUDE REGEX "Bullet3OpenCL")
  list(FILTER BULLET_SOURCE_FILES EXCLUDE REGEX "Bullet3Dynamics")
  list(FILTER BULLET_SOURCE_FILES EXCLUDE REGEX "Bullet3Collision")
  list(FILTER BULLET_SOURCE_FILES EXCLUDE REGEX "Bullet3Geometry")
  list(FILTER BULLET_SOURCE_FILES EXCLUDE REGEX "Bullet3Serialize/Bullet2FileLoader")

  MESSAGE(STATUS "target_sources(), source_group(), include_directories() on Bullet's sources...")
  target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${BULLET_SOURCE_FILES})
  source_group("ext\\Bullet" FILES ${BULLET_SOURCE_FILES})
  include_directories(${bullet_SOURCE_DIR}/src)
endif()
MESSAGE(STATUS "Finished Fetching Bullet!")