
add_library(glad INTERFACE)
target_include_directories(glad INTERFACE  include/)
target_sources(glad INTERFACE "${CMAKE_CURRENT_LIST_DIR}/src/glad.c")
#set(glad_include "${CMAKE_CURRENT_LIST_DIR}/include/")
