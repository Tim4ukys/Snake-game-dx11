project(imgui)

file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCE} ${PUBLIC_HEADER})
add_library(IMGUI::IMGUI ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
