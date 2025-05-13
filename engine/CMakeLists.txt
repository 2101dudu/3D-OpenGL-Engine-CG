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
  src/structs.cpp
  src/catmull_rom.cpp
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

  # FreeGLUT (supports 64-bit and 32-bit)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit
    set(FREEGLUT_LIB "${TOOLKITS_FOLDER}/freeglut/x64/freeglut.lib")
    set(FREEGLUT_DLL "${TOOLKITS_FOLDER}/freeglut/x64/freeglut.dll")
  else() # 32-bit
    set(FREEGLUT_LIB "${TOOLKITS_FOLDER}/freeglut/freeglut.lib")
    set(FREEGLUT_DLL "${TOOLKITS_FOLDER}/freeglut/freeglut.dll")
  endif()

  if(NOT EXISTS "${TOOLKITS_FOLDER}/freeglut/GL/freeglut.h"
     OR NOT EXISTS "${FREEGLUT_LIB}")
    message(FATAL_ERROR "FreeGLUT not found!")
  endif()

  if(NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h"
     OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")
    message(ERROR ": GLEW not found")
  endif(NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h"
        OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")

  include_directories(${TOOLKITS_FOLDER}/glew ${TOOLKITS_FOLDER}/freeglut)
  target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES}
                        ${TOOLKITS_FOLDER}/glew/glew32.lib ${FREEGLUT_LIB})

  if(EXISTS "${FREEGLUT_DLL}")
    file(COPY ${FREEGLUT_DLL} DESTINATION ${CMAKE_BINARY_DIR})
  endif()

  if(EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll")
    file(COPY ${TOOLKITS_FOLDER}/glew/glew32.dll
         DESTINATION ${CMAKE_BINARY_DIR})
  endif(EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll")

  # DevIL
  if(NOT EXISTS "${TOOLKITS_FOLDER}/devil/IL/il.h"
     OR NOT EXISTS "${TOOLKITS_FOLDER}/devil/DevIL.lib")
    message(FATAL_ERROR "DevIL not found!")
  endif()

  include_directories(${TOOLKITS_FOLDER}/devil)
  target_link_libraries(${PROJECT_NAME} ${TOOLKITS_FOLDER}/devil/DevIL.lib)

  if(EXISTS "${TOOLKITS_FOLDER}/devil/DevIL.dll")
    file(COPY ${TOOLKITS_FOLDER}/devil/DevIL.dll DESTINATION ${CMAKE_BINARY_DIR})
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

    # Manually set paths to FreeGLUT
    set(FREEGLUT_INCLUDE_DIR /opt/homebrew/include)
    set(FREEGLUT_LIBRARY
        /opt/homebrew/Cellar/freeglut/3.6.0/lib/libglut.3.12.1.dylib)

    if(NOT EXISTS ${FREEGLUT_INCLUDE_DIR}/GL/freeglut.h OR NOT EXISTS
                                                           ${FREEGLUT_LIBRARY})
      message(
        FATAL_ERROR
          "FreeGLUT not found! Install it using: brew install freeglut")
    endif()

    message(STATUS "FreeGLUT found: ${FREEGLUT_LIBRARY}")
    message(STATUS "FreeGLUT include path: ${FREEGLUT_INCLUDE_DIR}")

    include_directories(${FREEGLUT_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES}
                          ${FREEGLUT_LIBRARY})
  else()
    # Linux linking

    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_directories(${GLEW_LIBRARY_DIRS})
    add_definitions(${GLEW_DEFINITIONS})

    set(FREEGLUT_INCLUDE_DIR /usr/include)
    set(FREEGLUT_LIBRARY /usr/lib/libglut.so)

    include_directories(${FREEGLUT_INCLUDE_DIR})

    if(NOT GLEW_FOUND)
      message(FATAL_ERROR "GLEW não encontrado!")
    endif()

    find_path(DEVIL_INCLUDE_DIR IL/il.h)
    find_library(DEVIL_LIBRARY NAMES IL DevIL)

    if(NOT DEVIL_INCLUDE_DIR OR NOT DEVIL_LIBRARY)
      message(FATAL_ERROR "DevIL not found! Please install it (e.g., sudo apt install libdevil-dev)")
    endif()

    include_directories(${DEVIL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${DEVIL_LIBRARY})

    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES}
                          ${FREEGLUT_LIBRARY} GLEW)
  endif()

endif()

target_include_directories(engine PRIVATE include)
target_link_libraries(engine tinyxml2)
