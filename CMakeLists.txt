cmake_minimum_required(VERSION 3.5)
project(Boron LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -fno-exceptions -fno-rtti")

set(Boron_Includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

set(Boron_Sources
    src/Boron.h
    src/Buffers.h
    src/Bytes.h
    src/Deserializable.h
    src/Serializable.h
    src/cbor/Allocators.h
    src/cbor/DataModel.h
    src/cbor/DataModelBase.cpp
    src/cbor/DataModelBase.h
    src/cbor/Decoder.cpp
    src/cbor/Decoder.h
    src/cbor/Encoder.cpp
    src/cbor/Encoder.h
    src/cbor/Item.cpp
    src/cbor/Item.h
    src/cbor/Tags.h
    src/cbor/Types.h
    src/cbor/ValueBuilder.h
    )

add_library(boron ${Boron_Sources})
target_include_directories(boron PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

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
            unittests/test_serialization.cpp)

add_executable(testsuite ${TEST_SOURCES} ${Boron_Sources})
target_include_directories(testsuite PUBLIC ${Boron_Includes})
target_link_libraries(testsuite GTest::gtest_main boron)

include(GoogleTest)
gtest_discover_tests(testsuite)