cmake_minimum_required(VERSION 3.10)
project(liblua C)

# Lua 核心源文件
set(LUA_CORE_SRC
    lapi.c
    lcode.c
    lctype.c
    ldebug.c
    ldo.c
    ldump.c
    lfunc.c
    lgc.c
    llex.c
    lmem.c
    lobject.c
    lopcodes.c
    lparser.c
    lstate.c
    lstring.c
    ltable.c
    ltm.c
    lundump.c
    lvm.c
    lzio.c
)

# Lua 库源文件
set(LUA_LIB_SRC
    lauxlib.c
    lbaselib.c
    lcorolib.c
    ldblib.c
    liolib.c
    lmathlib.c
    loadlib.c
    loslib.c
    lstrlib.c
    ltablib.c
    lutf8lib.c
    linit.c
)

# 创建 Lua 静态库
add_library(liblua STATIC ${LUA_CORE_SRC} ${LUA_LIB_SRC})

# 设置包含目录
target_include_directories(liblua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 设置编译选项
if(UNIX)
    target_compile_definitions(liblua PRIVATE LUA_USE_POSIX)
    target_link_libraries(liblua PRIVATE m dl)
elseif(WIN32)
    target_compile_definitions(liblua PRIVATE LUA_USE_WINDOWS)
endif()

# 设置输出名称
set_target_properties(liblua PROPERTIES OUTPUT_NAME lua)
