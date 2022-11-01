#pragma once
#include <QtGui>
#include<vector>
#include<iostream>
typedef std::vector<QPoint> PtList;

class CBrushes {
public:
	CBrushes(QPoint start, bool type, int radius) : type(type), radius(radius)
	{
		trace.push_back(start);
	}
	void MoveTo(QPoint pt)
	{
		if (Current == pt)
			return;
		Current = pt;
		trace.push_back(Current);
	}
	void Draw(QPainter& painter)
	{
		QPen pen;
		pen.setStyle(Qt::SolidLine);
		pen.setBrush(type ? Qt::blue : Qt::red);
		//std::cout << radius << std::endl;
		pen.setWidth(radius);
		painter.setPen(pen);
		for (auto p : trace)
			painter.drawPoint(p);
	}
	void Get_Inside(std::vector<std::pair<int, int>>& TB, std::vector<std::pair<int, int>>& TF, 
		QPoint start, int W, int H)
	{
		for (auto p : trace)
		{
			p -= start;
			int px = p.x(), py = p.y();
			for (int i = -radius; i <= radius; ++i) 
			{
				int r = sqrt(radius * radius - i * i);
				for (int j = -r; j <= r; ++j)
				{
					int nx = px + i, ny = py + j;
					if (nx < 0 || nx >= W || ny < 0 || ny >= H)
						continue;
					if (!type)
						TB.push_back(std::make_pair(nx, ny));
					else
						TF.push_back(std::make_pair(nx, ny));
				}
			}
		}
	}

private:
	int radius;
	bool type;
	PtList trace;
	QPoint Current;
};

