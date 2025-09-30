cmake_minimum_required(VERSION 3.31)

project(Lua547)
set(CMAKE_C_STANDARD 99)

set(LUA_SOURCES
    src/lapi.c
    src/lcode.c
    src/lctype.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/llex.c
    src/lmem.c
    src/lobject.c
    src/lopcodes.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/ltable.c
    src/ltm.c
    src/lundump.c
    src/lvm.c
    src/lzio.c
    src/lauxlib.c
    src/lbaselib.c
    src/lcorolib.c
    src/ldblib.c
    src/liolib.c
    src/lmathlib.c
    src/loadlib.c
    src/loslib.c
    src/lstrlib.c
    src/ltablib.c
    src/lutf8lib.c
    src/linit.c
)

add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3)

if(UNIX AND NOT APPLE) # Linux
    target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
    target_link_libraries(lua PRIVATE dl m)
elseif(APPLE) # macOS
    target_compile_definitions(lua PRIVATE LUA_USE_MACOSX)
    target_link_libraries(lua PRIVATE m)
elseif(WIN32) # Windows
    # MinGW target, but we don't use mingw
endif()

# Add lua.hpp for C++ compatibility
add_library(lua_cpp INTERFACE)
target_include_directories(lua_cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(lua_cpp INTERFACE lua)