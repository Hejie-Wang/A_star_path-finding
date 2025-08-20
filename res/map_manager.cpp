
// map_manager.cpp：将map_manager中接口实例化

#include "map_manager.h"
#include <cstdio>

RMap MapManager::g_map;

//实现地图加载
bool MapManager::loadMap(const char* fname) {
    FILE* fp = fopen(fname, "rb");
    if (!fp) return false;

    uint32_t magic;
    fread(&magic, sizeof(uint32_t), 1, fp);
    if (magic != 0x15432345) {
        fclose(fp);
        return false;
    }

    // 读取地图数据
    fread(&g_map.m_mx, sizeof(int32_t), 1, fp);
    fread(&g_map.m_my, sizeof(int32_t), 1, fp);

    delete[] g_map.m_map;
    g_map.m_map = new int8_t[g_map.m_mx * g_map.m_my];
    fread(g_map.m_map, sizeof(int8_t), g_map.m_mx * g_map.m_my, fp);

    // 读取起终点
    fread(&g_map.m_startX, sizeof(int32_t), 1, fp);
    fread(&g_map.m_startY, sizeof(int32_t), 1, fp);
    fread(&g_map.m_endX, sizeof(int32_t), 1, fp);
    fread(&g_map.m_endY, sizeof(int32_t), 1, fp);

    fclose(fp);

    // 标记障碍物边缘
    //markObstacleEdges();
    
    // 在地图上标记起点和终点
    g_map.m_map[g_map.m_startY * g_map.m_mx + g_map.m_startX] = RMAP_START_POINT;
    g_map.m_map[g_map.m_endY * g_map.m_mx + g_map.m_endX] = RMAP_END_POINT;

    return true;
}


//标记障碍物边缘
// void MapManager::markObstacleEdges() {
//     // 创建临时地图副本
//     int8_t* tempMap = new int8_t[g_map.m_mx * g_map.m_my];
//     memcpy(tempMap, g_map.m_map, g_map.m_mx * g_map.m_my * sizeof(int8_t));
    
//     // 遍历地图，查找障碍物
//     for (int y = 0; y < g_map.m_my; y++) {
//         for (int x = 0; x < g_map.m_mx; x++) {
//             int idx = y * g_map.m_mx + x;
            
//             // 如果是障碍物，则标记其周围的点为边缘
//             if (tempMap[idx] == RMAP_OBSTACLE) {
//                 // 检查周围两个单位的点
//                 for (int dy = -2; dy <= 2; dy++) {
//                     for (int dx = -2; dx <= 2; dx++) {
//                         int nx = x + dx;
//                         int ny = y + dy;
                        
//                         // 检查边界
//                         if (nx >= 0 && nx < g_map.m_mx && ny >= 0 && ny < g_map.m_my) {
//                             int nidx = ny * g_map.m_mx + nx;
//                             // 如果原本是自由区域，则标记为边缘
//                             if (g_map.m_map[nidx] == RMAP_FREE) {
//                                 g_map.m_map[nidx] = RMAP_OBSTACLE_EDGE;
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
    
//     // 释放临时地图
//     delete[] tempMap;
// }


//实现地图渲染功能
QImage MapManager::renderMap() {
    if (!g_map.m_map) return QImage();

    QImage img(g_map.m_mx, g_map.m_my, QImage::Format_RGB888);
    uchar* data = img.bits();

    for (int y = 0; y < g_map.m_my; y++) {
        for (int x = 0; x < g_map.m_mx; x++) {
            int idx = y * g_map.m_mx + x;
            int pixel = (y * g_map.m_mx + x) * 3;

            switch (g_map.m_map[idx]) {
                case RMAP_FREE:
                    data[pixel] = data[pixel+1] = data[pixel+2] = 255;  // 白色
                    break;
                    
                // case RMAP_OBSTACLE:
                //     data[pixel] = 190;
                //     data[pixel+1] = 190;
                //     data[pixel+2] = 190;    // 灰色
                //     break;

                case RMAP_OBSTACLE_UNDESCOVERED:
                    data[pixel] = 190;
                    data[pixel+1] = 190;
                    data[pixel+2] = 190;    // 灰色
                    break;
    
                case RMAP_OBSTACLE_SCANNED:
                    data[pixel] = data[pixel+1] = data[pixel+2] = 0;    // 黑色（不可通行）
                    break;
                    
                case RMAP_START_POINT:
                    data[pixel] = 255;     // R
                    data[pixel+1] = 0;     // G
                    data[pixel+2] = 0;     // B - 红色
                    break;
                    
                case RMAP_END_POINT:
                    data[pixel] = 0;       // R
                    data[pixel+1] = 0;     // G
                    data[pixel+2] = 255;   // B - 蓝色
                    break;
                    
                case RMAP_OBSTACLE_EDGE:
                    data[pixel] = 230;
                    data[pixel+1] = 230;
                    data[pixel+2] = 250;  //lavender
                    break;
                    
                default:
                    data[pixel] = data[pixel+1] = data[pixel+2] = 255;  // 默认白色
                    break;
            }


        //     // 绘制网格线（浅灰色）
        //     // 水平线
        //     if (y > 0) {
        //         int linePixel = ((y-1) * g_map.m_mx + x) * 3;
        //         data[linePixel] = data[linePixel+1] = data[linePixel+2] = 240;  // 浅灰色
        //     }
        //     // 垂直线
        //     if (x > 0) {
        //         int linePixel = (y * g_map.m_mx + (x-1)) * 3;
        //         data[linePixel] = data[linePixel+1] = data[linePixel+2] = 240;  // 浅灰色
        //     }
         }
    }

    // // 绘制最外边框
    // // 上边框
    // for (int x = 0; x < g_map.m_mx; x++) {
    //     int pixel = x * 3;
    //     data[pixel] = data[pixel+1] = data[pixel+2] = 200;  // 深一点的灰色
    // }
    // // 下边框
    // for (int x = 0; x < g_map.m_mx; x++) {
    //     int pixel = ((g_map.m_my-1) * g_map.m_mx + x) * 3;
    //     data[pixel] = data[pixel+1] = data[pixel+2] = 200;
    // }
    // // 左边框
    // for (int y = 0; y < g_map.m_my; y++) {
    //     int pixel = (y * g_map.m_mx) * 3;
    //     data[pixel] = data[pixel+1] = data[pixel+2] = 200;
    // }
    // // 右边框
    // for (int y = 0; y < g_map.m_my; y++) {
    //     int pixel = (y * g_map.m_mx + (g_map.m_mx-1)) * 3;
    //     data[pixel] = data[pixel+1] = data[pixel+2] = 200;
    // }

    return img;
}

