#pragma once
#include<QImage>
#include "maxflow.h"
#include "wartershed.h"
#include "kmeans.h"
using namespace maxflow;
class LazySnap
{
public:
	LazySnap(int W, int H, Vector3d **Image, PtSet&TB, PtSet&TF):
		W(W), H(H), Image(Image), TB(TB), TF(TF) {}
	void Initialize();
	void Cut();
	void Get_alpha(bool** &alpha);
private:
	const double lambda = 400;
	int W, H;
	Vector3d** Image;
	PtSet&TB;
	PtSet&TF;
	int* Type; // 2 for undetermined, 0 for background, 1 for foreground
	int class_num;
	int** be;
	std::vector<PtSet>Region;
	std::vector<std::set<int>>Edges;
	std::vector<int>nodes;
	int* nodes_id;
	Ckmeans kmeans[2]; //0 for backgournd ,1 for foreground
	Vector3d* Color;
};