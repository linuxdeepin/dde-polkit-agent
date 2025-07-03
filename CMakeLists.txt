cmake_minimum_required(VERSION 3.16)

set(BIN_NAME dde-polkit-agent)

project(${BIN_NAME} CXX)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

option(USE_DEEPIN_POLKIT "enable deepin polkit" OFF)

message(STATUS "TO USE Deepin_Polkit : ${USE_DEEPIN_POLKIT}")


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -O2")
endif()

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

set(QT_VERSION_MAJOR 6)
set(DTK_VERSION_MAJOR 6)

include(GNUInstallDirs)

# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} ${REQUIRED_QT_VERSION} REQUIRED Core Widgets DBus Concurrent LinguistTools)
find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED Widget Core Tools)
find_package(DDEShell REQUIRED)

pkg_check_modules(Polkit-qt6 REQUIRED polkit-qt6-1)

qt_add_dbus_adaptor(DBUS_INTERFACES ${CMAKE_SOURCE_DIR}/dbus/adapter/org.deepin.dde.Polkit1.AuthAgent.xml policykitlistener.h PolicyKitListener)

if(${USE_DEEPIN_POLKIT})
  add_definitions(-DUSE_DEEPIN_POLKIT)
endif()

dtk_add_dbus_interfaces(DBUS_INTERFACES ${CMAKE_SOURCE_DIR}/dbus/org.deepin.dde.Accounts1.xml)
dtk_add_dbus_interface(DBUS_INTERFACES ${CMAKE_SOURCE_DIR}/dbus/org.deepin.dde.Accounts1.User.xml accounts1userinterface)

file(GLOB DDE_POLKIT_AGENT_SCRS "*.h" "*.cpp")

add_executable(${BIN_NAME}
    ${DBUS_INTERFACES}
    ${DDE_POLKIT_AGENT_SCRS}
    images.qrc
)

target_include_directories(${BIN_NAME} PUBLIC
    Dtk${DTK_VERSION_MAJOR}::Widget
    Dtk${DTK_VERSION_MAJOR}::Core
    ${Polkit-qt6_INCLUDE_DIRS}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Concurrent
)

target_link_libraries(${BIN_NAME} PRIVATE
    Dtk${DTK_VERSION_MAJOR}::Widget
    Dtk${DTK_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Concurrent
    Dde::Shell
    ${Polkit-qt6_LIBRARIES}
)

# can use qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES xx xx) from Qt6.7
set(TRANSLATION_LANGUAGES
    af am_ET ar ast az bg bn bo ca cs da de el en_AU en_GB eo es et fa fi fr gl_ES
    he hi_IN hr hu hy id it ja ko ku_IQ lo lt ml mn ms nb ne nl pl pt_BR pt ro ru
    si sk sl sq sr sv sw ta tr pam ug uk vi zh_CN zh_HK zh_TW
)

set(TRANSLATION_FILES "")
list(APPEND TRANSLATION_FILES "${PROJECT_SOURCE_DIR}/translations/dde-polkit-agent.ts")

foreach(LANG ${TRANSLATION_LANGUAGES})
    list(APPEND TRANSLATION_FILES "${PROJECT_SOURCE_DIR}/translations/dde-polkit-agent_${LANG}.ts")
endforeach()

# can generate ts and qm file
qt_add_translations(${BIN_NAME}
    TS_FILES ${TRANSLATION_FILES}
    SOURCES ${DDE_POLKIT_AGENT_SCRS}
    QM_FILES_OUTPUT_VARIABLE TR_QM_FILES
)

## qm files
install(FILES ${TR_QM_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/${BIN_NAME}/translations)

## header files
file(GLOB HEADER_FILES "agent-extension-proxy.h" "agent-extension.h")
install(FILES ${HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dpa)

# bin
install(TARGETS ${BIN_NAME} DESTINATION lib/polkit-1-dde)
