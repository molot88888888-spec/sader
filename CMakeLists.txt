cmake_minimum_required(VERSION 3.20)

project(sader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(ENABLE_COVERAGE "Build with coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage -fprofile-abs-path)
    add_link_options(--coverage -lgcov)
endif()

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(PostgreSQL REQUIRED)

# libpqxx — отдельная библиотека, не входит в FindPostgreSQL
find_library(PQXX_LIBRARY NAMES pqxx-8.0 pqxx REQUIRED)
find_path(PQXX_INCLUDE_DIR NAMES pqxx/pqxx REQUIRED)

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
    src/WorkerRepository.cpp
)

target_include_directories(sader PRIVATE include)
target_link_libraries(sader PRIVATE OpenSSL::Crypto)
target_link_libraries(sader PRIVATE CURL::libcurl)
target_include_directories(sader PRIVATE ${PQXX_INCLUDE_DIR})
target_link_libraries(sader PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(sader PRIVATE ${PQXX_LIBRARY})

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

# ---------- Тест WorkerRepository (PostgreSQL) ----------
add_executable(test_workerrepository
    tests/test_workerrepository.cpp
    src/WorkerRepository.cpp
)

target_include_directories(test_workerrepository PRIVATE include ${PQXX_INCLUDE_DIR})
target_link_libraries(test_workerrepository PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(test_workerrepository PRIVATE ${PQXX_LIBRARY})
