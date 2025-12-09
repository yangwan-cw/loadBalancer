cmake_minimum_required(VERSION 3.30)
project(loadBalancer C)

set(CMAKE_C_STANDARD 17)

# Add Lua subdirectory
add_subdirectory(lua)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(loadBalancer
        src/main.c
        include/types.h
        include/version.h
        include/lua.h
        src/core/loadbalancer.c
        src/core/loadbalancer.h
        src/config/lua_config.c
        src/config/lua_config.h
        src/health_check/health_check.c
        src/health_check/health_check.h
        src/algorithm/round_robin.c
        src/algorithm/round_robin.h
        src/networking/server.c
        src/networking/server.h
        src/utils/args.c
        src/utils/args.h
        src/utils/logger.c
        src/utils/logger.h
        src/utils/display.c
        src/utils/display.h
        src/utils/signal_handler.c
        src/utils/signal_handler.h
        src/utils/usage.c
        src/utils/usage.h)

# Add Lua include directory
target_include_directories(loadBalancer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lua)

# Link Lua library
target_link_libraries(loadBalancer PRIVATE liblua)

if(WIN32)
    target_link_libraries(loadBalancer PRIVATE ws2_32)
endif()
