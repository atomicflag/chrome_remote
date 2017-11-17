cmake_minimum_required(VERSION 3.8)
project(chrome_remote LANGUAGES CXX VERSION 1.1.1)

option(BUILD_DEMO "Build demo app" OFF)
option(USE_MODULES "Use Clang modules" OFF)

include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
include(GNUInstallDirs)

conan_basic_setup(TARGETS)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system iostreams filesystem)

add_library(${PROJECT_NAME} src/chrome_remote.cpp src/browser.cpp)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/chrome_remote DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(BUILD_DEMO)
	add_executable(chrome_remote_demo main.cpp)
	target_link_libraries(chrome_remote_demo PRIVATE ${PROJECT_NAME})
endif()

get_directory_property(TARGETS BUILDSYSTEM_TARGETS)
foreach(TARGET IN LISTS TARGETS)
	set_target_properties(${TARGET} PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED TRUE
		CXX_EXTENSIONS FALSE
		INSTALL_RPATH_USE_LINK_PATH TRUE
	)
	if(USE_MODULES)
		target_compile_options(${TARGET} PRIVATE
			-fmodules -fmodules-cache-path=${PROJECT_BINARY_DIR} -fmodules-ignore-macro=IS_TESTING)
		if(CMAKE_BUILD_TYPE STREQUAL Debug)
			target_compile_options(${TARGET} PRIVATE -gmodules)
		endif()
	endif()
	target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
	target_link_libraries(${TARGET} PRIVATE Boost::boost Boost::iostreams Boost::system Boost::filesystem Threads::Threads ${CONAN_TARGETS})
endforeach()
