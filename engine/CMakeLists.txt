cmake_minimum_required(VERSION 3.5)

# Project Name
project(engine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add source files
add_executable(${PROJECT_NAME} src/main.cpp src/xml_parser.cpp src/draw.cpp src/utils.cpp)

include_directories(include)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
  message(FATAL_ERROR "OPENGL not found!")
endif()

# Integrate tinyXML2
include(FetchContent)
FetchContent_Declare(
  tinyxml2
  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG master
)
FetchContent_MakeAvailable(tinyxml2)

# Windows-specific configuration
if(WIN32)
  message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})
  set(TOOLKITS_FOLDER
      ""
      CACHE PATH "Path to Toolkits folder")

  if(NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h"
     OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
    message(FATAL_ERROR "GLUT not found!")
  endif()

  include_directories(${TOOLKITS_FOLDER}/glut)
  target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES}
                        ${TOOLKITS_FOLDER}/glut/glut32.lib)

  if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
    file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll
         DESTINATION ${CMAKE_BINARY_DIR})
  endif()

  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
                                                              ${PROJECT_NAME})
else() # Linux and macOS

  find_package(GLUT REQUIRED)
  include_directories(${GLUT_INCLUDE_DIR})
  link_directories(${GLUT_LIBRARY_DIRS})
  add_definitions(${GLUT_DEFINITIONS})

  if(NOT GLUT_FOUND)
    message(FATAL_ERROR "GLUT not found!")
  endif()

  if(APPLE) # macOS-specific changes
    message(STATUS "Applying macOS-specific settings...")

    # Silence OpenGL deprecation warnings
    add_definitions(-DGL_SILENCE_DEPRECATION)

    # Ensure correct framework linking
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework GLUT")
  else()
    # Linux linking
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
  endif()

endif()

target_include_directories(engine PRIVATE include)
target_link_libraries(engine tinyxml2)
