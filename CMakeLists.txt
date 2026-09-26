cmake_minimum_required(VERSION 3.20)

project(sader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# ---------- Основная программа ----------
add_executable(sader
    src/main.cpp
    src/CommandParser.cpp
    src/Executor.cpp
    src/HashWorker.cpp
    src/FileWorker.cpp
    src/TextWorker.cpp
    src/ModelWorker.cpp
    src/CsvWorker.cpp
    src/StringUtils.cpp
    src/JsonWorker.cpp
    src/ProcessWorker.cpp
    src/HttpWorker.cpp
)

target_include_directories(sader PRIVATE include)
target_link_libraries(sader PRIVATE OpenSSL::Crypto)
target_link_libraries(sader PRIVATE CURL::libcurl)

# ---------- Тест TextWorker ----------
add_executable(test_textworker
    tests/test_textworker.cpp
    src/TextWorker.cpp
)

target_include_directories(test_textworker PRIVATE include)

# ---------- Тест FileWorker ----------
add_executable(test_fileworker
    tests/test_fileworker.cpp
    src/FileWorker.cpp
)

target_include_directories(test_fileworker PRIVATE include)

# ---------- Тест HashWorker ----------
add_executable(test_hashworker
    tests/test_hashworker.cpp
    src/HashWorker.cpp
)

target_include_directories(test_hashworker PRIVATE include)
target_link_libraries(test_hashworker PRIVATE OpenSSL::Crypto)

add_executable(test_modelworker
    tests/test_modelworker.cpp
    src/ModelWorker.cpp
)

target_include_directories(test_modelworker PRIVATE include)

add_executable(test_csvworker
    tests/test_csvworker.cpp
    src/CsvWorker.cpp
)

target_include_directories(test_csvworker PRIVATE include)

add_executable(test_jsonworker
    tests/test_jsonworker.cpp
    src/JsonWorker.cpp
)

target_include_directories(test_jsonworker PRIVATE include)

add_executable(test_processworker
    tests/test_processworker.cpp
    src/ProcessWorker.cpp
)

target_include_directories(test_processworker PRIVATE include)

add_executable(test_httpworker
    tests/test_httpworker.cpp
    src/HttpWorker.cpp
)

target_include_directories(test_httpworker PRIVATE include)
target_link_libraries(test_httpworker PRIVATE CURL::libcurl)
