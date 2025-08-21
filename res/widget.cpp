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
    , m_initialStartPoint(nullptr) // 新增：初始化初始起点为空指针
{
    ui->setupUi(this);

    // 连接定时器信号
    connect(animationTimer, &QTimer::timeout, this, &Widget::updateAnimation);

    // 设置定时器间隔为100毫秒
    animationTimer->setInterval(100);

    // 在Widget构造函数中连接按钮信号
    connect(ui->findPathButton, &QPushButton::clicked, this, &Widget::startPathfinding);

    //初始化激光雷达
    m_lidarSensor.init(120.0f, 60);  // 120度探测角度，60像素探测范围

    // 新增：创建激光雷达显示窗口
    m_lidarDisplay = new LidarDisplay();
    m_lidarDisplay->setWindowTitle("A* Path Finding");
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
    // 新增：清理历史路径和初始起点，防止内存泄露
    AStar::clearPath(m_historicalPath);
    if (m_initialStartPoint) {
        delete m_initialStartPoint;
        m_initialStartPoint = nullptr;
    }
}

void Widget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    
    // 获取地图图像
    QImage mapImg = MapManager::renderMap();
    if (mapImg.isNull()) {
        painter.drawText(rect(), Qt::AlignCenter, "Map Unload");
        return;
    }
    
    // 缩放图像以适应窗口
    QImage scaledImg = mapImg.scaled(size(), Qt::KeepAspectRatio);
    painter.drawImage(0, 0, scaledImg);
    
    // 如果有路径，绘制路径
    if (!path.empty() || !m_historicalPath.empty()) {
        // 计算缩放比例
        float scaleX = (float)scaledImg.width() / mapImg.width();
        float scaleY = (float)scaledImg.height() / mapImg.height();

        // 新增：绘制历史路径
        if (!m_historicalPath.empty()) {
            painter.setPen(QPen(Qt::gray, 2, Qt::DotLine)); // 使用灰色虚线绘制历史路径
            for (size_t i = 1; i < m_historicalPath.size(); ++i) {
                int x1 = m_historicalPath[i-1]->x * scaleX + scaleX/2;
                int y1 = m_historicalPath[i-1]->y * scaleY + scaleY/2;
                int x2 = m_historicalPath[i]->x * scaleX + scaleX/2;
                int y2 = m_historicalPath[i]->y * scaleY + scaleY/2;
                painter.drawLine(x1, y1, x2, y2);
            }
        }

        // 绘制当前规划的路径
        if (!path.empty()){
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
        
        }
        
        // 新增：在地图上标记最开始的起点
        if (m_initialStartPoint) {
            painter.setBrush(QBrush(Qt::red));
            painter.setPen(QPen(Qt::darkRed, 2));
            int startX = m_initialStartPoint->x * scaleX + scaleX/2;
            int startY = m_initialStartPoint->y * scaleY + scaleY/2;
            painter.drawEllipse(startX - 4, startY - 4, 8, 8);
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
    if (MapManager::loadMap("E:/Qtproject/A_star/test_7.map")) {
        // 将所有障碍物标记为未探测状态
        const RMap& mapData = MapManager::getMapData();
        for (int y = 0; y < mapData.m_my; y++) {
            for (int x = 0; x < mapData.m_mx; x++) {
                int idx = y * mapData.m_mx + x;
                if (mapData.m_map[idx] == RMAP_OBSTACLE) {
                    MapManager::updateMapPoint(x, y, RMAP_OBSTACLE_UNDESCOVERED);
                }
            }
        }
        
        // 重置激光雷达探测
        m_lidarSensor.init(120.0f, 30);
        
        update(); // 触发重绘
    }
}


//寻路算法入口
void Widget::startPathfinding() {
    // 清除之前的路径
    AStar::clearPath(path);
    // 新增：如果是第一次寻路，则记录初始起点并清空历史路径
    if (!m_initialStartPoint && !path.empty()) {
        m_initialStartPoint = new Node(path[0]->x, path[0]->y);
    }
    // 新增：每次开始新的寻路时，清空历史路径
    AStar::clearPath(m_historicalPath);
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

//更新路径规划动画
void Widget::updateAnimation() {
    if (currentPathIndex < path.size() - 1) {
        currentPathIndex++;
        
        // 检查路径碰撞
        if (checkPathCollision()) {
            // 停止当前动画
            animationTimer->stop();
            
            // 重新规划路径
            replanPath();
        } else {
            // 继续动画
            update(); // 触发重绘
        }
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
        m_lidarSensor.getDetctedEdges(),
        QPoint(path[currentPathIndex]->x, path[currentPathIndex]->y),
        m_droneHeading
    );
}

//检查规划路径碰撞
bool Widget::checkPathCollision()
{
    if (path.empty() || currentPathIndex >= path.size()) {
        return false;
    }
    
    const RMap& mapData = MapManager::getMapData();
    
    // 检查从当前位置到路径终点的所有点
    for (size_t i = currentPathIndex; i < path.size(); i++) {
        int x = path[i]->x;
        int y = path[i]->y;
        
        // 检查是否为已探测障碍物
        if (MapManager::isDetectedObstacle(x, y)) {
            return true;
        }
        
        // 检查路径点周围的区域（安全边界）
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = x + dx;
                int ny = y + dy;
                
                if (nx >= 0 && nx < mapData.m_mx && ny >= 0 && ny < mapData.m_my) {
                    if (MapManager::isDetectedObstacle(nx, ny)) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

//重新规划路径
void Widget::replanPath()
{
    // 保存当前位置作为新的起点
    if (currentPathIndex < path.size()) {
        // 新增：将已经走过的路径段保存到历史路径中
        for (size_t i = 0; i <= currentPathIndex; ++i) {
            m_historicalPath.push_back(new Node(path[i]->x, path[i]->y));
        }

        int currentX = path[currentPathIndex]->x;
        int currentY = path[currentPathIndex]->y;

        //获取地图数据
        const RMap& mapData = MapManager::getMapData();
        
        // 使用MapManager的方法更新起点
        MapManager::setStartPoint(currentX, currentY);
        
        // 清除旧路径
        AStar::clearPath(path);
        currentPathIndex = 0;
        
        // 重新规划路径
        if (AStar::findPath(mapData, path)) {
            // 继续动画
            isAnimating = true;
            animationTimer->start();
        } else {
            // 无法找到新路径
            isAnimating = false;
            update();
        }
    }
}

// void Widget::showMap(){
//     MapManager::show_map();
// }
