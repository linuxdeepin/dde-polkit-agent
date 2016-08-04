cmake_minimum_required(VERSION 3.2)

project(dde-polkit-agent)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

aux_source_directory(. SRC_LIST)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(POLKITQT-1 REQUIRED polkit-qt5-1)
pkg_search_module(DTKBASE REQUIRED dtkbase)
pkg_search_module(DTKWIDGET REQUIRED dtkwidget)

include_directories(BEFORE ${POLKITQT-1_INCLUDE_DIRS})
include_directories(BEFORE ${DTKBASE_INCLUDE_DIRS})
include_directories(BEFORE ${DTKWIDGET_INCLUDE_DIRS})

qt5_add_dbus_adaptor(SRC_LIST com.deepin.Polkit1AuthAgent.xml policykitlistener.h PolicyKitListener)
add_executable(${PROJECT_NAME} ${SRC_LIST} images.qrc)

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::DBus
        ${POLKITQT-1_LIBRARIES}
        ${DTKBASE_LIBRARIES}
        ${DTKWIDGET_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION /usr/lib/polkit-1-dde/)
install(FILES polkit-dde-authentication-agent-1.desktop
        DESTINATION /etc/xdg/autostart/)
