cmake_minimum_required(VERSION 3.5)
project(engine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
  ${PROJECT_NAME}
  src/main.cpp
  src/xml_parser.cpp
  src/draw.cpp
  src/utils.cpp
  src/menu.cpp
  src/imgui/imgui.cpp
  src/imgui/imgui_demo.cpp
  src/imgui/imgui_impl_glut.cpp
  src/imgui/imgui_impl_opengl2.cpp
  src/imgui/imgui_tables.cpp
  src/imgui/imgui_widgets.cpp
  src/imgui/imgui_draw.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include include/imgui)

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
  GIT_TAG master)
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

  if(NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h"
     OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")
    message(ERROR ": GLEW not found")
  endif(NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h"
        OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")

  include_directories(${TOOLKITS_FOLDER}/glut ${TOOLKITS_FOLDER}/glew)
  target_link_libraries(
    ${PROJECT_NAME} ${OPENGL_LIBRARIES} ${TOOLKITS_FOLDER}/glut/glut32.lib
    ${TOOLKITS_FOLDER}/glew/glew32.lib)

  if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll"
     AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll")
    file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll
         DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${TOOLKITS_FOLDER}/glew/glew32.dll
         DESTINATION ${CMAKE_BINARY_DIR})
  endif(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll"
        AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll")

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

    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_directories(${GLEW_LIBRARY_DIRS})
    add_definitions(${GLEW_DEFINITIONS})

    if(NOT GLEW_FOUND)
      message(FATAL_ERROR "GLEW não encontrado!")
    endif()
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES}
                          GLEW)
  endif()

endif()

target_include_directories(engine PRIVATE include)
target_link_libraries(engine tinyxml2)
