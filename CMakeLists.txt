cmake_minimum_required(VERSION 3.20)
project(boost_asio_sample)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(boost_asio_client client/main.cpp)
target_link_libraries(boost_asio_client ${CONAN_LIBS})

add_executable(boost_asio_server server/main.cpp)
target_link_libraries(boost_asio_server ${CONAN_LIBS})