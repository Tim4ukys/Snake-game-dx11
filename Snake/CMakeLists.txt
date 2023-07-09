project("Snake")

set(SOURCE
	"main.cpp"
	"Game.cpp"
	"AudioMan.cpp"
)
set(HEADERS
		"main.hpp"
		"Game.hpp"
		"AudioMan.hpp" Timer.hpp)

add_executable(${PROJECT_NAME} WIN32 ${SOURCE} ${HEADERS})

find_package(sparsehash CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} SNAKE::CORE IMGUI::IMGUI sparsehash::sparsehash)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17        
)