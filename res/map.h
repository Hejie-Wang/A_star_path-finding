
//map.h：实现地图数据结构的定义

#pragma once
#include <cstdio>
#include <cstdint>

//定义地图类/结构体
struct RMap
{
    int32_t    m_mx;		//地图的宽
    int32_t    m_my;		//地图的高
    int32_t    m_grx;		//栅格的宽
    int32_t    m_gry;		//栅格的高
    int32_t    m_startX;	//起始点的x坐标
    int32_t    m_startY;	//起始点的y坐标
    int32_t    m_endX;		//终点的x坐标
    int32_t    m_endY;		//终点的y坐标
    int8_t     *m_map;		//地图数据
};

//地图每个像素的值的定义
enum RMapItemType
{
    RMAP_FREE = 0,					// 空白区域
    RMAP_OBSTACLE,					// 障碍物
    RMAP_OBSTACLE_UNDESCOVERED,		// 未被发现的障碍物
    RMAP_OBSTACLE_SCANNED,			// 已被扫描到的障碍物
    RMAP_START_POINT,				// 起点
    RMAP_END_POINT,					// 终点
    RMAP_CURRENT_POSITION,			// 当前点
    RMAP_OBSTACLE_EDGE				// 障碍物扩展的点
};

// 全局地图实例
extern RMap g_map;