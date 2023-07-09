cmake_minimum_required(VERSION 3.23)
project("SnakeDX11")

add_subdirectory(core)
add_subdirectory(imgui)
add_subdirectory(Snake)

add_custom_target(resources_sync
    COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:Snake>
    VERBATIM)

add_dependencies(Snake resources_sync)
