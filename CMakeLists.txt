cmake_minimum_required(VERSION 3.18)

# Change project name here
set(PROJECT_NAME "AfterBurnerII")

# Web options
set(ASPECT_RATIO "320/224")
set(CLICK_TO_START ON) # a click is needed to activate sound!

# --------------------------------------
# no need to change anything after this!
# --------------------------------------

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/dae)

# Microsoft Windows support
if(WIN32)
    include(MicrosoftWindows)

    # on legacy platforms, we must download the libraries because there is no package manager :(
    MicrosoftWindows_SetupLibs()
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g -fPIC")
endif()

# enable C++20
set(CMAKE_CXX_STANDARD 20)

# for LSP support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("${PROJECT_NAME}")

file(GLOB_RECURSE DIR_SRC src/*.cpp engine/*.cpp)
add_executable("${PROJECT_NAME}" ${DIR_SRC})

if(EMSCRIPTEN)
    include(Emscripten)

elseif(WIN32)
    # legacy platforms require very painful library setups
    MicrosoftWindows_AddLibs()

else()
    find_package(SDL2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2::Main)

    find_package(SDL2_image REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2::Image)

    find_package(SDL2_ttf REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2::TTF)

    find_package(SDL2_mixer REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2::Mixer)

    find_package(OpenGL)
    include_directories(${PROJECT_NAME} ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
endif()

# include directories
include_directories(engine)
include_directories(src)

# disable warnings for external libraries
file(GLOB_RECURSE EXTERNAL_DIR "src/**")
set_source_files_properties(
  ${EXTERNAL_DIR}
  PROPERTIES
  COMPILE_FLAGS "-w"
)

# symlink resources folder on supported platforms (sorry, Microsoft Windows!)
if(NOT WIN32)
    add_custom_command(TARGET "${PROJECT_NAME}" PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
else()
    MicrosoftWindows_AddCompileCommands()
endif()
