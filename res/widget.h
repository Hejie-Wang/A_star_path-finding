#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include "map_manager.h"
#include "a_star.h"
#include "lidar_sensor.h"    
#include "lidar_display.h"  

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void startPathfinding();
    void loadMapFile();
    //void showMap();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateAnimation();
    void updateLidarDisplay();

private:
    Ui::Widget *ui;
    QTimer *animationTimer;
    std::vector<Node*> path;
    int currentPathIndex;
    bool isAnimating;

    LidarSensor m_lidarSensor;
    LidarDisplay *m_lidarDisplay;
    float m_droneHeading;      // 无人机航向

};
#endif // WIDGET_H
