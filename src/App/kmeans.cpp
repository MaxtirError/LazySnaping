#include "kmeans.h"
#include<iostream>
void Ckmeans::Work()
{
	srand(time(0));
	//initialize mu
	auto& X = *Datas;
	int n = X.size();
	std::cout << n << std::endl;
	mu[0] = X[0];
	double* dis = new double[n]{0};
	for (int i = 1; i < n; ++i)
	{
		if (mu[0].squaredNorm() < X[i].squaredNorm())
			mu[0] = X[i];
	}
	for (int i = 0; i < n; ++i)
		dis[i] = (mu[0] - X[i]).squaredNorm();
	for (int k = 1; k < KMEANS_K; ++k)
	{
		int mx = 0;
		for (int i = 0; i < n; ++i)
			if (dis[i] > dis[mx])
				mx = i;
		mu[k] = X[mx];
		for (int i = 0; i < n; ++i)
			dis[i] = std::min(dis[i], (mu[k] - X[i]).squaredNorm());
	}
	delete[] dis;

	Vector3d nmu[KMEANS_K];
	int cnt[KMEANS_K];
	double Energy = 1e9, nEnergy;
	for (int iter = 0; iter < 50; ++iter)
	{
		for (int i = 0; i < KMEANS_K; ++i)
			nmu[i].setZero(), cnt[i] = 0;
		nEnergy = 0;

		for (int i = 0; i < n; ++i)
		{
			auto x = k_argmin(X[i]);
			nmu[x.first] += X[i];
			cnt[x.first]++;
			nEnergy += x.second;
		}
		for (int k = 0; k < KMEANS_K; ++k)
			if (!cnt[k])
				mu[k].setZero();
			else
				mu[k] = nmu[k] / cnt[k];

		if ((Energy - nEnergy) / Energy < 0.001)
			break;

		Energy = nEnergy;
	}
}