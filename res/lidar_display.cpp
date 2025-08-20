#include "lidar_display.h"
#include <QPainter>
#include <cmath>

LidarDisplay::LidarDisplay(QWidget *parent)
    : QWidget(parent)
    , m_droneHeading(0.0f)
{
    // 设置窗口标题和大小
    setWindowTitle("激光雷达探测");
    resize(400, 400);
}

void LidarDisplay::updateDisplay(const std::vector<QPoint>& detectionArea, 
                               const std::vector<QPoint>& detectedObstacles,
                               const QPoint& dronePos, 
                               float droneHeading)
{
    m_detectionArea = detectionArea;
    m_detectedObstacles = detectedObstacles;
    m_dronePos = dronePos;
    m_droneHeading = droneHeading;
    
    // 触发重绘
    update();
}

void LidarDisplay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    
    // 设置背景
    painter.fillRect(rect(), Qt::black);
    
    // 如果没有数据，显示提示信息
    if (m_detectionArea.empty()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Waiting for detection data...");
        return;
    }
    
    // 计算缩放比例，使探测区域适应窗口
    float scale = std::min(width(), height()) / 60.0f; // 假设探测范围最大为60
    
    // 计算偏移，使无人机位于窗口中心
    float offsetX = width() / 2.0f - m_dronePos.x() * scale;
    float offsetY = height() / 2.0f - m_dronePos.y() * scale;
    
    // 绘制探测区域（浅绿色点）
    painter.setPen(QPen(Qt::darkGreen, 1));
    for (const QPoint& point : m_detectionArea) {
        painter.drawPoint(static_cast<int>(point.x() * scale + offsetX), 
                         static_cast<int>(point.y() * scale + offsetY));
    }
    
    // 绘制探测到的障碍物（红色点）
    painter.setPen(QPen(Qt::red, 2));
    for (const QPoint& point : m_detectedObstacles) {
        painter.drawPoint(static_cast<int>(point.x() * scale + offsetX), 
                         static_cast<int>(point.y() * scale + offsetY));
    }
    
    // 绘制无人机（蓝色三角形）
    painter.save();
    painter.translate(static_cast<int>(m_dronePos.x() * scale + offsetX), 
                     static_cast<int>(m_dronePos.y() * scale + offsetY));
    painter.rotate(m_droneHeading);
    
    QPolygonF droneShape;
    droneShape << QPointF(0, -5) << QPointF(-3, 5) << QPointF(3, 5);
    painter.setBrush(QBrush(Qt::blue));
    painter.setPen(QPen(Qt::darkBlue, 1));
    painter.drawPolygon(droneShape);
    
    // 绘制航向指示线
    painter.setPen(QPen(Qt::yellow, 2));
    painter.drawLine(0, 0, 0, -10);
    
    painter.restore();
}
