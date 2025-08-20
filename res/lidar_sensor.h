#ifndef LIDAR_SENSOR_H
#define LIDAR_SENSOR_H

#include <QObject>
#include <QPoint>
#include <vector>
#include "map.h"  // 包含RMap定义

class LidarSensor : public QObject
{
    Q_OBJECT

public:
    // 构造函数
    explicit LidarSensor(QObject *parent = nullptr);
    
    // 初始化激光雷达参数
    void init(float detectionAngle = 120.0f, int detectionRange = 30);
    
    // 更新探测数据
    void update(const QPoint& dronePos, float droneHeading);
    
    // 获取探测到的障碍物点
    const std::vector<QPoint>& getDetectedObstacles() const;
    
    // 获取探测区域点
    const std::vector<QPoint>& getDetectionArea() const;

signals:
    // 探测数据更新信号
    void detectionUpdated(const std::vector<QPoint>& detectedObstacles);

private:
    // 计算探测区域
    void calculateDetectionArea(const QPoint& dronePos, float droneHeading);
    
    // 检测障碍物
    void detectObstacles();

private:
    float m_detectionAngle;      // 探测角度（120度）
    int m_detectionRange;        // 探测范围（30像素）
    QPoint m_dronePos;           // 无人机位置
    float m_droneHeading;        // 无人机航向
    std::vector<QPoint> m_detectionArea;     // 探测区域点集
    std::vector<QPoint> m_detectedObstacles;  // 探测到的障碍物
};

#endif // LIDAR_SENSOR_H
