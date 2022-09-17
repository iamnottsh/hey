cmake_minimum_required(VERSION 3.23)
project(hey)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost 1.79.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
if (WIN32)
    link_libraries(wsock32 ws2_32)
endif ()

find_package(OpenSSL 3.0.5 REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
link_libraries(${OPENSSL_LIBRARIES})
file(
        COPY ${OPENSSL_INCLUDE_DIR}/../libcrypto-3-x64.dll ${OPENSSL_INCLUDE_DIR}/../libssl-3-x64.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(hey main.cpp partial.h)
