#pragma once
#include<Eigen/LU>
#include<set>
using namespace Eigen;
typedef std::pair<int, int> Pt;
typedef std::vector<Pt> PtSet;
class CWatershed
{
public:
	CWatershed();
	CWatershed(int width, int height, Vector3d** Image) : W(width), H(height), Image(Image) {}
	void Classify(int &class_num, int** &be, std::vector<PtSet> &Region, std::vector<std::set<int>> &Edges)
	{
		be = new int*[W];
		for (int i = 0; i < W; ++i) {
			be[i] = new int[H];
			for (int j = 0; j < H; ++j) {
				int Gray = Image[i][j].mean();
				Pts[Gray].push_back(std::make_pair(i, j));
				be[i][j] = -1;
			}
		}

		class_num = 0;
		for (int i = 0; i < 256; ++i)
		{
			for (auto x : Pts[i]) {
				int px = x.first, py = x.second;
				for (int d = 0; d < 4; ++d)
				{
					int nx = px + dx[d], ny = py + dy[d];
					if (nx < 0 || nx >= W || ny < 0 || ny >= H)
						continue;
					if (be[nx][ny] != -1)
					{
						if (be[px][py] == -1) 
						{
							be[px][py] = be[nx][ny];
							Region[be[px][py]].push_back(std::make_pair(px, py));
						}
						else if(be[nx][ny] != be[px][py])
						{
							Edges[be[nx][ny]].insert(be[px][py]);
							Edges[be[px][py]].insert(be[nx][ny]);
						}
					}
				}
				if (be[px][py] == -1)
				{
					Region.push_back(PtSet());
					Region[class_num].push_back(std::make_pair(px, py));
					be[px][py] = class_num++;
					Edges.push_back(std::set<int>());
				}
			}
		}
		_class_num = class_num;
		_be = be;
	}
private:
	const int dx[4] = { 0, 0, 1, -1 };
	const int dy[4] = { 1, -1, 0, 0 };
	Vector3d** Image;
	int W, H;
	int _class_num;
	int** _be;
	std::vector<std::pair<int, int>> Pts[256];
};