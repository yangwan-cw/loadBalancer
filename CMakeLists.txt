cmake_minimum_required(VERSION 3.30)
project(loadBalancer C)

set(CMAKE_C_STANDARD 17)

add_executable(loadBalancer src/main.c
        include/types.h
        src/loadbalancer.c
        src/loadbalancer.h
        include/version.h)

if(WIN32)
    target_link_libraries(loadBalancer ws2_32)
endif()