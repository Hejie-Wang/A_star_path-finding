#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <random>
#include <algorithm>

// 定义地图文件结构
struct MapHeader {
    uint32_t magic = 0x15432345;
    int32_t width;
    int32_t height;
};

struct MapPoints {
    int32_t startX;
    int32_t startY;
    int32_t endX;
    int32_t endY;
};

// 定义地图中的元素类型
constexpr int8_t RMAP_FREE = 0;
constexpr int8_t RMAP_OBSTACLE = 1;

// 获取某个坐标周围的障碍物数量
int get_adjacent_walls(int x, int y, int width, int height, const std::vector<int8_t>& map) {
    int wall_count = 0;
    for (int j = y - 1; j <= y + 1; ++j) {
        for (int i = x - 1; i <= x + 1; ++i) {
            if (i == x && j == y) continue; // 跳过中心点
            if (i >= 0 && i < width && j >= 0 && j < height) { // 检查边界
                if (map[j * width + i] == RMAP_OBSTACLE) {
                    wall_count++;
                }
            } else {
                // 边界外视为墙壁，以封闭地图
                wall_count++;
            }
        }
    }
    return wall_count;
}

// 执行一步元胞自动机模拟
void simulation_step(int width, int height, std::vector<int8_t>& map) {
    std::vector<int8_t> new_map = map;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int neighbors = get_adjacent_walls(x, y, width, height, map);
            if (neighbors > 4) {
                new_map[y * width + x] = RMAP_OBSTACLE;
            } else if (neighbors < 4) {
                new_map[y * width + x] = RMAP_FREE;
            }
        }
    }
    map = new_map;
}


int main() {
    MapHeader header;
    header.width = 200;
    header.height = 200;

    // 随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);

    // 1. 初始化地图，随机填充障碍物
    std::vector<int8_t> mapData(header.width * header.height, RMAP_FREE);
    for (int i = 0; i < header.width * header.height; ++i) {
        if (dis(gen) < 45) { // 45% 的概率成为障碍物
            mapData[i] = RMAP_OBSTACLE;
        }
    }

    // 2. 运行元胞自动机模拟
    int simulation_steps = 5; // 迭代次数
    for (int i = 0; i < simulation_steps; ++i) {
        simulation_step(header.width, header.height, mapData);
    }
    
    // 3. 额外进行几步平滑处理 (规则稍有不同)
    int smoothing_steps = 3;
    for (int i = 0; i < smoothing_steps; i++) {
        std::vector<int8_t> tempMap = mapData;
        for (int y = 1; y < header.height - 1; y++) {
            for (int x = 1; x < header.width - 1; x++) {
                int wall_count = 0;
                for (int j = y - 1; j <= y + 1; j++) {
                    for (int k = x - 1; k <= x + 1; k++) {
                        if (mapData[j * header.width + k] == RMAP_OBSTACLE) wall_count++;
                    }
                }
                if (wall_count >= 5) tempMap[y * header.width + x] = RMAP_OBSTACLE;
                else tempMap[y * header.width + x] = RMAP_FREE;
            }
        }
        mapData = tempMap;
    }


    // 4. 设置起点和终点
    MapPoints points;
    std::uniform_int_distribution<> pos_dis(0, header.width -1);
    
    auto find_valid_point = [&]() {
        int x, y;
        do {
            x = pos_dis(gen);
            y = pos_dis(gen);
        } while (mapData[y * header.width + x] != RMAP_FREE);
        return std::make_pair(x, y);
    };

    auto p_start = find_valid_point();
    points.startX = p_start.first;
    points.startY = p_start.second;

    auto p_end = find_valid_point();
    points.endX = p_end.first;
    points.endY = p_end.second;
    

    // --- 写入到文件 ---
    std::ofstream outFile("maps/cave_map.map", std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open file for writing." << std::endl;
        return 1;
    }

    outFile.write(reinterpret_cast<const char*>(&header), sizeof(header));
    outFile.write(reinterpret_cast<const char*>(mapData.data()), mapData.size() * sizeof(int8_t));
    outFile.write(reinterpret_cast<const char*>(&points), sizeof(points));

    outFile.close();

    std::cout << "Successfully created maps/cave_map.map" << std::endl;

    return 0;
}