cmake_minimum_required(VERSION 3.21)

project(
	ObjectImpactFramework
	VERSION 1.6.7
	LANGUAGES CXX
)

if (MSVC)
    add_compile_options(/bigobj)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(XSEPlugin)
