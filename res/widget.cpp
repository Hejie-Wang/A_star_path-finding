#include "widget.h"
#include "ui_widget.h"
#include <QTimer>
#include <QPainter>
#include "drone_icon.h"    
#include "map_manager.h"    


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , animationTimer(new QTimer(this))
    , currentPathIndex(0)
    , isAnimating(false)
    , m_droneHeading(0.0f)  
{
    ui->setupUi(this);

    // 连接定时器信号
    connect(animationTimer, &QTimer::timeout, this, &Widget::updateAnimation);

    // 设置定时器间隔为100毫秒
    animationTimer->setInterval(100);

    // 在Widget构造函数中连接按钮信号
    connect(ui->findPathButton, &QPushButton::clicked, this, &Widget::startPathfinding);

    //初始化激光雷达
    m_lidarSensor.init(120.0f, 30);  // 120度探测角度，30像素探测范围

    // 新增：创建激光雷达显示窗口
    m_lidarDisplay = new LidarDisplay();
    m_lidarDisplay->setWindowTitle("激光雷达探测");
    m_lidarDisplay->resize(400, 400);
    m_lidarDisplay->show();

    //连接激光雷达更新信号
    connect(&m_lidarSensor, &LidarSensor::detectionUpdated, this, &Widget::updateLidarDisplay);
}

Widget::~Widget()
{
    delete ui;
    delete m_lidarDisplay;
    AStar::clearPath(path);
}

void Widget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    
    // 获取地图图像
    QImage mapImg = MapManager::renderMap();
    if (mapImg.isNull()) {
        painter.drawText(rect(), Qt::AlignCenter, "地图未加载");
        return;
    }
    
    // 缩放图像以适应窗口
    QImage scaledImg = mapImg.scaled(size(), Qt::KeepAspectRatio);
    painter.drawImage(0, 0, scaledImg);
    
    // 如果有路径，绘制路径
    if (!path.empty()) {
        // 计算缩放比例
        float scaleX = (float)scaledImg.width() / mapImg.width();
        float scaleY = (float)scaledImg.height() / mapImg.height();

        //高亮航路点
        
        // 设置画笔
        painter.setPen(QPen(Qt::green, 2));

        //获取地图数据用于障碍物检查
         const RMap& mapData = MapManager::getMapData();
        
        // 绘制路径
        for (size_t i = 1; i < path.size(); i++) {
            //计算实际绘制坐标
            int x1 = path[i-1]->x * scaleX + scaleX/2;
            int y1 = path[i-1]->y * scaleY + scaleY/2;
            int x2 = path[i]->x * scaleX + scaleX/2;
            int y2 = path[i]->y * scaleY + scaleY/2;
            painter.drawLine(x1, y1, x2, y2);
        
            // 检查路径点是否在障碍物边界上，如果是则调整位置
            if (mapData.m_map && path[i]->y < mapData.m_my && path[i]->x < mapData.m_mx) {
                int mapValue = mapData.m_map[path[i]->y * mapData.m_mx + path[i]->x];
                if (mapValue == 1) { // 假设1表示障碍物
                    // 调整位置，稍微偏移以避免重叠
                    x2 += (path[i]->x > 0) ? -scaleX/4 : scaleX/4;
                    y2 += (path[i]->y > 0) ? -scaleY/4 : scaleY/4;
                }
            }

            painter.drawLine(x1, y1, x2, y2);

            if (i != 1 && i != path.size() - 1) {
                // 中间航路点用较小的黄色圆点
                painter.setBrush(QBrush(Qt::yellow));
                painter.setPen(QPen(Qt::darkYellow, 1));
                painter.drawEllipse(x1-2, y1-2, 4, 4);
            }
        }
        
        // 如果正在动画中，绘制当前位置
        if (isAnimating && currentPathIndex < path.size()) {
            int x = path[currentPathIndex]->x * scaleX + scaleX/2;
            int y = path[currentPathIndex]->y * scaleY + scaleY/2;
            
        // 计算航向
        if (currentPathIndex < path.size() - 1) {
            int nextX = path[currentPathIndex+1]->x * scaleX + scaleX/2;
            int nextY = path[currentPathIndex+1]->y * scaleY + scaleY/2;
            m_droneHeading = DroneIcon::calculateHeading(QPoint(x, y), QPoint(nextX, nextY));
        }
        
        // 更新激光雷达探测
        m_lidarSensor.update(QPoint(path[currentPathIndex]->x, path[currentPathIndex]->y), m_droneHeading);
        
        // 绘制新的无人机图标
        DroneIcon::drawDroneIcon(painter, QPoint(x, y), m_droneHeading);
        }
    }
}

void Widget::loadMapFile() {
    if (MapManager::loadMap("E:/Qtproject/A_star/test_1.map")) {
        update(); // 触发重绘
    }
}

//寻路算法入口
void Widget::startPathfinding() {
    // 清除之前的路径
    AStar::clearPath(path);
    currentPathIndex = 0;
    
    // 获取地图数据
    const RMap& mapData = MapManager::getMapData();
    
    // 使用A*算法寻找路径
    if (AStar::findPath(mapData, path)) {
        // 找到路径，开始动画
        isAnimating = true;
        animationTimer->start();
    } else {
        // 未找到路径
        isAnimating = false;
        update();
    }
}

void Widget::updateAnimation() {
    if (currentPathIndex < path.size() - 1) {
        currentPathIndex++;
        update(); // 触发重绘
    } else {
        // 动画结束
        animationTimer->stop();
        isAnimating = false;
    }
}

// 新增：更新激光雷达显示
void Widget::updateLidarDisplay()
{
    m_lidarDisplay->updateDisplay(
        m_lidarSensor.getDetectionArea(),
        m_lidarSensor.getDetectedObstacles(),
        QPoint(path[currentPathIndex]->x, path[currentPathIndex]->y),
        m_droneHeading
    );
}

// void Widget::showMap(){
//     MapManager::show_map();
// }