//更新地图当前点状态
void MapManager::updateMapPoint(int x, int y, int8_t newState) {
    if (x >= 0 && x < g_map.m_mx && y >= 0 && y < g_map.m_my) {
        g_map.m_map[y * g_map.m_mx + x] = newState;
    }
}

bool MapManager::isDetectedObstacle(int x, int y) {
    if (x >= 0 && x < g_map.m_mx && y >= 0 && y < g_map.m_my) {
        return g_map.m_map[y * g_map.m_mx + x] == RMAP_OBSTACLE_SCANNED;
    }
    return false;
}


void MapManager::resetObstacleDetection() {
    for (int y = 0; y < g_map.m_my; y++) {
        for (int x = 0; x < g_map.m_mx; x++) {
            int idx = y * g_map.m_mx + x;
            if (g_map.m_map[idx] == RMAP_OBSTACLE_SCANNED) {
                g_map.m_map[idx] = RMAP_OBSTACLE_UNDESCOVERED;
            }
        }
    }
}

//重新设置起点（用于A_star算法寻路）
void MapManager::setStartPoint(int x, int y) {
    if (x >= 0 && x < g_map.m_mx && y >= 0 && y < g_map.m_my) {
        // 清除旧起点
        g_map.m_map[g_map.m_startY * g_map.m_mx + g_map.m_startX] = RMAP_FREE;
        
        // 设置新起点
        g_map.m_startX = x;
        g_map.m_startY = y;
        // g_map.m_map[y * g_map.m_mx + x] = RMAP_START_POINT;
    }
}


// //展示地图
// void MapManager::show_map() {
//     if (!g_map.m_map) {
//         printf("地图数据为空！\n");
//         return;
//     }

//     printf("地图尺寸: %d x %d\n", g_map.m_mx, g_map.m_my);
//     printf("起点: (%d, %d)\n", g_map.m_startX, g_map.m_startY);
//     printf("终点: (%d, %d)\n", g_map.m_endX, g_map.m_endY);
//     printf("地图内容:\n");

//     for (int y = 0; y < g_map.m_my; y++) {
//         for (int x = 0; x < g_map.m_mx; x++) {
//             int idx = y * g_map.m_mx + x;
//             printf("%d ", g_map.m_map[idx]);
//         }
//         printf("\n");
//     }
// }



// 新增：动态扩展单个障碍物边缘
void MapManager::expandObstacleEdge(int x, int y) {
    // 新增：检查传入的坐标是否是有效的障碍物
    int idx = y * g_map.m_mx + x;
    if (g_map.m_map[idx] != RMAP_OBSTACLE_SCANNED) {
        return; // 新增：如果不是已扫描的障碍物，则不进行扩展
    }

    // 新增：检查周围两个单位的点
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            if (dx == 0 && dy == 0) continue; // 新增：跳过障碍物本身

            int nx = x + dx;
            int ny = y + dy;

            // 新增：检查边界
            if (nx >= 0 && nx < g_map.m_mx && ny >= 0 && ny < g_map.m_my) {
                int nidx = ny * g_map.m_mx + nx;
                // 新增：如果原本是自由区域，则标记为边缘
                if (g_map.m_map[nidx] == RMAP_FREE) {
                    g_map.m_map[nidx] = RMAP_OBSTACLE_EDGE;
                }
            }
        }
    }
}
