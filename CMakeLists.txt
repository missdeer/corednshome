cmake_minimum_required(VERSION 3.15)
project(corednshome VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILECOMMANDS ON)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    set(BUILD_TYPE Debug)
    add_compile_definitions(LOGS_ENABLED)
ELSE(CMAKE_BUILD_TYPE MATCHES Debug)
    set(BUILD_TYPE Release)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

add_subdirectory(3rdparty)
add_subdirectory(src)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${ProjectName} PRIVATE
        Qt::Core5Compat
        )
    qt_finalize_project()
endif()
    