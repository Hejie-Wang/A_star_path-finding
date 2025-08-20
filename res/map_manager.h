
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

private:
    static RMap g_map;
    static void markObstacleEdges();
};
