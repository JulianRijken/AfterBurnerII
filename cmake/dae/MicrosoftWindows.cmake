function(MicrosoftWindows_SetupLibs)
    # Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
        cmake_policy(SET CMP0135 NEW)
    endif()

    include(FetchContent)

    # SDL2
    FetchContent_Declare(
        SDL2
        URL     https://www.libsdl.org/release/SDL2-devel-2.28.0-VC.zip
        URL_MD5 196066034d87764b5f4d1c90e56230c6
        OVERRIDE_FIND_PACKAGE
        SOURCE_DIR ${CMAKE_BINARY_DIR}/sdl2-src
    )

    # SDL2_image
    FetchContent_Declare(
        SDL2_image
        URL     https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.6.3-VC.zip
        URL_MD5 55e7eebdcdd0fe38ee0344e3ad280a54
        OVERRIDE_FIND_PACKAGE
        SOURCE_DIR ${CMAKE_BINARY_DIR}/sdl2_image-src
    )

    # SDL2_ttf
    FetchContent_Declare(
        SDL2_ttf
        URL     https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.20.2-VC.zip
        URL_MD5 96ffce885b6e1d2148ea36cedccccf3a
        OVERRIDE_FIND_PACKAGE
        SOURCE_DIR ${CMAKE_BINARY_DIR}/sdl2_ttf-src
    )

    # SDL2_mixer
    FetchContent_Declare(
        SDL2_mixer
        URL     https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.6.3-VC.zip
        URL_MD5 238f376a083f5a9c4c8386f7bc71333b
        OVERRIDE_FIND_PACKAGE
        SOURCE_DIR ${CMAKE_BINARY_DIR}/sdl2_mixer-src
    )
endfunction()

# I died writing this
function(MicrosoftWindows_AddLibs)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    
    target_include_directories(${PROJECT_NAME} PRIVATE ${sdl2_SOURCE_DIR}/include)
    target_include_directories(${PROJECT_NAME} PRIVATE ${sdl2_image_SOURCE_DIR}/include)
    target_include_directories(${PROJECT_NAME} PRIVATE ${sdl2_ttf_SOURCE_DIR}/include)
    target_include_directories(${PROJECT_NAME} PRIVATE ${sdl2_mixer_SOURCE_DIR}/include)

    target_link_libraries(${PROJECT_NAME} ${sdl2_SOURCE_DIR}/lib/x64/SDL2.lib)
    target_link_libraries(${PROJECT_NAME} ${sdl2_SOURCE_DIR}/lib/x64/SDL2main.lib)
    target_link_libraries(${PROJECT_NAME} ${sdl2_image_SOURCE_DIR}/lib/x64/SDL2_image.lib)
    target_link_libraries(${PROJECT_NAME} ${sdl2_ttf_SOURCE_DIR}/lib/x64/SDL2_ttf.lib)
    target_link_libraries(${PROJECT_NAME} ${sdl2_mixer_SOURCE_DIR}/lib/x64/SDL2_mixer.lib)
endfunction()

function(MicrosoftWindows_AddCompileCommands)

    # legacy platform support (manual copy every build)
    add_custom_command(TARGET "${PROJECT_NAME}" PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

    # need to manually copy shared libs on legacy platforms like Microsoft Windows
    add_custom_command(
        TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/sdl2-src/lib/x64/SDL2.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VERBATIM
    )
    add_custom_command(
        TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/sdl2_image-src/lib/x64/SDL2_image.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VERBATIM
    )
    add_custom_command(
        TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/sdl2_ttf-src/lib/x64/SDL2_ttf.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VERBATIM
    )
    add_custom_command(
        TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/sdl2_mixer-src/lib/x64/SDL2_mixer.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VERBATIM
    )
endfunction()