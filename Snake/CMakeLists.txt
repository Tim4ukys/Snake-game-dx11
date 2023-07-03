project("Snake")

set(SOURCE
	"main.cpp"
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCE})

target_link_libraries(${PROJECT_NAME} SNAKE::CORE)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17        
)