#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include "tiff.h"
#include "tiffio.h"
#include <vector>
#include <Atts.h>
#include <DBOperator.h>
#include <RLCProducer.h>
#include <time.h>
using namespace std;
typedef unsigned char BYTE;



int _tmain(int argc, _TCHAR* argv[])
{
	clock_t startTime, endTime;
	startTime = clock();

	RLCProducer rlc("C:/Users/93866/Documents/Visual Studio 2015/Projects/crop/crop/butler/","2010","2016");
	rlc.produceTIFF("C:/Users/93866/Documents/Visual Studio 2015/Projects/crop/crop/butler/", "2010", "2016");
	//rlc.convertOriginalLine2VData();
	//rlc.convertTIFF2RLC();
	//rlc.convertSortRLC2VData();
	//rlc.intersectRLC();
	//rlc.compressRLC();
	//rlc.convertRLC2VData("C:/Users/93866/Documents/Visual Studio 2015/Projects/crop/crop/CDL_1910/compress_result.txt");
	/*
	TIFF *tifFile;
	DBOperator db;
	db.doConnect();
	
	int currentYear = 2016;
	string tifFileName = "C:/Users/93866/Documents/Visual Studio 2015/Projects/crop/crop/" + to_string(currentYear) + ".tif";
	if ((tifFile = TIFFOpen(tifFileName.c_str(), "r")) != NULL) {                      //TIFFOpen的参数使用你的tiff文件路径和名称
		int width, height;
		TIFFGetField(tifFile, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tifFile, TIFFTAG_IMAGELENGTH, &height);
		int pixelCount = width * height;
		cout << "width=" << width << endl;
		cout << "pixelCount=" << pixelCount << endl;

		uint32* image = (uint32*)malloc(pixelCount * sizeof(uint32));
		TIFFReadRGBAImage(tifFile, width, height, image, 1);
		BYTE* R = new BYTE[pixelCount];    //为存放数据分配内存空间
		BYTE* G = new BYTE[pixelCount];    //为存放数据分配内存空间
		BYTE* B = new BYTE[pixelCount];    //为存放数据分配内存空间
		uint32 *rowPointerToSrc = image + (height - 1)*width;
		BYTE *rowPointerToR = R;
		BYTE *rowPointerToG = G;
		BYTE *rowPointerToB = B;
		cout << "Begin..." << endl;
		Atts atts;
		atts.loadDataFromFile("C:/Users/93866/Documents/Visual Studio 2015/Projects/crop/crop/attcomma.txt");//读取属性文件
		//rowPointerToSrc -= width * 4000;
		long  f_id = 1;
		//左上(270,65)，右下(321,117)
		for (int y = width-1; y >= 0; --y)
		{
			uint32 *colPointerToSrc = rowPointerToSrc;
			BYTE *colPointerToR = rowPointerToR;
			BYTE *colPointerToG = rowPointerToG;
			BYTE *colPointerToB = rowPointerToB;
			for (int x = 0; x <= height - 1; ++x)
			{
				colPointerToR[0] = (BYTE)TIFFGetR(colPointerToSrc[0]);//获取R通道
				colPointerToG[0] = (BYTE)TIFFGetG(colPointerToSrc[0]);//获取G通道
				colPointerToB[0] = (BYTE)TIFFGetB(colPointerToSrc[0]);//获取B通道
				//cout << "x=" << x << "," << "y=" << y << ":" << atts.getCropIdByRGB((int)colPointerToR[0], (int)colPointerToG[0], (int)colPointerToB[0]) << endl;
				//cout << "(" << (int)colPointerToR[0] << "," << (int)colPointerToG[0] << "," << (int)colPointerToB[0] << "," << atts.getCropIdByRGB((int)colPointerToR[0], (int)colPointerToG[0], (int)colPointerToB[0])<< "),";
				int cropId = atts.getCropIdByRGB((int)colPointerToR[0], (int)colPointerToG[0], (int)colPointerToB[0]);
				if(cropId!=0)
				  db.doQuery("insert into t_crop_"+to_string(currentYear)+" values("+ to_string(f_id)+","+ to_string(x)+","+ to_string(y)+","+ to_string(cropId)+","+ to_string((int)colPointerToR[0]) +","+ to_string((int)colPointerToG[0]) +","+ to_string((int)colPointerToB[0]) +")");
				colPointerToR++;
				colPointerToG++;
				colPointerToB++;
				colPointerToSrc++;
				f_id++;
			}
			rowPointerToSrc -= width;
			rowPointerToR += width;
			rowPointerToG += width;
			rowPointerToB += width;
			
		}
		delete(image);
		delete(R);
		delete(G);
		delete(B);
	}
	
	cout << "Done!" << endl;
	*/
	endTime = clock();
	cout << "Totle Time : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	system("pause");

	return 0;
}

