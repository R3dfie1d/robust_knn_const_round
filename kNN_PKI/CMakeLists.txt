cmake_minimum_required(VERSION 3.16)
project(kNN_PKI)

set(CMAKE_CXX_STANDARD 11)

add_executable(kNN_PKI
  src/main.cpp
  src/crypto.cpp
  src/SetupPhase.cpp
  src/MyTime.cpp
)

include_directories("/usr/lib/x86_64-linux-gnu/")

target_link_libraries(kNN_PKI gmp gmpxx)
