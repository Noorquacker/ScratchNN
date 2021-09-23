cmake_minimum_required(VERSION 3.5)

project(ScratchNN LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(project_sources
	main.cpp
	idx.cpp
	stupiddnn.cpp
	neuron.cpp
	inputneuron.cpp
	dnntrainer.cpp
	quacktext.cpp
)

add_executable(${PROJECT_NAME} ${project_sources})
target_link_libraries(ScratchNN SDL2 m pthread SDL_ttf)
