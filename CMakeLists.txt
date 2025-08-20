cmake_minimum_required(VERSION 3.16)

project(MapViewer VERSION 1.0 LANGUAGES CXX C)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#设置CMAKE_PREFIX_PATH变量
set(CMAKE_PREFIX_PATH "D:/ProgramFiles/Qt/6.8.2/mingw_64")

# 查找 Qt 包
find_package(Qt6 COMPONENTS Widgets Core Gui REQUIRED)

# 源文件
set(SOURCES
    res/map_manager.cpp
    res/widget.cpp
    res/main.cpp
    res/a_star.cpp
    res/lidar_sensor.cpp    
    res/lidar_display.cpp   
    res/drone_icon.cpp      
)

# 头文件
set(HEADERS
    res/map_manager.h
    res/map.h
    res/widget.h
    res/a_star.h
    res/lidar_sensor.h      
    res/lidar_display.h     
    res/drone_icon.h        
)

set(FORMS
    res/widget.ui
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/res)

# 自动处理 Qt 的 moc 和 uic
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# 如果需要资源文件，可以添加
# set(QT_RESOURCES
#     resources.qrc
# )
# target_sources(${PROJECT_NAME} PRIVATE ${QT_RESOURCES})
