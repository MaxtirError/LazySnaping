#include "LazySnap.h"
#include<iostream>
void LazySnap::Initialize()
{
	CWatershed Water(W, H, Image);
	Water.Classify(class_num, be, Region, Edges);
	std::cout << class_num << std::endl;
	Type = new int[class_num];
	for (int i = 0; i < class_num; ++i)
		Type[i] = 2;
	for (Pt p : TB)
	{
		Type[be[p.first][p.second]] = 0;
	}
	for (Pt p : TF)
		Type[be[p.first][p.second]] = 1;
	nodes_id = new int[class_num];
	Color = new Vector3d[class_num];
	for (int i = 0; i < class_num; ++i)
	{
		Color[i].setZero();
		if (Type[i] == 2) 
		{
			nodes_id[i] = nodes.size();
			nodes.push_back(i);
		}
		for (auto p : Region[i]) 
		{
			Color[i] += Image[p.first][p.second];
			//if (Type[i] != 2) 
			//	kmeans[Type[i]].add_Data(Image[p.first][p.second]);
		}
		Color[i] /= Region[i].size();
		if (Type[i] != 2)
			kmeans[Type[i]].add_Data(Color[i]);
	}
	kmeans[0].Work();
	kmeans[1].Work();
}

void LazySnap::Cut()
{
	int nodes_num = nodes.size();
	int edge_num = 0;
	for (int i = 0;i < class_num; ++i)
		if(Type[i] == 2)
			edge_num += Edges[i].size() * 2;
	std::cout << edge_num << std::endl;
	Graph_DDD Graph(nodes_num, edge_num);
	Graph.add_node(nodes_num);
	for (int uid = 0; uid < nodes_num; ++uid)
	{
		int u = nodes[uid];
		double db = sqrt(kmeans[0].k_argmin(Color[u]).second);
		double df = sqrt(kmeans[1].k_argmin(Color[u]).second);
		double d[2] = { db / (db + df), df / (db + df) };
		for (auto v : Edges[u])
		{
			int vid = nodes_id[v];
			double w = lambda * 1.0 / ((Color[u] - Color[v]).squaredNorm() + 1);
			if (Type[v] == 2)
				Graph.add_edge(uid, vid, w, 0);
			else
				d[!Type[v]] += w * 2.0;
		}
		Graph.add_tweights(uid, d[0], d[1]);
	}
	Graph.maxflow();

	for (int uid = 0; uid < nodes_num; ++uid)
		Type[nodes[uid]] = Graph.what_segment(uid) == Graph_DDD::SOURCE;
}


void LazySnap::Get_alpha(bool** &alpha)
{
	alpha = new bool*[W];
	for (int i = 0; i < W; ++i)
		alpha[i] = new bool[H];
	for (int i = 0; i < class_num; ++i)
	{
		for (auto p : Region[i])
			alpha[p.first][p.second] = Type[i];
	}
}