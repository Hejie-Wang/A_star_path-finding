#ifndef LIDAR_DISPLAY_H
#define LIDAR_DISPLAY_H

#include <QWidget>
#include <QPoint>
#include <vector>

class LidarDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit LidarDisplay(QWidget *parent = nullptr);
    
    // 更新显示数据
    void updateDisplay(const std::vector<QPoint>& detectionArea, 
                      const std::vector<QPoint>& detectedObstacles,
                      const QPoint& dronePos, 
                      float droneHeading);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<QPoint> m_detectionArea;      // 探测区域
    std::vector<QPoint> m_detectedObstacles;   // 探测到的障碍物
    QPoint m_dronePos;                         // 无人机位置
    float m_droneHeading;                      // 无人机航向
};

#endif // LIDAR_DISPLAY_H
