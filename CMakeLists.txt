cmake_minimum_required(VERSION 3.5)
project(Boron LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fno-exceptions -fno-rtti")

set(Boron_Includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

set(Boron_LibSources
    lib/Buffers.h
    lib/Bytes.h
    lib/Deserializable.h
    lib/Serializable.h
    lib/cbor/Allocators.h
    lib/cbor/DataModel.h
    lib/cbor/DataModelBase.cpp
    lib/cbor/DataModelBase.h
    lib/cbor/Decoding.cpp
    lib/cbor/Decoding.h
    lib/cbor/Decoder.cpp
    lib/cbor/Decoder.h
    lib/cbor/Encoder.cpp
    lib/cbor/Encoder.h
    lib/cbor/Encoding.cpp
    lib/cbor/Encoding.h
    lib/cbor/Errors.h
    lib/cbor/Header.h
    lib/cbor/Item.cpp
    lib/cbor/Item.h
    lib/cbor/Tags.h
    lib/cbor/Types.h
    lib/cbor/ValueBuilder.h
    lib/json/Decoder.h
    lib/json/Encoder.cpp
    lib/json/Encoder.h
    lib/json/Types.h
    )

add_library(boron ${Boron_LibSources})
target_include_directories(boron PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(Boron_ToolSources
    src/Functions.cpp
    src/Functions.h
    src/main.cpp
    )

add_executable(borontool ${Boron_ToolSources})
target_include_directories(borontool PUBLIC ${Boron_Includes})
target_link_libraries(borontool PUBLIC boron)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(TEST_SOURCES
            unittests/test_cbor.cpp
            unittests/test_encoding.cpp
            unittests/test_serialization.cpp)

add_executable(testsuite ${TEST_SOURCES} ${Boron_Sources})
target_include_directories(testsuite PUBLIC ${Boron_Includes})
target_link_libraries(testsuite GTest::gtest_main boron)

include(GoogleTest)
gtest_discover_tests(testsuite)