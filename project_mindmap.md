# 项目思维导图 (Mermaid 源码)

您可以将以下代码块中的内容分别复制到 [Mermaid Live Editor](https://mermaid.live) 或其他支持Mermaid的工具中，以生成并下载思维导图图片。

---

## 1. Widget核心模块交互图 - 源码

```mermaid
graph TD
    subgraph Widget主控
        A[Widget]
    end

    subgraph 功能模块
        B[A* 寻路算法 a_star.h]
        C[地图管理器 map_manager.h]
        D[激光雷达传感器 lidar_sensor.h]
    end

    subgraph UI与显示
        E[UI界面 widget.ui]
        F[无人机图标 drone_icon.h]
        G[激光雷达显示 lidar_display.h]
        H[Qt绘图 QPainter]
    end
    
    subgraph 事件驱动
        I[QTimer]
        J[QPushButton]
    end

    J -- "findPathButton.clicked()" --> J_func[startPathfinding()]
    J_func --> A
    
    I -- "timeout()" --> I_func[updateAnimation()]
    I_func --> A

    A -- 调用 --> C
    A -- 调用 --> B
    
    subgraph "Widget 模块功能: startPathfinding()"
        direction LR
        J1[开始寻路] --> J2{加载地图数据};
        J2 --> J3[调用A*算法 findPath];
        J3 -- 成功 --> J4[启动动画];
        J3 -- 失败 --> J5[更新UI];
    end
    
    subgraph "Widget 模块功能: updateAnimation()"
        direction LR
        I1[更新动画] --> I2{更新路径点索引};
        I2 --> I3[检查路径碰撞 checkPathCollision];
        I3 -- 未碰撞 --> I4[触发重绘 update()];
        I3 -- 碰撞 --> I5[重新规划路径 replanPath];
    end
    
    subgraph "Widget 模块功能: replanPath()"
        direction LR
        P1[重新规划] --> P2[保存历史路径];
        P2 --> P3[设置新起点];
        P3 --> P4[调用A*算法 findPath];
        P4 -- 成功 --> P5[启动新动画];
    end


    subgraph "Widget 模块绘制: paintEvent()"
        direction LR
        K[绘制事件] -- 触发 --> L{渲染地图};
        L -- 获取 --> C[MapManager::renderMap()];
        L --> M[绘制历史/当前路径];
        M --> N[绘制无人机图标];
        N -- 获取位置和朝向 --> A;
        N -- 调用 --> F[DroneIcon::drawDroneIcon];
    end
    
    subgraph "Widget 模块更新: updateLidarDisplay()"
        direction LR
        D -- "detectionUpdated()" --> S1[更新激光雷达显示];
        S1 -- 调用 --> G[LidarDisplay::updateDisplay];
    end

    A -- "paintEvent()" --> H
    A -- "updateLidarDisplay()" --> G
    A --"updateAnimation()"-->D
```

---

## 2. Connect核心信号槽流程图 - 源码

```mermaid
graph LR
    subgraph 用户界面 UI
        User[用户] --> FindPathButton[QPushButton findPathButton]
    end

    subgraph 核心控制器
        W[Widget]
    end

    subgraph 定时器
        Timer[QTimer animationTimer]
    end

    subgraph 激光雷达
        Lidar[LidarSensor m_lidarSensor]
    end
    
    subgraph 流程
        FindPathButton --> Signal1{clicked}
        Signal1 --> Slot1(Widget::startPathfinding)

        Timer --> Signal2{timeout}
        Signal2 --> Slot2(Widget::updateAnimation)

        Lidar --> Signal3{detectionUpdated}
        Signal3 --> Slot3(Widget::updateLidarDisplay)
    end
    
    %% 定义样式
    classDef object fill:#f9f,stroke:#333,stroke-width:2px;
    classDef signal fill:#ccf,stroke:#333,stroke-width:2px,rx:10,ry:10;
    classDef slot fill:#cfc,stroke:#333,stroke-width:2px,rx:20,ry:20;
    
    class W,Timer,Lidar,FindPathButton object
    class Signal1,Signal2,Signal3 signal
    class Slot1,Slot2,Slot3 slot
end
