cmake_minimum_required(VERSION 3.15)
project(luasteam)

# Add FetchContent
include(FetchContent)

# Fetch LuaJIT
FetchContent_Declare(
    luajit
    GIT_REPOSITORY https://github.com/WohlSoft/LuaJIT.git
    GIT_TAG v2.1
)
FetchContent_MakeAvailable(luajit)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
file(GLOB SOURCE_FILES "src/*.cpp")

# Platform-specific settings
if(WIN32)
    set(STEAM_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sdk/redistributable_bin")
    
    # Disable specific warnings
    add_compile_options(/wd4127 /wd4996)
    
    # Add library as a shared library
    add_library(luasteam SHARED ${SOURCE_FILES})
    
    # Set output name
    set_target_properties(luasteam PROPERTIES 
        OUTPUT_NAME "luasteam"
        SUFFIX ".dll"
    )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64-bit build
        set(STEAM_LIB "${STEAM_SDK_PATH}/win64/steam_api64.lib")
    else()
        # 32-bit build
        set(STEAM_LIB "${STEAM_SDK_PATH}/steam_api.lib")
    endif()

elseif(APPLE)
    set(STEAM_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sdk/redistributable_bin")
    set(LUAJIT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third-party")
    
    # Enable universal binary building
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    
    add_library(luasteam SHARED ${SOURCE_FILES})
    
    set_target_properties(luasteam PROPERTIES 
        OUTPUT_NAME "luasteam"
        SUFFIX ".so"
    )
    
    # Set minimum macOS version
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
    
    # You'll need universal versions of these libraries
    set(STEAM_LIB "${STEAM_SDK_PATH}/osx/libsteam_api.dylib")
    
    # Configure LuaJIT for universal build
    set(CMAKE_OSX_ARCHITECTURES_LUAJIT "arm64;x86_64")
    
else() # Linux
    set(STEAM_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sdk/redistributable_bin")
    
    add_library(luasteam SHARED ${SOURCE_FILES})
    
    set_target_properties(luasteam PROPERTIES 
        OUTPUT_NAME "luasteam"
        SUFFIX ".so"
    )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(STEAM_LIB "${STEAM_SDK_PATH}/linux64/libsteam_api.so")
    else()
        set(STEAM_LIB "${STEAM_SDK_PATH}/linux32/libsteam_api.so")
    endif()
endif()

# Common settings for all platforms
target_compile_definitions(luasteam PRIVATE -DWIN32_LEAN_AND_MEAN)

# Compiler-specific options
if(MSVC)
    target_compile_options(luasteam PRIVATE 
        /W4                  # Warning level 4
        /wd4996             # Disable deprecation warnings
        /wd4127             # Disable conditional expression is constant
        /wd4324             # Disable structure padding warning
    )
else()
    target_compile_options(luasteam PRIVATE 
        -Wno-invalid-offsetof 
        -Wall
    )
endif()

# Include directories
target_include_directories(luasteam PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/LuaJIT/src"
)

if(WIN32)
    # For Windows, link against the LuaJIT we just built
    add_dependencies(luasteam libluajit)
    target_link_libraries(luasteam PRIVATE
        "${STEAM_LIB}"
        libluajit
    )
elseif(APPLE)
    # For macOS, we need universal binary versions of the libraries
    target_link_libraries(luasteam PRIVATE
        "${STEAM_LIB}"
        "${LUAJIT_PATH}/lib/libluajit-5.1.a"
    )
else()
    # For Linux, link against system LuaJIT
    target_link_libraries(luasteam PRIVATE
        "${STEAM_LIB}"
        "luajit-5.1"
    )
endif()

# Installation rules
install(TARGETS luasteam
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)