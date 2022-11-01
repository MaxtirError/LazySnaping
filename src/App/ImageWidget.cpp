#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#define BRUSH_SIZE 10
using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	draw_status = false;
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	ChooseStatus = kNone;
	shape = NULL;
	Image_RGB = NULL;
}


ImageWidget::~ImageWidget(void)
{
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);

	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	image_start_position.setX((width() - ptr_image_->width()) / 2);
	image_start_position.setY((height() - ptr_image_->height()) / 2);
	QRect rect = QRect( (width()-ptr_image_->width())/2, (height()-ptr_image_->height())/2, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_);


	// Draw choose region
	for (auto shape : shapes)
		shape.Draw(painter);
	if (shape != NULL)
		shape->Draw(painter);

	painter.end();
}


void ImageWidget::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button() && ChooseStatus != kNone)
	{
		shape = new CBrushes(event->pos(), ChooseStatus == kForeground, BRUSH_SIZE);
		draw_status = true;
	}
	update();
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status)
	{
		assert(shape != NULL);
		shape->MoveTo(event->pos());
	}
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (draw_status) 
	{
		draw_status = false;
		assert(shape != NULL);
		shapes.push_back(*shape);
		delete shape;
		shape = NULL;
		update();
	}
}


void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	ChooseStatus = kNone;
	update();

}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else			//仅水平翻转			
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)		//仅垂直翻转
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	ChooseStatus = kNone;
	update();
}

void ImageWidget::TurnGray()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}

	ChooseStatus = kNone;
	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	ChooseStatus = kNone;
	update();
}

void ImageWidget::ChooseBackground()
{
	ChooseStatus = kBackground;
}

void ImageWidget::ChooseForeground()
{
	ChooseStatus = kForeground;
}


void ImageWidget::GetImageData()
{
	image_width = ptr_image_->width();
	image_height = ptr_image_->height();
	Image_RGB = new Vector3d * [image_width];
	for (int i = 0; i < image_width; ++i) {
		Image_RGB[i] = new Vector3d[image_height];
		//mask[i] = new bool[image_height] {0};
		for (int j = 0; j < image_height; ++j) {
			auto z = ptr_image_->pixel(i, j);
			Image_RGB[i][j] = Vector3d(GetRValue(z), GetGValue(z), GetBValue(z));
		}
	}
}

void ImageWidget::Watershed()
{
	GetImageData();
	//mask = new bool* [image_width];
	CWatershed Tool(image_width, image_height, Image_RGB);
	int class_num, ** be;
	std::vector<PtSet> Region;
	std::vector<std::set<int>> Edges;
	Tool.Classify(class_num, be, Region, Edges);
	std::cout << class_num << std::endl;
	QRgb *Color = new QRgb[class_num];
	srand(time(0));
	for (int i = 0; i < class_num; ++i) {
		Color[i] = qRgb(rand() % 256, rand() % 256, rand() % 256);
	}
	for (int i = 0; i < image_width; ++i)
		for (int j = 0; j < image_height; ++j)
			ptr_image_->setPixel(i, j, Color[be[i][j]]);
	delete[] Color;
	for (int i = 0; i < image_width; ++i)
		delete[] be[i];
	delete[] be;
	for (int i = 0; i < image_width; ++i)
		delete[] Image_RGB[i];
	delete[] Image_RGB;
}

void ImageWidget::Cut()
{
	GetImageData();
	PtSet TB, TF;
	for (auto shape : shapes)
		shape.Get_Inside(TB, TF, image_start_position, image_width, image_height);
	LazySnap SnapTool(image_width, image_height, Image_RGB, TB, TF);
	SnapTool.Initialize();
	SnapTool.Cut();
	bool** alpha;
	SnapTool.Get_alpha(alpha);
	for (int i = 0; i < image_width; ++i)
		for (int j = 0; j < image_height; ++j)
			if (!alpha[i][j])
				ptr_image_->setPixel(i, j, qRgb(255, 255, 255));
	update();
	for (int i = 0; i < image_width; ++i)
		delete[] Image_RGB[i], delete[] alpha[i];
	delete[] Image_RGB;
	delete[] alpha;

}

void ImageWidget::Clear()
{
	shapes.clear();
	update();
}