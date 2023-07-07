project("core-engine")

set(PUBLIC_HEADER
	"include/Audio.hpp"
    "include/Graphics.hpp"
    "include/Window.hpp"
)
set(SOURCE
	"src/Audio.cpp"
    "src/Graphics.cpp"
    "src/Window.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCE} ${PUBLIC_HEADER})
add_library(SNAKE::CORE ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17        
)

set(DIRECTX_PATH $ENV{DXSDK_DIR})

find_package(directxmath CONFIG REQUIRED)
find_package(asmjit CONFIG REQUIRED)
find_package(Boost REQUIRED)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PUBLIC "${DIRECTX_PATH}Include/"
    PUBLIC ${Boost_INCLUDE_DIR}
)

if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8") 
    target_link_directories(${PROJECT_NAME}
        PUBLIC "${DIRECTX_PATH}Lib/x64/"
    )
else()
    target_link_directories(${PROJECT_NAME}
        PUBLIC "${DIRECTX_PATH}Lib/x86/"
    )
endif()

target_link_libraries(${PROJECT_NAME} 
    PUBLIC "d3d11.lib"
    PUBLIC "d3dx11.lib"
    PUBLIC asmjit::asmjit
    PUBLIC Microsoft::DirectXMath
)
