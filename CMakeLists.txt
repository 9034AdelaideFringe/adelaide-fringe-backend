cmake_minimum_required(VERSION 3.5)

project(fringe LANGUAGES CXX)



# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release CACHE STRING "set build type to Release" FORCE)    
endif(NOT CMAKE_BUILD_TYPE)


add_executable(server)

target_include_directories(server PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)

target_sources(server PRIVATE ${SRC} main.cpp)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
# find_package(UUID REQUIRED)



target_link_libraries(server PRIVATE pq OpenSSL::Crypto OpenSSL::SSL uuid pqxx Threads::Threads)

target_compile_definitions(server PRIVATE 
    CROW_STATIC_DIRECTORY=\"images/\"
    CROW_STATIC_ENDPOINT=\"/images/<path>\"
)

# target_compile_definitions(server PRIVATE JWT_DISABLE_PICOJSON)

add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:server> ${CMAKE_SOURCE_DIR}/server
    COMMENT "Copying server executable to the top-level directory"
)

target_compile_definitions(server PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
