cmake_minimum_required(VERSION 3.16)
project(User)

set(CMAKE_CXX_STANDARD 14)

add_executable(User main.cpp polynomial.h RSA.h crypto.cpp crypto.h)

target_link_libraries(${PROJECT_NAME} gmp gmpxx)
target_link_libraries (${PROJECT_NAME} pthread)