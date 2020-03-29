cmake_minimum_required(VERSION 3.5.1)

# Project name and version
project(restserver VERSION 0.5.1)

include(IdentifyOS.cmake)
set(ODB_USE_FILE "${CMAKE_CURRENT_LIST_DIR}/UseODB.cmake")

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} 
                src/main.cpp 
                src/keyvaluedbengine.cpp
                src/restdbserviceserver.cpp
                src/user-odb.cxx
                src/settings.cpp)

add_definitions(-D_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR -DDATABASE_MYSQL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer -g")

set(MODEL_PROJECT_INCLUDE_DIR "inc")

find_package(cpprestsdk REQUIRED)

set(BOOST_COMPONENTS system chrono thread)
find_package(Boost 1.65 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
find_package(Threads)

find_path(LibCrypto_INCLUDE_DIR
    NAMES openssl/crypto.h
    HINTS
        ${CMAKE_PREFIX_PATH}/include 
        ${CMAKE_INSTALL_PREFIX}/include
    )
find_library(LibCrypto_SHARED_LIBRARY
    NAMES libcrypto.so libcrypto.dylib
    HINTS
    ${CMAKE_PREFIX_PATH}/build/crypto
    ${CMAKE_PREFIX_PATH}/build
    ${CMAKE_PREFIX_PATH}
    ${CMAKE_PREFIX_PATH}/lib64
    ${CMAKE_PREFIX_PATH}/lib 
    ${CMAKE_INSTALL_PREFIX}/build/crypto
    ${CMAKE_INSTALL_PREFIX}/build
    ${CMAKE_INSTALL_PREFIX}
    ${CMAKE_INSTALL_PREFIX}/lib64
    ${CMAKE_INSTALL_PREFIX}/lib
    )

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

if( Log4cxx_DIR )
  find_package( Log4cxx NO_MODULE )
elseif( NOT Log4cxx_FOUND )
  message(STATUS "Searching for log4cxx/logger.h")
  find_path( Log4cxx_INCLUDE_DIR log4cxx/logger.h )

  message(STATUS "Searching for libLog4cxx")
  find_library( Log4cxx_LIBRARY log4cxx )

  include( FindPackageHandleStandardArgs )
  FIND_PACKAGE_HANDLE_STANDARD_ARGS( Log4cxx Log4cxx_INCLUDE_DIR Log4cxx_LIBRARY )
  if( LOG4CXX_FOUND )
    set( Log4cxx_FOUND TRUE )
  endif()
endif()

find_package(PkgConfig QUIET)

function(find_odb_api component)
	string(TOUPPER "${component}" component_u)
	set(ODB_${component_u}_FOUND FALSE PARENT_SCOPE)

	pkg_check_modules(PC_ODB_${component} QUIET "libodb-${component}")

	find_path(ODB_${component}_INCLUDE_DIR
		NAMES odb/${component}/version.hxx
		HINTS
			${ODB_LIBODB_INCLUDE_DIRS}
			${PC_ODB_${component}_INCLUDE_DIRS})

	find_library(ODB_${component}_LIBRARY
		NAMES odb-${component} libodb-${component}
		HINTS
			${ODB_LIBRARY_PATH}
			${PC_ODB_${component}_LIBRARY_DIRS})

	set(ODB_${component_u}_INCLUDE_DIRS ${ODB_${component}_INCLUDE_DIR} CACHE STRING "ODB ${component} include dirs")
	set(ODB_${component_u}_LIBRARIES ${ODB_${component}_LIBRARY} CACHE STRING "ODB ${component} libraries")

	mark_as_advanced(ODB_${component}_INCLUDE_DIR ODB_${component}_LIBRARY)

	if(ODB_${component_u}_INCLUDE_DIRS AND ODB_${component_u}_LIBRARIES)
		set(ODB_${component_u}_FOUND TRUE PARENT_SCOPE)
		set(ODB_${component}_FOUND TRUE PARENT_SCOPE)

		list(APPEND ODB_INCLUDE_DIRS ${ODB_${component_u}_INCLUDE_DIRS})
		list(REMOVE_DUPLICATES ODB_INCLUDE_DIRS)
		set(ODB_INCLUDE_DIRS ${ODB_INCLUDE_DIRS} PARENT_SCOPE)

		list(APPEND ODB_LIBRARIES ${ODB_${component_u}_LIBRARIES})
		list(REMOVE_DUPLICATES ODB_LIBRARIES)
		set(ODB_LIBRARIES ${ODB_LIBRARIES} PARENT_SCOPE)
	endif()
endfunction()

pkg_check_modules(PC_LIBODB QUIET "libodb")

set(ODB_LIBRARY_PATH "/usr/local/lib/" CACHE STRING "Common library search hint for all ODB libs")

find_path(libodb_INCLUDE_DIR
	NAMES odb/version.hxx
	HINTS
		${PC_LIBODB_INCLUDE_DIRS})

find_library(libodb_LIBRARY
	NAMES odb libodb mysql
	HINTS
		${ODB_LIBRARY_PATH}
		${PC_LIBODB_LIBRARY_DIRS})

find_program(odb_BIN
	NAMES odb
	HINTS
		${libodb_INCLUDE_DIR}/../bin)

set(ODB_LIBODB_INCLUDE_DIRS ${libodb_INCLUDE_DIR} CACHE STRING "ODB libodb include dirs")
set(ODB_LIBODB_LIBRARIES ${libodb_LIBRARY} CACHE STRING "ODB libodb library")
set(ODB_EXECUTABLE ${odb_BIN} CACHE STRING "ODB executable")

mark_as_advanced(libodb_INCLUDE_DIR libodb_LIBRARY odb_BIN)

if(ODB_LIBODB_INCLUDE_DIRS AND ODB_LIBODB_LIBRARIES)
	set(ODB_LIBODB_FOUND TRUE)
endif()

set(ODB_FIND_COMPONENTS mysql)

foreach(component ${ODB_FIND_COMPONENTS})
	find_odb_api(${component})
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ODB
	FOUND_VAR ODB_FOUND
	REQUIRED_VARS ODB_EXECUTABLE ODB_LIBODB_FOUND
	HANDLE_COMPONENTS)

set(CPP_REST_SDK_PATH cpprestsdk::cpprest)

if(ODB_LIBODB_FOUND)
    set(ODB_INCLUDE_DIRS ${ODB_LIBODB_INCLUDE_DIRS})
    set(ODB_LIBRARIES ${ODB_LIBODB_LIBRARIES} ${ODB_MYSQL_LIBRARIES})
endif()

if(Boost_FOUND)
    set(BOOST_SOURCE_PATH ${Boost_INCLUDE_DIRS})
    set(BOOST_LIBRARY_PATH ${Boost_LIBRARIES})
endif()

if(Log4cxx_FOUND)
    set(LOG4CXX_SOURCE_PATH ${Log4cxx_INCLUDE_DIR})
    set(LOG4CXX_LIBRARY_PATH ${Log4cxx_LIBRARY})
endif()

set(INCLUDE_PROJECT_SOURCES ${BOOST_SOURCE_PATH} ${OPENSSL_INCLUDE_DIR} ${LOG4CXX_SOURCE_PATH} ${ODB_INCLUDE_DIRS})

set(LIBRARIES_SEARCH_PATHS ${BOOST_LIBRARY_PATH} ${OPENSSL_CRYPTO_LIBRARY} ${CPP_REST_SDK_PATH} 
                ${OPENSSL_SSL_LIBRARY} ${ODB_LIBRARIES} ${LOG4CXX_LIBRARY_PATH})

# link_directories(${MODEL_PROJECT_INCLUDE_DIR})

include_directories(${MODEL_PROJECT_INCLUDE_DIR} ${INCLUDE_PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_SEARCH_PATHS})
