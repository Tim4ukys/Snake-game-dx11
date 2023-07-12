project("Snake")

set(SOURCE
	"main.cpp"
	"Game.cpp"
	"AudioMan.cpp"
)
set(HEADERS
		"main.hpp"
		"Game.hpp"
		"AudioMan.hpp" Timer.hpp Language.hpp Config.hpp)

add_executable(${PROJECT_NAME} WIN32 ${SOURCE} ${HEADERS})

find_package(sparsehash CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} SNAKE::CORE IMGUI::IMGUI sparsehash::sparsehash ${LUA_LIBRARIES} sol2)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17        
)