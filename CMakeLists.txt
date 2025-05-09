cmake_minimum_required(VERSION 3.12.0)
project("lol")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(./deps/glad)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)


add_executable(main ./src/main.cpp)
target_link_libraries(main glm::glm glfw glad)
target_include_directories(main PUBLIC glad_include)

add_executable(one ./src/normals.cpp)
target_link_libraries(one glm::glm)


#file(GLOB_RECURSE source_list "${CMAKE_SOURCE_DIR}/resources/*")
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/resources/shaders/main.vert
            ${CMAKE_SOURCE_DIR}/resources/shaders/main.frag
            ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders/
)