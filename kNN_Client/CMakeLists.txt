cmake_minimum_required(VERSION 3.16)
project(kNN_Client)

set(CMAKE_CXX_STANDARD 11)

add_executable(kNN_Client src/main.cpp src/crypto.h src/crypto.cpp src/SetupPhase.cpp src/SetupPhase.h src/readData.cpp src/readData.h src/MyTime.cpp src/MyTime.h src/KNN_single.cpp src/KNN_single.h src/KNN_client.cpp src/KNN_client.h src/KNN_AC.cpp src/KNN_AC.h src/CSV.cpp src/CSV.h)

include_directories("/usr/lib/x86_64-linux-gnu/")

target_link_libraries(kNN_Client gmp gmpxx)