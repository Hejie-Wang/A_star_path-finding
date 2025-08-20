
// map_manager.h：实现管理地图功能：包括地图加载，地图渲染，获取地图信息

#pragma once
#include "map.h"
#include <QImage>

class MapManager {
public:
    static bool loadMap(const char* fname);
    static QImage renderMap();
    static const RMap& getMapData() { return g_map; }
    //static void show_map();
    // 更新地图上特定点的状态
    static void updateMapPoint(int x, int y, int8_t newState);
    
    // 检查点是否为已探测障碍物
    static bool isDetectedObstacle(int x, int y);
    
    // 重置地图上所有障碍物为未探测状态
    static void resetObstacleDetection();

    // 添加重新设置起点的方法
    static void setStartPoint(int x, int y);

    // 新增：动态扩展单个障碍物边缘
    static void expandObstacleEdge(int x, int y);

private:
    static RMap g_map;
    static void markObstacleEdges();
};
