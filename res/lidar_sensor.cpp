#include "lidar_sensor.h"
#include "map_manager.h"  // 包含MapManager以获取地图数据
#include <cmath>
#include <algorithm>

LidarSensor::LidarSensor(QObject *parent)
    : QObject(parent)
    , m_detectionAngle(120.0f)
    , m_detectionRange(30)
    , m_droneHeading(0.0f)
{
}

void LidarSensor::init(float detectionAngle, int detectionRange)
{
    m_detectionAngle = detectionAngle;
    m_detectionRange = detectionRange;
}

void LidarSensor::update(const QPoint& dronePos, float droneHeading)
{
    m_dronePos = dronePos;
    m_droneHeading = droneHeading;
    
    // 计算探测区域
    calculateDetectionArea(dronePos, droneHeading);
    
    // 检测障碍物
    detectObstacles();
    
    // 发送探测更新信号，同时传递障碍物和边缘信息
     emit detectionUpdated(m_detectedObstacles, m_detectedEdges);
}

void LidarSensor::calculateDetectionArea(const QPoint& dronePos, float droneHeading)
{
    m_detectionArea.clear();
    
    // 将角度转换为弧度
    float angleRad = droneHeading * M_PI / 180.0f;
    float halfAngleRad = m_detectionAngle * M_PI / 360.0f; // 一半角度
    
    // 计算探测区域内的点
    for (int r = 1; r <= m_detectionRange; r++) {
        for (float a = -halfAngleRad; a <= halfAngleRad; a += 0.1f) {
            // 计算点的坐标
            float x = dronePos.x() + r * cos(angleRad + a);
            float y = dronePos.y() + r * sin(angleRad + a);
            
            // 添加到探测区域
            m_detectionArea.push_back(QPoint(static_cast<int>(x), static_cast<int>(y)));
        }
    }
}

void LidarSensor::detectObstacles()
{
    m_detectedObstacles.clear();
    
    // 通过MapManager获取地图数据
    const RMap& mapData = MapManager::getMapData();
    
    // 检查地图数据是否有效
    if (!mapData.m_map || mapData.m_mx <= 0 || mapData.m_my <= 0) {
        return;
    }
    
    // 检查探测区域内的点是否为障碍物
    for (const QPoint& point : m_detectionArea) {
        // 检查点是否在地图范围内
        if (point.x() >= 0 && point.x() < mapData.m_mx && 
            point.y() >= 0 && point.y() < mapData.m_my) {
            
            // 检查是否为障碍物
            int mapValue = mapData.m_map[point.y() * mapData.m_mx + point.x()];
            if (mapValue == RMAP_OBSTACLE || mapValue == RMAP_OBSTACLE_UNDESCOVERED) {
                // 添加到探测到的障碍物列表
                m_detectedObstacles.push_back(point);
                
                // 更新地图状态为已探测障碍物
                MapManager::updateMapPoint(point.x(), point.y(), RMAP_OBSTACLE_SCANNED);
                
                // 新增：动态扩展新发现障碍物的边缘
                MapManager::expandObstacleEdge(point.x(), point.y());
            } else if (mapValue == RMAP_OBSTACLE_EDGE) {
                // 将边缘点添加到检测列表
                m_detectedEdges.push_back(point);
            }
        }
    }
}

const std::vector<QPoint>& LidarSensor::getDetectedObstacles() const
{
    return m_detectedObstacles;
}

const std::vector<QPoint>& LidarSensor::getDetctedEdges() const
{
    return m_detectedEdges;
}

const std::vector<QPoint>& LidarSensor::getDetectionArea() const
{
    return m_detectionArea;
}
