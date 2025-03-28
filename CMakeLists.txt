cmake_minimum_required(VERSION 3.0)

project(fringe LANGUAGES CXX)


add_executable(server)

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)

target_sources(server PRIVATE ${SRC} main.cpp)

find_package(OpenSSL REQUIRED)
# find_package(UUID REQUIRED)

target_link_libraries(server PRIVATE pq OpenSSL::Crypto uuid)

# target_compile_definitions(server PRIVATE JWT_DISABLE_PICOJSON)

add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:server> ${CMAKE_SOURCE_DIR}/server
    COMMENT "Copying server executable to the top-level directory"
)

