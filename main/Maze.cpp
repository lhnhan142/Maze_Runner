#include "Maze.h"
#include <random>
#include <ctime>
#include <functional>
#include <algorithm>
#include <iostream>
#include <numeric>           // Dùng cho std::iota (để khởi tạo DSU)
#include <tuple>             // Dùng cho std::tuple (để lưu danh sách tường)

	void generate() {
		// ---- PHẦN 1: THIẾT LẬP KRUSKAL ----

		// Cấu trúc DSU (Disjoint Set Union)
		// Dùng để theo dõi xem các ô đã được nối với nhau chưa
		struct DSU {
			std::vector<int> parent;
			DSU(int n) : parent(n) {
				// Ban đầu, mỗi ô là "đảo" của riêng nó (parent của nó là chính nó)
				std::iota(parent.begin(), parent.end(), 0);
			}

			// Tìm "đảo" (set) gốc của một ô (có nén đường dẫn)
			int find(int i) {
				if (parent[i] == i)
					return i;
				return parent[i] = find(parent[i]);
			}

			// Nối (hợp nhất) hai "đảo" lại làm một
			void unite(int i, int j) {
				int root_i = find(i);
				int root_j = find(j);
				if (root_i != root_j) {
					parent[root_i] = root_j;
				}
			}
		};

		std::mt19937 gen(time(0)); // Bộ tạo số ngẫu nhiên

		// Tính toán số lượng ô lối đi (các ô có tọa độ lẻ)
		int numCellsX = (w - 1) / 2;
		int numCellsY = (h - 1) / 2;
		int numCells = numCellsX * numCellsY;
		DSU dsu(numCells);

		// Hàm lambda tiện ích: chuyển tọa độ (x, y) của ô lối đi
		// thành chỉ số (index) trong DSU
		auto getDsuIndex = [&](int x, int y) {
			int cellX = (x - 1) / 2;
			int cellY = (y - 1) / 2;
			return cellY * numCellsX + cellX;
			};

		// Danh sách tất cả các bức tường nội bộ có thể bị đục
		// Lưu dưới dạng: (tọa độ x, tọa độ y, index ô 1, index ô 2)
		std::vector<std::tuple<int, int, int, int>> wallList;

		// ---- PHẦN 2: THU THẬP VÀ XÁO TRỘN TƯỜNG ----

		// Đặt tất cả các ô lối đi là 'false' (lối đi)
		// Thuật toán Kruskal sẽ đục các tường 'true' để nối chúng lại
		for (int y = 1; y < h - 1; y += 2) {
			for (int x = 1; x < w - 1; x += 2) {
				walls[y][x] = false;
			}
		}

		// Thu thập tất cả các bức tường ngăn cách
		for (int y = 1; y < h - 1; y++) {
			for (int x = 1; x < w - 1; x++) {
				// Chỉ quan tâm đến các ô đang là tường ('true')
				if (walls[y][x]) {
					bool isHorizontalSeparator = (y % 2 == 0) && (x % 2 != 0);
					bool isVerticalSeparator = (y % 2 != 0) && (x % 2 == 0);

					if (isHorizontalSeparator) {
						// Tường ngang, ngăn cách ô (x, y-1) và (x, y+1)
						int cell1 = getDsuIndex(x, y - 1);
						int cell2 = getDsuIndex(x, y + 1);
						wallList.emplace_back(x, y, cell1, cell2);
					}
					else if (isVerticalSeparator) {
						// Tường dọc, ngăn cách ô (x-1, y) và (x+1, y)
						int cell1 = getDsuIndex(x - 1, y);
						int cell2 = getDsuIndex(x + 1, y);
						wallList.emplace_back(x, y, cell1, cell2);
					}
				}
			}
		}

		// Xáo trộn ngẫu nhiên danh sách tường
		std::shuffle(wallList.begin(), wallList.end(), gen);

		// ---- PHẦN 3: CHẠY THUẬT TOÁN KRUSKAL ----

		// Duyệt qua từng bức tường đã xáo trộn
		for (const auto& wall : wallList) {
			int x, y, cell1_idx, cell2_idx;
			std::tie(x, y, cell1_idx, cell2_idx) = wall;

			// Kiểm tra xem 2 ô mà bức tường này ngăn cách
			// đã được nối với nhau chưa
			if (dsu.find(cell1_idx) != dsu.find(cell2_idx)) {
				// Nếu chưa, đục bức tường này
				walls[y][x] = false;
				// Và nối (hợp nhất) 2 "đảo" này lại
				dsu.unite(cell1_idx, cell2_idx);
			}
		}

		// ---- PHẦN 4: (TÙY CHỌN) TẠO THÊM VÒNG LẶP (Giữ từ code cũ của bạn) ----
		std::uniform_real_distribution<> loopDis(0, 1);
		double loopProbability = 0.12;
		double edgeLoopProbability = 0.3;

		for (int y = 1; y < h - 1; y++) {
			for (int x = 1; x < w - 1; x++) {
				if (walls[y][x]) {
					bool isHorizontalSeparator = (y % 2 == 0) && (x % 2 != 0);
					bool isVerticalSeparator = (y % 2 != 0) && (x % 2 == 0);

					if (isHorizontalSeparator || isVerticalSeparator) {
						bool isEdgeWall = (x == w - 3) || (y == h - 3);
						double currentProbability = isEdgeWall ? edgeLoopProbability : loopProbability;

						if (loopDis(gen) < currentProbability) {
							walls[y][x] = false;
						}
					}
				}
			}
		}

		// ---- PHẦN 5: ĐẶT LỐI VÀO / LỐI RA (Giữ từ code cũ của bạn) ----
		std::uniform_int_distribution<> randX_start(1, w - 2);
		int startX = randX_start(gen) | 1;
		int startY = h - 1;
		entrance = { startX, startY };
		walls[startY][startX] = false;

		std::uniform_int_distribution<> randX_exit(2, w - 4);
		int exitX = randX_exit(gen) | 1;
		int exitY = 0;
		walls[exitY][exitX] = false;
		exit = { exitX, exitY };
	}

	// Hàm rải bẫy (đỏ và vàng) một cách ngẫu nhiên lên các ô lối đi
	void generateTraps(double redP, double yellowP) {
		std::mt19937 gen(time(0));
		std::uniform_real_distribution<> dis(0, 1);
		std::uniform_int_distribution<> dirDis(0, 1);
		// Rải bẫy đỏ (đơn lẻ)
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				if (!walls[y][x]) {
					if (dis(gen) < redP)
						traps[y][x] = 1;
				}
			}
		}
		// Rải bẫy vàng (theo vệt 3 ô)
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				if (!walls[y][x] && traps[y][x] == 0) {
					if (dis(gen) < yellowP) {
						int direction = dirDis(gen); // 0 = ngang, 1 = dọc
						if (direction == 0) {
							if (x + 2 < w &&
								!walls[y][x + 1] && traps[y][x + 1] == 0 &&
								!walls[y][x + 2] && traps[y][x + 2] == 0) {
								traps[y][x] = 2; traps[y][x + 1] = 2; traps[y][x + 2] = 2;
							}
						}
						else {
							if (y + 2 < h &&
								!walls[y + 1][x] && traps[y + 1][x] == 0 &&
								!walls[y + 2][x] && traps[y + 2][x] == 0) {
								traps[y][x] = 2; traps[y + 1][x] = 2; traps[y + 2][x] = 2;
							}
						}
					}
				}
			}
		}
		// Đảm bảo lối vào và lối ra không bao giờ có bẫy
		traps[entrance.second][entrance.first] = 0;
		traps[exit.second][exit.first] = 0;
	}
};
