cmake_minimum_required(VERSION 3.16)

project(Game2024)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# Set executable name
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Define the executable or library
add_executable(${EXECUTABLE_NAME})

# Adding sources
target_sources(${EXECUTABLE_NAME} PRIVATE src/game.cpp src/SDL_FontCache.c)

# Use C++20
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_20)

# Emscripten specific settings
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "-sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sSDL2_IMAGE_FORMATS='png'"
        LINK_FLAGS "-sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sSDL2_IMAGE_FORMATS='png' -sINITIAL_MEMORY=64mb -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=1gb --preload-file ../assets"
    )
    
    add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/index.html ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/index.html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/serve.py ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/serve.py
        DEPENDS ${EXECUTABLE_NAME}
        COMMENT "Copying HTML and Flask server files to build directory"
    )
else()
    if (WIN32)
        target_compile_definitions(${EXECUTABLE_NAME} PRIVATE "PLATFORM_WINDOWS")
        set(CMAKE_CXX_STANDARD 20)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")

        # Find the SDL2 package
        set(SDL2_PATH "C:/SDL2-2.30.6-mingw/x86_64-w64-mingw32")
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
        include_directories(${SDL2_PATH}/include/SDL2)
        target_link_libraries(${EXECUTABLE_NAME} 
            PRIVATE 
            mingw32
            "${SDL2_PATH}/lib/libSDL2main.a" 
            "${SDL2_PATH}/lib/libSDL2.a" 
            "${SDL2_PATH}/lib/libSDL2_ttf.a"
            "${SDL2_PATH}/lib/libSDL2_mixer.a"
            "${SDL2_PATH}/lib/libSDL2_image.a"
            setupapi
            imm32
            version
            winmm
            rpcrt4
        )

        target_sources(${EXECUTABLE_NAME} PRIVATE resources.rc)

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_definitions(${EXECUTABLE_NAME} PRIVATE DEBUG=1)
        elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_definitions(${EXECUTABLE_NAME} PRIVATE DEBUG=0)
        endif()
        
        add_custom_target(copy_resources ALL
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/assets
            DEPENDS ${EXECUTABLE_NAME}
            COMMENT "Copying assets directory to build directory"
        )
    endif()
endif()

# Set the project as the startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXECUTABLE_NAME})

# Installation settings
install(TARGETS ${EXECUTABLE_NAME} BUNDLE DESTINATION ./install)
