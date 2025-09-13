# MapViewer - 基于 A* 算法的路径规划查看器

本仓库是一个基于 Qt 和 C++ 实现的 2D 地图路径规划可视化项目。核心算法采用 A* 算法，用于在给定地图中寻找从起点到终点的最优路径，并动态模拟无人机沿路径飞行的过程，同时集成了激光雷达（Lidar）对周围障碍物的探测和路径重规划功能。

## 算法介绍

A\* (A-Star) 算法是一种在图形中寻找最短路径的启发式搜索算法。它通过评估每个节点的“代价”来决定搜索方向，从而高效地找到最优解。

每个节点的代价 `f(n)` 由两部分组成：

1.  **`g(n)`**: 从起点到当前节点 `n` 的实际移动代价。
2.  **`h(n)`**: 从当前节点 `n` 到目标点的估计移动代价（启发式函数）。本项目中使用曼哈顿距离作为启发式函数。

A\* 算法的核心思想是，在每一步都优先选择 f(n) 值最小的节点进行扩展，直到找到目标点为止。

**推荐学习视频:**

如果您对 A\* 算法的原理感兴趣，可以观看以下视频进行学习：

*   [【算法】A*寻路算法 | C++实现 | 可视化](https://www.bilibili.com/video/BV1bv411y79P/)

## 技术栈与工具

*   **编程语言**: `C++17`
*   **GUI 框架**: `Qt 6` (Core, Gui, Widgets)
*   **构建系统**: `CMake`
*   **开发环境**: 支持标准 C++17 和 Qt6 的开发环境 (如: Visual Studio, CLion, VS Code with C++ extensions)

## 项目结构

```
.
├── CMakeLists.txt              # CMake 构建配置文件
├── create_map/                 # 用于生成测试地图的工具代码
│   ├── create_cave_map.cpp
│   └── create_map.cpp
├── maps/                       # 存放地图文件 (*.map)
│   ├── cave_map.map
│   └── ...
├── res/                        # 核心源代码和资源
│   ├── a_star.h / .cpp         # A* 寻路算法的核心实现
│   ├── map_manager.h / .cpp    # 地图加载、渲染和管理模块
│   ├── widget.h / .cpp         # Qt 主窗口，负责 UI 和交互逻辑
│   ├── lidar_sensor.h / .cpp   # 模拟激光雷达传感器
│   ├── lidar_display.h / .cpp  # 雷达扫描界面的显示
│   ├── drone_icon.h / .cpp     # 无人机图标的绘制
│   ├── main.cpp                # 程序入口
│   └── widget.ui               # 主窗口的 UI 设计文件
└── README.md                   # 就是你现在看到的文件
```

### 文件功能详解

*   **`a_star`**: 封装了 A\* 寻路算法。`findPath()` 函数接收一个地图对象，计算并返回一条从起点到终点的最优路径（`std::vector<Node*>`）。
*   **`map_manager`**: 一个静态类，负责所有与地图数据相关的操作。`loadMap()` 从二进制文件加载地图，`renderMap()` 将地图数据渲染成 `QImage` 以便在 UI 上显示。
*   **`widget`**: 项目的主窗口。它初始化 UI，处理用户输入（如加载地图、开始寻路），并使用 `QTimer` 和 `paintEvent` 来驱动路径动画的播放和无人机位置的更新。
*   **`lidar_sensor`**: 模拟无人机搭载的激光雷达。它可以探测指定范围内的障碍物，并返回探测结果。
*   **`drone_icon`**: 负责在地图上绘制代表无人机的图标，并根据无人机的朝向进行旋转。
*   **`main.cpp`**: 整个应用程序的入口。它创建并显示 `Widget` 主窗口。

## 快速开始

1.  **配置环境**:
    *   确保您已安装 `CMake` 和 C++ 编译器。
    *   安装 `Qt 6`。
    *   **重要**: 修改 [`CMakeLists.txt`](CMakeLists.txt:10) 文件第 10 行的 `CMAKE_PREFIX_PATH`，使其指向您本地的 Qt 6 安装路径。

2.  **构建项目**:
    ```bash
    # 创建一个构建目录
    mkdir build
    cd build
    
    # 运行 CMake 并编译
    cmake ..
    cmake --build .
    ```

3.  **运行程序**:
    *   编译成功后，在 `build` 目录下会生成可执行文件 `MapViewer.exe` (Windows) 或 `MapViewer` (Linux/macOS)。
    *   **注意**: 本项目通过修改源代码来加载地图。请打开 `res/widget.cpp` 文件，并修改第 176 行来指定您想加载的 `.map` 文件。
    *   修改完毕后，重新编译并运行程序，然后点击 "Start" 按钮开始路径规划和动画演示。


## 成果展示

### 基础 A* 寻路：简单地图

![基础寻路演示](image/A_star_map1.gif)

### 动态避障与路径重规划：复杂地图

![动态避障演示](image/A_star_map7.gif)