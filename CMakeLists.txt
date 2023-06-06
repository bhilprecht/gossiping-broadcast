cmake_minimum_required(VERSION 3.15)
project(gossiping VERSION 0.0.1 LANGUAGES CXX)

include_directories(src)

find_package(nlohmann_json REQUIRED)
find_package(boost REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/message.cpp src/node.cpp)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} boost::boost)
