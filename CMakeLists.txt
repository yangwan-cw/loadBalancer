cmake_minimum_required(VERSION 3.30)
project(loadBalancer C)

set(CMAKE_C_STANDARD 17)

# Add Lua subdirectory
add_subdirectory(lua)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(loadBalancer src/main.c
        include/types.h
        src/loadbalancer.c
        src/loadbalancer.h
        include/version.h
        src/config/config_parser.c
        src/config/config_parser.h
        src/config/config_validator.c
        src/config/config_validator.h
        src/config/lua_config.c
        src/config/lua_config.h
        include/lua.h)

# Add Lua include directory
target_include_directories(loadBalancer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lua)

# Link Lua library
target_link_libraries(loadBalancer PRIVATE liblua)

if(WIN32)
    target_link_libraries(loadBalancer PRIVATE ws2_32)
endif()
