
//A*寻路算法封装
//private: A*核心算法、
//public: 写入地图

#ifndef ASTAR_H
#define ASTAR_H

#include "map_manager.h"
#include <vector>
#include <cmath>
#include <algorithm>

struct Node {
    int x, y;
    float g, h, f;
    Node* parent;
    
    Node(int x, int y) : x(x), y(y), g(0), h(0), f(0), parent(nullptr) {}
    
    // 计算到目标点的启发式距离（曼哈顿距离）
    float calculateHeuristic(int endX, int endY) {
        return abs(x - endX) + abs(y - endY);
    }
    
    // 检查两个节点是否相同
    bool operator==(const Node& other) const {
        return x == other.x && y == other.y;
    }
};

class AStar {
public:
    static bool findPath(const RMap& map, std::vector<Node*>& path);
    static void clearPath(std::vector<Node*>& path);
    
private:
    static bool isValid(int x, int y, const RMap& map);
    static float calculateDistance(int x1, int y1, int x2, int y2);
    static std::vector<Node*> getNeighbors(Node* node, const RMap& map);
    static Node* findNodeInList(const std::vector<Node*>& list, int x, int y);
};

#endif // ASTAR_H
