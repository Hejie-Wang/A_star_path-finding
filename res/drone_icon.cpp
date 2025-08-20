#include "drone_icon.h"
#include <cmath>

void DroneIcon::drawDroneIcon(QPainter& painter, const QPoint& pos, float heading, float scale)
{
    // 保存当前画笔状态
    painter.save();
    
    // 绘制无人机主体
    drawDroneBody(painter, pos, heading, scale);
    
    // 绘制方向箭头
    drawDirectionArrows(painter, pos, heading, scale);
    
    // 恢复画笔状态
    painter.restore();
}

float DroneIcon::calculateHeading(const QPoint& currentPos, const QPoint& nextPos)
{
    // 计算两点之间的角度（弧度）
    float angle = atan2(nextPos.y() - currentPos.y(), nextPos.x() - currentPos.x());
    
    // 转换为度数
    return angle * 180.0f / M_PI;
}

void DroneIcon::drawDroneBody(QPainter& painter, const QPoint& pos, float heading, float scale)
{
    // 移动到无人机位置并旋转
    painter.translate(pos);
    painter.rotate(heading);
    
    // 绘制无人机主体（圆形）
    painter.setBrush(QBrush(Qt::blue));
    painter.setPen(QPen(Qt::darkBlue, 1));
    painter.drawEllipse(QPointF(0, 0), 5 * scale, 5 * scale);
    
    // 绘制无人机前端（三角形）
    QPolygonF frontShape;
    frontShape << QPointF(0, -5 * scale) 
               << QPointF(-3 * scale, -2 * scale) 
               << QPointF(3 * scale, -2 * scale);
    painter.setBrush(QBrush(Qt::darkBlue));
    painter.drawPolygon(frontShape);
}

void DroneIcon::drawDirectionArrows(QPainter& painter, const QPoint& pos, float heading, float scale)
{
    // 重置变换
    painter.resetTransform();
    
    // X轴箭头（航向方向）
    painter.save();
    painter.translate(pos);
    painter.rotate(heading);
    
    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(0, 0, 0, -10 * scale);
    
    // 箭头头部
    QPolygonF xArrowHead;
    xArrowHead << QPointF(0, -10 * scale) 
               << QPointF(-3 * scale, -7 * scale) 
               << QPointF(3 * scale, -7 * scale);
    painter.setBrush(QBrush(Qt::red));
    painter.drawPolygon(xArrowHead);
    
    painter.restore();
    
    // Y轴箭头（垂直于航向方向）
    painter.save();
    painter.translate(pos);
    painter.rotate(heading - 90); // 右手系
    
    painter.setPen(QPen(Qt::green, 2));
    painter.drawLine(0, 0, 0, -8 * scale);
    
    // 箭头头部
    QPolygonF yArrowHead;
    yArrowHead << QPointF(0, -8 * scale) 
               << QPointF(-2 * scale, -5 * scale) 
               << QPointF(2 * scale, -5 * scale);
    painter.setBrush(QBrush(Qt::green));
    painter.drawPolygon(yArrowHead);
    
    painter.restore();
}
