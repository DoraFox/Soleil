cmake_minimum_required(VERSION 3.5)

project(Soleil VERSION 0.1 LANGUAGES C CXX)

#set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/mingw81_64")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

# option(QCORO_BUILD_EXAMPLES "Build examples" ON)
# add_feature_info(Examples QCORO_BUILD_EXAMPLES "Build examples")

find_package(QT NAMES Qt5 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Concurrent Core Quick Sql LinguistTools Svg SerialBus Xml)

set(CPP_PATH   ${CMAKE_SOURCE_DIR}/cplusplus)
set(QML_PATH   ${CMAKE_SOURCE_DIR}/qml)
set(ASIO_PATH  ${CMAKE_SOURCE_DIR}/3rdparty/Asio)
set(QCORO_PATH ${CMAKE_SOURCE_DIR}/3rdparty/QCoro)
set(QXLSX_PATH ${CMAKE_SOURCE_DIR}/3rdparty/QXlsx)
set(DLL_PATH   ${CMAKE_SOURCE_DIR}/dll)
set(GLAD_PATH  ${CMAKE_SOURCE_DIR}/3rdparty/glad)
set(GLFW_PATH  ${CMAKE_SOURCE_DIR}/3rdparty/glfw)
set(GLM_PATH   ${CMAKE_SOURCE_DIR}/3rdparty/glm)

#set(QML_IMPORT_PATH ${QML_PATH} ${CPP_PATH} CACHE STRING "" FORCE)
set(QML_IMPORT_PATH ${QML_PATH} ${CPP_PATH} CACHE STRING "" FORCE)

set(ZH_NAME ${PROJECT_NAME}_zh)
set(EN_NAME ${PROJECT_NAME}_en)
set(EXCEL_NAME "translation.xlsx")

set(RESOURCE_RELATIVE_PATH "resource")
set(TRANSLATION_RELATIVE_PATH ${RESOURCE_RELATIVE_PATH}/translation)
set(IMAGE_RELATIVE_PATH ${RESOURCE_RELATIVE_PATH}/image)

set(TM_PATH ${CMAKE_BINARY_DIR}/${TRANSLATION_RELATIVE_PATH})
set(TS_PATH ${CMAKE_SOURCE_DIR}/${TRANSLATION_RELATIVE_PATH})

set(MSGPREFIX_STR "Soleil====")

set(APP_ICON_NAME "OHR.SleepingFox_ZH-CN2622967726_UHD.jpg")

set(DYNAMIC_QML_RESOURCE 1)

add_compile_definitions(
    PROJECT_NAME=${PROJECT_NAME}
    PROJECT_DIR=${CMAKE_SOURCE_DIR}
    PROJECT_BUILD_PATH=${CMAKE_BINARY_DIR}

    MSGPREFIX=${MSGPREFIX_STR}

    ZH_NAME=${ZH_NAME}
    EN_NAME=${EN_NAME}
    EXCEL_NAME=${EXCEL_NAME}
    CPP_PATH=${CPP_PATH}
    QML_PATH=${QML_PATH}
    TRANSLATION_RELATIVE_PATH=${TRANSLATION_RELATIVE_PATH}

    IMAGE_RELATIVE_PATH=${IMAGE_RELATIVE_PATH}
    APP_ICON_NAME=${APP_ICON_NAME}

    DYNAMIC_QML_RESOURCE=${DYNAMIC_QML_RESOURCE}

    NOT_SHOW_QML
    CPP_OPENGL
)

file(GLOB_RECURSE PROJECT_HEADERS
    cplusplus/*.h
    ${GLAD_PATH}/*.h
)

file(GLOB_RECURSE PROJECT_SOURCES
    ${GLAD_PATH}/*.c
    resource/icon.rc
    cplusplus/*.cpp
)

set(USE_CAN_LIN FALSE)
if(NOT USE_CAN_LIN)
    file(GLOB_RECURSE REMOVE_HEADERS
        cplusplus/business/can/*.h
        cplusplus/business/lin/*.h
    )

    file(GLOB_RECURSE REMOVE_SOURCES
        cplusplus/business/can/*.cpp
        cplusplus/business/lin/*.cpp
    )

    list(REMOVE_ITEM PROJECT_HEADERS ${REMOVE_HEADERS})
    list(REMOVE_ITEM PROJECT_SOURCES ${REMOVE_SOURCES})
endif()

message(${PROJECT_SOURCES})

if(DYNAMIC_QML_RESOURCE)
    file(GLOB_RECURSE QML_SOURCES
        qml/*.qml
        qml/qmldir
    )
else()
    set(QML_SOURCES qml/qml.qrc)
endif()

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${TM_PATH}) # set the output files(*.tm) in TM_PATH

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/qml ${TS_FILES} OPTIONS -no-obsolete)

# used in module QCoro
set(QCORO_BUILD_EXAMPLES FALSE)
set(BUILD_TESTING FALSE)
add_subdirectory(3rdparty/QCoro)
add_subdirectory(3rdparty/QXlsx/QXlsx)
add_subdirectory(3rdparty/glm)
add_subdirectory(3rdparty/glfw)

add_executable(Soleil
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
    ${QML_SOURCES}
    ${QM_FILES}
)

target_include_directories(Soleil
    PUBLIC
    ${CPP_PATH}
    ${ASIO_PATH}/include
    ${QCORO_PATH}/qcoro
    #${QXLSX_PATH}/QXlsx/header
    ${GLAD_PATH}/include
    ${GLFW_PATH}/include
    ${GLM_PATH}
)

target_link_directories(Soleil
    PUBLIC
    #${QXLSX_PATH}/lib
    ${DLL_PATH}/x64
    ${CMAKE_BINARY_DIR}/dll/x64
)

if(WIN32)
    target_link_libraries(Soleil
        PUBLIC
        -lwsock32
        -lws2_32
        -lUser32
        -lwinmm
    )
endif()

if(USE_CAN_LIN)
    target_link_libraries(Soleil
        PUBLIC
        -lBinlog
        -lPLinApi
    )
endif()

target_link_libraries(Soleil
    PUBLIC
    #-lQXlsx
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Xml
    QCoro${QT_VERSION_MAJOR}Core
    QCoro${QT_VERSION_MAJOR}Network
    glfw

    PRIVATE
    -static-libgcc
    -static-libstdc++
    -dynamic
    QXlsx::QXlsx
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::SerialBus
)

target_compile_options(Soleil
    PUBLIC
    -DASIO_STANDALONE
    -Wall
    -W
    -g
)

set_target_properties(Soleil PROPERTIES
    WIN32_EXECUTABLE TRUE  #set TRUE for not show console window
)

install(TARGETS Soleil
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

file(
    COPY
    ${CMAKE_SOURCE_DIR}/${IMAGE_RELATIVE_PATH}
    DESTINATION
    ${CMAKE_BINARY_DIR}/${RESOURCE_RELATIVE_PATH}
)

file(
    COPY
    ${DLL_PATH}/x64/
    DESTINATION
    ${CMAKE_BINARY_DIR}
)

#MAIN_QML  ../Soleil/qml/main.qml
