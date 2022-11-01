#pragma once
#include<Eigen/LU>
#include<set>
#define KMEANS_K 64
using namespace Eigen;
class Ckmeans
{
public:
	Ckmeans() { Datas = new std::vector<Vector3d>; }
	Ckmeans(std::vector<Vector3d> *Datas) : Datas(Datas) {}
	void add_Data(Vector3d d) { Datas->push_back(d); }
	Vector3d mu[KMEANS_K];
	std::pair<int,double> k_argmin(const Vector3d& x)
	{
		int kClass = 0;
		double dis = (x - mu[0]).squaredNorm(), ndis;
		for (int k = 1; k < KMEANS_K; ++k)
			if ((ndis = (x - mu[k]).squaredNorm()) < dis)
				dis = ndis, kClass = k;
		return std::make_pair(kClass, dis);
	}
	void Work();
private:
	std::vector<Vector3d> *Datas;
};