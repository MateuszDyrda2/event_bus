cmake_minimum_required(VERSION 3.22)

set(MDEventBusVersionMajor 0)
set(MDEventBusVersionMinor 1)
set(MDEventBusVersionPatch 0)
set(MDEventBusVersion ${MDEventBusVersionMajor}.${MDEventBusVersionMinor}.${MDEventBusVersionPatch})

option(md_BUILD_TESTS "Build tests" ON)

project(MDEventBus VERSION ${MDEventBusVersion})

add_library(md_event_bus INTERFACE) 
add_library(md::event_bus ALIAS md_event_bus)
target_include_directories(md_event_bus INTERFACE include/)

set_property(
	TARGET md_event_bus 
	PROPERTY CXX_STANDARD 17
)

if(${md_BUILD_TESTS})
	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY 	https://github.com/google/googletest.git
		GIT_TAG 		release-1.11.0
	)
	FetchContent_MakeAvailable(googletest)
	enable_testing()
	add_executable(
		eventbus_test
		test/eventbus_test.cpp
	)
	target_link_libraries(
		eventbus_test
		gtest_main
		md::event_bus
	)
	include(GoogleTest)
	gtest_discover_tests(eventbus_test)
endif()