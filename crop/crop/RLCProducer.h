#pragma once
#include <string>
#include <iostream>
#include "tiff.h"
#include "tiffio.h"
#include <vector>
#include <map>
#include <Atts.h>
#include <algorithm>
#include <cstdlib>
#include <windows.h>
using namespace std;
typedef unsigned char BYTE;
extern void SplitString(const string& s, vector<string>& v, const string& c);
class RLCProducer
{
private:
	string intersectRLCLine(const string& sourceRLC, string& newRLC);
	string fileDir;//数据文件所在目录
	string startYear;
	string endYear;
	int width, height;//文件的行列数；
public:
	RLCProducer(string fileDir, string startYear, string endYear);
	~RLCProducer();
	void convertTIFF2RLC();
//private:
	void intersectRLC();
	void convertRLC2VData();
	void convertSortRLC2VData();
	void compressRLC();
	void compressRLCLine(vector<string>& RLCVector);
	void compressRLCLine(map<string,int>& RLCMap,string RLCline);
	void compressRLCBetweenLines(vector<string>& firstRLCVector, vector<string>& secondRLCVector);
	bool combineRLCItem(string &firstItem, string secondItem);
	void updateRLCMap(map<string, int>& RLCMap, string RLCline);
	void convertOriginal2VData();
	void convertOriginalLine2VData();
	void produceTIFF(string fileDir,string year, string pattern);
};

