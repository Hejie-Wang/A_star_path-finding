
//A*寻路算法
//调用MapManager中的getMapData方法获得地图数据
//规划一条从起点到终点的避开障碍物的路径
//路径写入地图显示，最后不断调用update刷新地图显示，并更新飞机此时的位置（GUI界面操作）

#include "a_star.h"

bool AStar::findPath(const RMap& map, std::vector<Node*>& path) {
    if (!map.m_map) return false;
    
    // 起点和终点
    Node* startNode = new Node(map.m_startX, map.m_startY);
    Node* endNode = new Node(map.m_endX, map.m_endY);
    
    // 开放列表和关闭列表
    std::vector<Node*> openList;
    std::vector<Node*> closedList;
    
    // 将起点加入开放列表
    openList.push_back(startNode);
    
    while (!openList.empty()) {
        // 找到f值最小的节点
        Node* currentNode = openList[0];
        int currentIndex = 0;
        
        for (size_t i = 1; i < openList.size(); i++) {
            if (openList[i]->f < currentNode->f) {
                currentNode = openList[i];
                currentIndex = i;
            }
        }
        
        // 将当前节点从开放列表移到关闭列表
        openList.erase(openList.begin() + currentIndex);
        closedList.push_back(currentNode);
        
        // 找到目标点
        if (*currentNode == *endNode) {
            // 回溯路径
            Node* current = currentNode;
            while (current != nullptr) {
                path.push_back(current);
                current = current->parent;
            }
            
            // 反转路径，从起点到终点
            std::reverse(path.begin(), path.end());
            
            // 清理内存
            delete endNode;
            for (Node* node : openList) {
                delete node;
            }

            // 清理 closedList 中不属于最终路径的节点
            for (Node* node : closedList) {
                bool inPath = false;
                for (Node* pathNode : path) {
                    if (node == pathNode) {
                        inPath = true;
                        break;
                    }
                }
                if (!inPath) {
                    delete node;
                }
            }
            
            return true;
        }
        
        // 获取邻居节点
        std::vector<Node*> neighbors = getNeighbors(currentNode, map);
        
        for (Node* neighbor : neighbors) {
            // 如果邻居在关闭列表中，跳过
            if (findNodeInList(closedList, neighbor->x, neighbor->y)) {
                delete neighbor;
                continue;
            }
            
            // 计算新的g值
            float newG = currentNode->g + calculateDistance(currentNode->x, currentNode->y, neighbor->x, neighbor->y);
            
            // 检查邻居是否在开放列表中
            Node* existingNode = findNodeInList(openList, neighbor->x, neighbor->y);
            
            if (!existingNode) {
                // 邻居不在开放列表中，添加它
                neighbor->g = newG;
                neighbor->h = neighbor->calculateHeuristic(endNode->x, endNode->y);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = currentNode;
                openList.push_back(neighbor);
            } else if (newG < existingNode->g) {
                // 找到更好的路径，更新节点
                existingNode->g = newG;
                existingNode->f = existingNode->g + existingNode->h;
                existingNode->parent = currentNode;
                delete neighbor;
            } else {
                delete neighbor;
            }
        }
    }
    
    // 没有找到路径
    delete endNode;
    for (Node* node : openList) {
        delete node;
    }
    for (Node* node : closedList) {
        delete node;
    }
    
    return false;
}

void AStar::clearPath(std::vector<Node*>& path) {
    for (Node* node : path) {
        delete node;
    }
    path.clear();
}

bool AStar::isValid(int x, int y, const RMap& map) {
    // 检查是否超出边界
    if (x < 0 || x >= map.m_mx || y < 0 || y >= map.m_my) {
        return false;
    }
    
    // 检查是否是障碍物
    int idx = y * map.m_mx + x;
    int mapValue = map.m_map[idx];
    
    // 已探测障碍物和障碍物边缘都视为不可通行
    return mapValue != RMAP_OBSTACLE_SCANNED &&
           mapValue != RMAP_OBSTACLE_EDGE;
}

float AStar::calculateDistance(int x1, int y1, int x2, int y2) {
    // 使用欧几里得距离
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

std::vector<Node*> AStar::getNeighbors(Node* node, const RMap& map) {
    std::vector<Node*> neighbors;
    
    // 8个方向
    int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    
    for (int i = 0; i < 8; i++) {
        int newX = node->x + dx[i];
        int newY = node->y + dy[i];
        
        if (isValid(newX, newY, map)) {
            neighbors.push_back(new Node(newX, newY));
        }
    }
    
    return neighbors;
}

Node* AStar::findNodeInList(const std::vector<Node*>& list, int x, int y) {
    for (Node* node : list) {
        if (node->x == x && node->y == y) {
            return node;
        }
    }
    return nullptr;
}

