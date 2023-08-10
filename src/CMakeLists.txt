cmake_minimum_required(VERSION 3.5)

project(
  CoreDNSHome
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network  LinguistTools)
if(Qt6_FOUND)
  find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
endif()

if(NOT APPLE)
  find_package(ZLIB)
endif()

# First check the command line variable VCPKG_TARGET_TRIPLET
if(DEFINED VCPKG_TARGET_TRIPLET)
  set(TRIPLET ${VCPKG_TARGET_TRIPLET})
else()
  # If it's not defined, check the environment variable
  set(TRIPLET $ENV{VCPKG_TARGET_TRIPLET})
endif()

# Now check if TRIPLET ends with "-windows-static"
string(FIND "${TRIPLET}" "-windows-static" POSITION)
if(${POSITION} GREATER_EQUAL 0)
  if(NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  else()
    add_compile_options("/MT$<$<CONFIG:Debug>:d>")
    if(POLICY CMP0091)
      cmake_policy(SET CMP0091 NEW)
    endif()
  endif()
endif()

if(MSVC)
  if(NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "IntelLLVM")
    add_compile_options(/utf-8)
  endif()
  add_compile_options("/EHsc")
  SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"")
endif()
if(WIN32)
  add_compile_definitions(UNICODE)
endif(WIN32)

include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})

set(TS_FILES
    translations/corednshome_zh_CN.ts
    translations/corednshome_zh_TW.ts
    translations/corednshome_zh_HK.ts
    translations/corednshome_da.ts # 丹麦语
    translations/corednshome_fi.ts # 芬兰语
    translations/corednshome_en.ts # 英语
    translations/corednshome_el.ts # 希腊语
    translations/corednshome_he.ts # 希伯来语
    translations/corednshome_hu.ts # 匈牙利语
    translations/corednshome_is.ts # 冰岛语
    translations/corednshome_nb.ts # 挪威语
    translations/corednshome_es.ts # 西班牙语
    translations/corednshome_pt.ts # 葡萄牙语
    translations/corednshome_ro.ts # 罗马尼亚语
    translations/corednshome_sk.ts # 斯洛伐克语
    translations/corednshome_uk.ts # 乌克兰语
    translations/corednshome_ja.ts # 日语
    translations/corednshome_ko.ts # 韩语
    translations/corednshome_fr.ts # 法语
    translations/corednshome_de.ts # 德语
    translations/corednshome_it.ts # 意大利
    translations/corednshome_ru.ts # 俄语
    translations/corednshome_nl.ts # 荷兰
    translations/corednshome_sv.ts # 瑞典
    translations/corednshome_th.ts # 泰国
    translations/corednshome_tr.ts # 土耳其
    translations/corednshome_pl.ts # 波兰
    translations/corednshome_cs.ts # 捷克
)

if(APPLE)
  set_source_files_properties(
    ${TS_FILES}
    PROPERTIES
      OUTPUT_LOCATION
      "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources/translations"
  )
else(APPLE)
  set_source_files_properties(
    ${TS_FILES} PROPERTIES OUTPUT_LOCATION
                           "${CMAKE_CURRENT_BINARY_DIR}/translations")
endif(APPLE)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    donatedialog.cpp
    donatedialog.h
    donatedialog.ui
    networkreplyhelper.cpp
    networkreplyhelper.h
    scopedguard.cpp
    scopedguard.h
    settings.cpp
    settings.h
    corednshome.qrc)

if(WIN32)
  list(APPEND PROJECT_SOURCES corednshome.rc)
endif(WIN32)

if(APPLE)
  list(APPEND PROJECT_SOURCES corednshome.icns)
endif(APPLE)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES}
                        OPTIONS -no-obsolete)
  qt_add_executable(${PROJECT_NAME} ${QM_FILES} MANUAL_FINALIZATION
                    ${PROJECT_SOURCES})
  target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core5Compat)
else()
  qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES}
                         OPTIONS -no-obsolete)
  add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${QM_FILES})
endif()

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

if(ZLIB_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE zlibstatic)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON
                                                 MACOSX_BUNDLE ON)

if(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE
          "-framework AppKit"
          "-framework Carbon"
          "-framework Foundation"
          "-framework ApplicationServices"
          "-framework CoreServices"
          objc
          )
  set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/corednshome.icns"
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources/corednshome.icns)
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES RESOURCE "./corednshome.icns"
               MACOSX_BUNDLE_BUNDLE_NAME "CoreDNSHome"
               MACOSX_BUNDLE_GUI_IDENTIFIER "com.ismisv.corednshome"
               MACOSX_BUNDLE_ICON_FILE "corednshome.icns"
               MACOSX_BUNDLE_INFO_STRING "CoreDNSHome"
               MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING
               ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
endif()

install(
  TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(${PROJECT_NAME})
endif()
