#ifndef DRONE_ICON_H
#define DRONE_ICON_H

#include <QPoint>
#include <QPainter>

class DroneIcon
{
public:
    // 绘制无人机图标
    static void drawDroneIcon(QPainter& painter, const QPoint& pos, float heading, float scale = 1.0f);
    
    // 计算无人机航向
    static float calculateHeading(const QPoint& currentPos, const QPoint& nextPos);

private:
    // 绘制无人机主体
    static void drawDroneBody(QPainter& painter, const QPoint& pos, float heading, float scale);
    
    // 绘制方向箭头
    static void drawDirectionArrows(QPainter& painter, const QPoint& pos, float heading, float scale);
};

#endif // DRONE_ICON_H
