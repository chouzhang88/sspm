#include "stdafx.h"
 
#include "RLCProducer.h"
 
bool cmp(string s1, string s2)
{
	vector<string> sv1;
	SplitString(s1, sv1, ",");
	int n1 = atoi(sv1[sv1.size() - 1].c_str());

	vector<string> sv2;
	SplitString(s2, sv2, ",");
	int n2 = atoi(sv2[sv2.size() - 1].c_str());

	if (n1 >= n2)
		return true;
	else
		return false;
}


RLCProducer::RLCProducer(string fileDir, string startYear, string endYear)
{
	this->fileDir = fileDir;
	this->startYear = startYear;
	this->endYear = endYear;
}


RLCProducer::~RLCProducer()
{
}
//读取tiff影像文件，转换为游程编码，存到txt
void RLCProducer::convertTIFF2RLC()
{
	int sYear = atoi(startYear.c_str());
	int eYear = atoi(endYear.c_str());
	cout << "===================================== Step 1: Begin converting TIFF data to RLC =====================================" << endl;

	for (int year = sYear; year <= eYear; year++)
	{
		TIFF *tifFile;         
		string inputFilename = fileDir+"/" +to_string(year)+".tif";
		ifstream iff;
		 
		cout << "===================================== converting "<<year<<" =====================================" << endl;
	
		string outputFileName = fileDir+"/RLC_"+ to_string(year) +".txt";
		
		if ((tifFile = TIFFOpen(inputFilename.c_str(), "r")) != NULL) {                      //TIFFOpen的参数使用你的tiff文件路径和名称
			
			TIFFGetField(tifFile, TIFFTAG_IMAGEWIDTH, &width);
			TIFFGetField(tifFile, TIFFTAG_IMAGELENGTH, &height);
			long int pixelCount = width * height;
			cout << "width=" << width << endl;
			cout << "height=" << height << endl;
			cout << "pixelCount=" << pixelCount << endl;

			uint32* image = (uint32*)malloc(pixelCount * sizeof(uint32));
			TIFFReadRGBAImage(tifFile, width, height, image, 1);
			//BYTE* R = new BYTE[pixelCount];    //为存放数据分配内存空间
			//BYTE* G = new BYTE[pixelCount];    //为存放数据分配内存空间
			//BYTE* B = new BYTE[pixelCount];    //为存放数据分配内存空间
			uint32 *rowPointerToSrc = image + (height - 1)*width;
			//BYTE *rowPointerToR = R;
			//BYTE *rowPointerToG = G;
			//BYTE *rowPointerToB = B;
			
			Atts atts;
			atts.loadDataFromFile("D:/attcomma.txt");//读取属性文件
																												 //rowPointerToSrc -= width * 4000;
			ofstream outFile;
			outFile.open(outputFileName);

			long  f_id = 1;
			//左上(270,65)，右下(321,117)
			//width = 1310;
			for (int y = height - 1; y >= 0; --y)
			{
				vector<vector<int>> RLCline;//存储每一行像素的游程编码
				uint32 *colPointerToSrc = rowPointerToSrc;
				//BYTE *colPointerToR = rowPointerToR;
				//BYTE *colPointerToG = rowPointerToG;
				//BYTE *colPointerToB = rowPointerToB;
				BYTE r;
				BYTE g;
				BYTE b;
				for (int x = 0; x <= width - 1; ++x)
				{
					__try
					{
						r = (BYTE)TIFFGetR(colPointerToSrc[0]);//获取R通道
						g = (BYTE)TIFFGetG(colPointerToSrc[0]);//获取G通道
						b = (BYTE)TIFFGetB(colPointerToSrc[0]);//获取B通道
																			  //cout << "x=" << x << "," << "y=" << y << ":" << atts.getCropIdByRGB((int)colPointerToR[0], (int)colPointerToG[0], (int)colPointerToB[0]) << endl;
																			  //cout << "(" << (int)colPointerToR[0] << "," << (int)colPointerToG[0] << "," << (int)colPointerToB[0] << "," << atts.getCropIdByRGB((int)colPointerToR[0], (int)colPointerToG[0], (int)colPointerToB[0])<< "),";
						int cropId = atts.getCropIdByRGB((int)r, (int)g, (int)b);
						//if (cropId != 0)//不是黑色像素
						//{
						if (x == 0) //每行的第一个像素
						{
							vector<int> v;
							v.push_back(cropId);
							v.push_back(1);
							RLCline.push_back(v);
						}
						else {
							vector<int> v = (vector<int>)RLCline[RLCline.size() - 1];
							if (cropId == v[0])
							{
								//((vector<int>)RLCline[RLCline.size() - 1])[1]++;
								v[1]++;
								RLCline[RLCline.size() - 1] = v;
							}
							else
							{
								vector<int> v;
								v.push_back(cropId);
								v.push_back(1);
								RLCline.push_back(v);
							}
						}
						//}
					}
					__except (EXCEPTION_EXECUTE_HANDLER) { cout << "access vialation exceptionn" << endl; }
					//colPointerToR++;
					//colPointerToG++;
					//colPointerToB++;
					colPointerToSrc++;
					f_id++;
				}

				//把每行的游程编码写入文件
				string tempLine;
				for (int i = 0; i < RLCline.size(); i++)
				{
					vector<int> v = (vector<int>)RLCline[i];
					tempLine += "(" + to_string(v[0]) + "," + to_string(v[1]) + ");";
				}
				outFile << tempLine << "\n";
				cout << y << ",";
				rowPointerToSrc -= width;
				//rowPointerToR += width;
				//rowPointerToG += width;
				//rowPointerToB += width;

			}
			free(image);
			//delete(R);
			//delete(G);
			//delete(B);
			TIFFClose(tifFile);
			outFile.close();
		}
	}
	cout << "===================================== Finish step 1 =====================================" << endl;
	intersectRLC();
	compressRLC();
	convertRLC2VData();
}
//基于行游程编码，把多年的影像进行叠加操作，即把同一行中，且临近的具有相同轮作规律的像素合并到一起
void RLCProducer::intersectRLC()
{
	//先把最初一年的RLC数据存入向量sourceRLC
	cout << "===================================== Step2: begin intersection between layers =====================================" << endl;
	ifstream ifRLC;
	ifRLC.open(fileDir+"RLC_"+startYear+".txt");
	if (!ifRLC)
		return;
	vector<string> sourceRLC;
	string line;
	while (getline(ifRLC, line))
	{
		sourceRLC.push_back(line);
	}
	ifRLC.close();
	//再读入每一年的RLC,依次与sourceRLC进行按行叠加操作；叠加完的结果存在sourceRLC里，再与下一年进行叠加
	int lineNumber = 0;
	int sYear = atoi(startYear.c_str());
	int eYear = atoi(endYear.c_str());
	for (int i = sYear+1; i <= eYear; i++)
	{
		cout << "========== intersect with "<<i<<" ===========" << endl;
		ifRLC.open(fileDir + "RLC_"+to_string(i)+".txt");
		lineNumber = 0;
		while (getline(ifRLC, line))
		{
			string newLine(line);
			string sourceLine = (string)sourceRLC[lineNumber];
			newLine = newLine.substr(0, newLine.length() - 1);//去掉尾巴的逗号
			sourceLine = sourceLine.substr(0, sourceLine.length() - 1);

			string mergedLine=intersectRLCLine(sourceLine, newLine);
			sourceRLC[lineNumber] = mergedLine;
	
			lineNumber++;
		}
		ifRLC.close();
	}
	ofstream outFile;
	outFile.open(fileDir+"/intersect_result.txt");
	for (int i = 0; i < sourceRLC.size(); i++)
	{
		string s = (string)sourceRLC[i];
		outFile << s << "\n";
	}
	outFile.close();
	cout << "===================================== finish step2 =====================================" << endl;
}
//通过将相邻行里、相同的游程编码进行合并，达到进一步压缩RLC数据文件的目的
void RLCProducer::compressRLC()
{
	ifstream ifRLC;
	ifRLC.open(fileDir + "/intersect_result.txt");
	if (!ifRLC)
		return;
	cout << "===================================== Step3: Begin compress RLC data =====================================" << endl;
	ofstream outFile;
	outFile.open(fileDir + "/compress_result.txt");
	//由于intersect_result文件里的每行是多年数据的叠加,申请的行空间可能不能由原文件的width决定
/*
	string firstLine;
	getline(ifRLC, firstLine);
	firstLine = firstLine.substr(0, firstLine.length() - 1);//去掉尾巴的分号
	vector<string> firstRLCVector;
	SplitString(firstLine, firstRLCVector, ";");
	compressRLCLine(firstRLCVector);
	 
	*/
	map<string, int> result;
	string secondLine;
	while (getline(ifRLC, secondLine))
	{

		secondLine = secondLine.substr(0, secondLine.length() - 1);//去掉尾巴的分号
		compressRLCLine(result, secondLine);
		/*
		vector<string> secondRLCVector;
		SplitString(secondLine, secondRLCVector, ";");
		compressRLCLine(secondRLCVector);

		compressRLCBetweenLines(firstRLCVector, secondRLCVector);

		for (int i = 0; i < firstRLCVector.size(); i++)
		{
			outFile << "(" << firstRLCVector[i] << ");";
		}
		
		
		outFile << "\n";

		firstRLCVector = secondRLCVector;*/
	}
	//合并后的最后一行，也要写入：
	/*for (int i = 0; i < firstRLCVector.size(); i++)
	{
		outFile << "(" << firstRLCVector[i] << ");";
	}*/
	map<string, int>::iterator it= result.begin();
	while (it != result.end())
	{
		outFile << "(" << it->first <<","<<it->second<< ");";
		it++;
	}
	outFile << "\n";
	outFile.close();
	ifRLC.close();
	cout << "===================================== Finish step 3 =====================================" << endl;
}
//把RLC几年叠加后的数据转化为prefixspan序列挖掘所需的数据格式
void RLCProducer::convertRLC2VData()
{
	ifstream ifRLC;
	ifRLC.open(fileDir+"/compress_result.txt");
	if (!ifRLC)
		return;
	cout << "===================================== Step4: begin convert RLC data to Sequence Database =====================================" << endl;
	ofstream outFile;
	outFile.open(fileDir+"/data_prefixSpan.txt");

	//由于intersect_result文件里的每行是多年数据的叠加,申请的行空间可能不能由原文件的width决定
	string line;

	int rowCount = 0;
	int groupId = 0;
	while (getline(ifRLC, line))
	{
		 
		line = line.substr(0, line.length() - 1);//去掉尾巴的分号

		vector<string> sourceVector;
		SplitString(line, sourceVector, ";");

		vector<string> itemVector;
		for (int i = 0; i < sourceVector.size(); i++)
		{
			string item = (string)sourceVector[i];
			item = item.substr(1, item.length() - 2);//去掉头尾的括号,如(88,18)变成88,18
			itemVector.clear();
			SplitString(item, itemVector, ",");//把88,18存到向量里
			for (int j = 0; j < itemVector.size() - 1; j++)
			{
				string cropId = (string)itemVector[j];
				if (cropId != "81" && cropId != "82" && cropId != "83" && cropId != "87" && cropId != "88" && cropId != "-1")
				{
					rowCount++;
					outFile << groupId << " " << (2010 + j) << " " << cropId << " " << itemVector[itemVector.size() - 1] << "\n";
				}
					
			}
			groupId++;
		}

	}
	outFile.close();
	ifRLC.close();
	cout << "===================================== Finish step 4 =====================================" << endl;
	cout << "total row=" <<rowCount<< endl;
}
//把RLC几年叠加后的数据，从大到小排序，转化为prefixspan序列挖掘所需的数据格式
void RLCProducer::convertSortRLC2VData()
{
	ifstream ifRLC;
	ifRLC.open(fileDir + "/compress_result.txt");
	if (!ifRLC)
		return;
	cout << "===================================== Step4: begin sort and convert RLC data to Sequence Database =====================================" << endl;
	ofstream outFile;
	outFile.open(fileDir + "/data_prefixSpan_sort.txt");

	//由于intersect_result文件里的每行是多年数据的叠加,申请的行空间可能不能由原文件的width决定
	string line;

	vector<string> outputVector;
	
	while (getline(ifRLC, line))
	{

		line = line.substr(0, line.length() - 1);//去掉尾巴的分号

		vector<string> sourceVector;
		SplitString(line, sourceVector, ";");

		for (int i = 0; i < sourceVector.size(); i++)
		{
			string item = (string)sourceVector[i];
			item = item.substr(1, item.length() - 2);//去掉头尾的括号,如(88,18)变成88,18
			
			outputVector.push_back(item);
			
			
		}

	}
	sort(outputVector.begin(), outputVector.end(), cmp);

	int groupId = 0;
	for (int i = 0; i < outputVector.size(); i++)
	{
		string item = (string)outputVector[i];
		vector<string> itemVector;
		SplitString(item, itemVector, ",");//把88,18存到向量里
		for (int j = 0; j < itemVector.size() - 1; j++)
		{
			string cropId = (string)itemVector[j];
			if (cropId != "81" && cropId != "82" && cropId != "83" && cropId != "87" && cropId != "88" && cropId != "-1")
				outFile << groupId << " " << (2010 + j) << " " << cropId << " " << itemVector[itemVector.size() - 1] << "\n";
		}
		groupId++;
	}

	outFile.close();
	ifRLC.close();
	cout << "===================================== Finish step 4 =====================================" << endl;
}
//对RLC文件中的一行游程编码进行自压缩，即合并同行里相同的游程编码；
void RLCProducer::compressRLCLine(vector<string>& RLCVector)
{
	vector<string>::iterator it = RLCVector.begin();
	while (it != RLCVector.end())
	{
		string RLCItem = (string)*it;
		//RLCItem = RLCItem.substr(1, RLCItem.length() - 2);
		
		vector<string>::iterator followIt = it + 1;
		while (followIt != RLCVector.end())
		{
			string followRLCItem = (string)*followIt;
			//followRLCItem = followRLCItem.substr(1, followRLCItem.length() - 2);

			if (combineRLCItem(RLCItem, followRLCItem) == true)
			{
				followIt=RLCVector.erase(followIt);
			}
			else
				followIt++;
		}
		*it = RLCItem;
		it++;
	}

}

void RLCProducer::compressRLCLine(map<string, int>& RLCMap, string RLCline)
{
	string c = ";";
	string::size_type pos1, pos2;
	pos2 = RLCline.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		string RLCItem = RLCline.substr(pos1, pos2 - pos1);
		updateRLCMap(RLCMap, RLCItem);

		pos1 = pos2 + c.size();
		pos2 = RLCline.find(c, pos1);
	}
	if (pos1 != RLCline.length())
	{
		string RLCItem = RLCline.substr(pos1);
		updateRLCMap(RLCMap, RLCItem);
	}
		 
}

void RLCProducer::updateRLCMap(map<string, int>& RLCMap, string RLCItem)
{
	RLCItem = RLCItem.substr(1, RLCItem.length() - 2);//去掉头尾的括号
	int position = RLCItem.find_last_of(",");
	string cropIds = RLCItem.substr(0, position);
	int number = atoi(RLCItem.substr(position + 1).c_str());


	if (RLCMap.count(cropIds) > 0)
	{
		RLCMap.find(cropIds)->second += number;

	}
	else
		RLCMap.insert(make_pair(cropIds, number));
}




//把两行RLC进行合并；且是把上一行合并到下一行，即把两行相同的RLC合并到下一行中，上一行只保留不同的RLC；这样有利于相邻行之间相同的RLC信息能逐行往下合并
void RLCProducer::compressRLCBetweenLines(vector<string>& firstRLCVector, vector<string>& secondRLCVector)
{
	vector<string>::iterator it=firstRLCVector.begin();

	while(it!= firstRLCVector.end())
	{
		string firstItem = (string)*it;
		bool ifCombined = false;
		for (int j = 0; j < secondRLCVector.size(); j++)
		{
			string secondItem = (string)secondRLCVector[j];
			if (combineRLCItem(secondItem, firstItem) == true)
			{
				it= firstRLCVector.erase(it);//把上一行中的相同RLC删掉
				secondRLCVector[j] = secondItem;//用合并过后的RLC更新第二行的RLC
				ifCombined = true;
				break;
			}
		}
		if(ifCombined==false)
		  it++;
	}
}

//对两个游程编码项进行合并。如果可以合并，返回真；
bool RLCProducer::combineRLCItem(string & firstItem, string secondItem)
{
	int firstPosition = firstItem.find_last_of(",");
	string firstCropIds = firstItem.substr(0, firstPosition);

	int secondPosition = secondItem.find_last_of(",");
	string secondCropIds = secondItem.substr(0, secondPosition);
	
	if (firstCropIds == secondCropIds)
	{
		int firstNumber = atoi(firstItem.substr(firstPosition+1).c_str());
		firstNumber+= atoi(secondItem.substr(firstPosition+1).c_str());
		firstItem = firstCropIds + "," + to_string(firstNumber);
		return true;
	}

	return false;
}




//对每行的游程编码进行叠加操作
string RLCProducer::intersectRLCLine(const string& sourceRLC, string& newRLC)
{
	string result = "";
	vector<string> sourceVector;
	SplitString(sourceRLC, sourceVector, ";");

	vector<string> newVector;
	SplitString(newRLC, newVector, ";");

	vector<string> newItemVector;
	vector<string> sourceItemVector;
	int dealedPixelNumber = 0;//正处理到sourceRLC中某一项中，已被合并叠加过的像素个数
	int currentItemIndex = 0;//正处理到sourceRLC的第几项的下标
	for (int i = 0; i < newVector.size(); i++)
	{
		string newItem = (string)newVector[i];
		newItem = newItem.substr(1, newItem.length() - 2);//去掉头尾的括号,如(88,18)变成88,18
		newItemVector.clear();
		SplitString(newItem, newItemVector, ",");//把88,18存到向量里
		string newCropId = newItemVector[0];
		int newCropNumber = atoi(newItemVector[1].c_str());

		while (newCropNumber > 0)//当前项的crop，是否还没处理完
		{
			string sourceItem = (string)sourceVector[currentItemIndex];
			sourceItem = sourceItem.substr(1, sourceItem.length() - 2);
			sourceItemVector.clear();
			SplitString(sourceItem, sourceItemVector, ",");//经多次叠加后，sourceItem可能变成(1,5,1,5,8),最后一项为像素数量，其余前面部分为多年种植的作物编号
			string sourceCropId = "";
			for (int j = 0; j<sourceItemVector.size() - 1; j++)
				sourceCropId += sourceItemVector[j] + ",";
			int sourceCropNumber = atoi(sourceItemVector[sourceItemVector.size() - 1].c_str());


			if (newCropNumber == (sourceCropNumber - dealedPixelNumber))
			{
				result += "(" + sourceCropId + newCropId + "," + to_string(newCropNumber) + ");";
				newCropNumber = 0;//当前项的crop，已处理完
				dealedPixelNumber = 0;
				currentItemIndex++;
			}
			else if (newCropNumber < (sourceCropNumber - dealedPixelNumber)) {
				result += "(" + sourceCropId + newCropId + "," + to_string(newCropNumber) + ");";
				dealedPixelNumber += newCropNumber;
				newCropNumber = 0;//当前项的crop，已处理完
			}
			else {
				result += "(" + sourceCropId + newCropId + "," + to_string((sourceCropNumber - dealedPixelNumber)) + ");";
				newCropNumber = newCropNumber - (sourceCropNumber - dealedPixelNumber);
				dealedPixelNumber = 0;
				currentItemIndex++;
			}
		}
	}
	return result;
}


//直接读取N幅tiff图像对应位置上的像素，转化为竖形的序列数据库
void RLCProducer::convertOriginal2VData()
{

	int sYear = atoi(startYear.c_str());
	int eYear = atoi(endYear.c_str());
	cout << "===================================== Step 1: Begin converting TIFF data to Sequence Database =====================================" << endl;
	string outputFileName = fileDir + "/original_prefix.txt";

	const int yearCount = 7;
	TIFF *tifFile[yearCount];
	uint32* image[yearCount];
	uint32 *rowPointerToSrc[yearCount];

	for (int year = 0; year < yearCount; year++)
	{
		string inputFilename = fileDir + "/" + to_string(sYear+year) + ".tif";
		tifFile[year] = TIFFOpen(inputFilename.c_str(), "r");
		TIFFGetField(tifFile[year], TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tifFile[year], TIFFTAG_IMAGELENGTH, &height);

		long int pixelCount = width * height;
		cout << "width=" << width << endl;
		cout << "height=" << height << endl;
		cout << "pixelCount=" << pixelCount << endl;

		image[year] = (uint32*)malloc(pixelCount * sizeof(uint32));
		 
		TIFFReadRGBAImage(tifFile[year], width, height, image[year], 1);

		rowPointerToSrc[year] = image[year] + (height - 1)*width;
	}

	Atts atts;
	atts.loadDataFromFile("D:/attcomma.txt");//读取属性文件
											 //rowPointerToSrc -= width * 4000;
	ofstream outFile;
	outFile.open(outputFileName);

	
			long  f_id = 1;
			 
			for (int y = height - 1; y >= 0; --y)
			{
				vector<vector<int>> RLCline;//存储每一行像素的游程编码
				uint32 *colPointerToSrc[yearCount];
				for(int i=0;i<yearCount;i++)
					colPointerToSrc[i]=rowPointerToSrc[i];
				
				BYTE r[yearCount];
				BYTE g[yearCount];
				BYTE b[yearCount];
				int cropId[yearCount];

				for (int x = 0; x <= width - 1; ++x)
				{
					__try
					{
						for (int i = 0; i < yearCount; i++)
						{
							r[i] = (BYTE)TIFFGetR(colPointerToSrc[i][0]);//获取R通道
							g[i] = (BYTE)TIFFGetG(colPointerToSrc[i][0]);//获取G通道
							b[i] = (BYTE)TIFFGetB(colPointerToSrc[i][0]);//获取B通道
							cropId[i] = atts.getCropIdByRGB((int)r[i], (int)g[i], (int)b[i]);
						}	
						for (int i = 0; i < yearCount; i++)
						{
							
							if (cropId[i] != 81 && cropId[i] != 82 && cropId[i] != 83 && cropId[i] != 87 && cropId[i] != 88 && cropId[i] != -1)
								outFile << f_id << " " << (2010 + i) << " " << cropId[i]  << "\n";
						}
					}
					__except (EXCEPTION_EXECUTE_HANDLER) { cout << "access vialation exceptionn" << endl; }
				 
					for (int i = 0; i < yearCount; i++)
					  colPointerToSrc[i]++;
					f_id++;
				}

				for (int i = 0; i < yearCount; i++)
				  rowPointerToSrc[i] -= width;
			 

			}
			for (int i = 0; i < yearCount; i++)
			  free(image[i]);
			for (int i = 0; i < yearCount; i++)
			  TIFFClose(tifFile[i]);
			outFile.close();
		
	
	cout << "===================================== Finish step 1 =====================================" << endl;
}

//直接读取N幅tiff图像对应位置上的像素，转化为竖形的序列数据库
void RLCProducer::convertOriginalLine2VData()
{

	int sYear = atoi(startYear.c_str());
	int eYear = atoi(endYear.c_str());
	cout << "===================================== Step 1: Begin converting TIFF data to Sequence Database =====================================" << endl;
	string outputFileName = fileDir + "/original_prefix.txt";

	const int yearCount = 7;
	TIFF *tifFile;
	uint32* image;
	uint32 *rowPointerToSrc;

	for (int year = 0; year < 1; year++)
	{
		string inputFilename = fileDir + "/" + to_string(sYear + year) + ".tif";
		tifFile = TIFFOpen(inputFilename.c_str(), "r");
		TIFFGetField(tifFile, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tifFile, TIFFTAG_IMAGELENGTH, &height);

		long int pixelCount = width * height;
		cout << "width=" << width << endl;
		cout << "height=" << height << endl;
		cout << "pixelCount=" << pixelCount << endl;

	
		TIFFClose(tifFile);
	}

	Atts atts;
	atts.loadDataFromFile("D:/attcomma.txt");//读取属性文件
											 //rowPointerToSrc -= width * 4000;
	ofstream outFile;
	outFile.open(outputFileName);


	long  f_id = 1;

	for (int y = height - 1; y >= 0; --y)
	{
		vector<vector<int>> cropIds;
		for (int year = 0; year < yearCount; year++)
		{
			string inputFilename = fileDir + "/" + to_string(sYear + year) + ".tif";
			tifFile = TIFFOpen(inputFilename.c_str(), "r");
			
			long int pixelCount = width * height;
			

			image = (uint32*)malloc(pixelCount * sizeof(uint32));

			TIFFReadRGBAImage(tifFile, width, height, image, 1);

			rowPointerToSrc = image + y*width;

			uint32 *colPointerToSrc = rowPointerToSrc;

			BYTE r;
			BYTE g;
			BYTE b;
			int cropId;

			for (int x = 0; x <= width - 1; ++x)
			{


				r = (BYTE)TIFFGetR(colPointerToSrc[0]);//获取R通道
				g = (BYTE)TIFFGetG(colPointerToSrc[0]);//获取G通道
				b = (BYTE)TIFFGetB(colPointerToSrc[0]);//获取B通道
				cropId = atts.getCropIdByRGB((int)r, (int)g, (int)b);

				if (year == 0)
				{
					vector<int> cropIdArr;
					cropIdArr.push_back(cropId);
					cropIds.push_back(cropIdArr);
				}
				else
				{
					vector<int> cropIdArr = (vector<int>)cropIds[x];
					cropIdArr.push_back(cropId);
					cropIds[x] = cropIdArr;
				}



				colPointerToSrc++;

			}
			
			

			free(image);
			TIFFClose(tifFile);
		}
		for (int i = 0; i < cropIds.size(); i++)
		{
			vector<int> cropIdArr = (vector<int>)cropIds[i];
			for (int j = 0; j < yearCount; j++)
			{
				int cropId = (int)cropIdArr[j];
				if (cropId != 81 && cropId != 82 && cropId != 83 && cropId != 87 && cropId != 88 && cropId != -1)
					outFile << f_id << " " << (sYear + j) << " " << cropId << "\n";
			}
			f_id++;
		}
		
		cropIds.clear();
	 


	}
	
 
	outFile.close();


	cout << "===================================== Finish step 1 =====================================" << endl;
}

void RLCProducer::produceTIFF(string fileDir, string year, string pattern)
{
	 

	TIFF *tifFile;
	TIFF *outTiff;
	string inputFilename = fileDir + "/" + year + ".tif";
	if ((tifFile = TIFFOpen(inputFilename.c_str(), "r")) != NULL) {                      //TIFFOpen的参数使用你的tiff文件路径和名称

		TIFFGetField(tifFile, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tifFile, TIFFTAG_IMAGELENGTH, &height);
	}

	string outfile = fileDir + "/" + year + "_pattern.tif";
	outTiff = TIFFOpen(outfile.c_str(), "w");
	
	int spp = 1; /* Samples per pixel */
	int bpp = 32; /* Bits per sample */
				  // We need to set some values for basic tags before we can add any data
	TIFFSetField(outTiff, TIFFTAG_IMAGEWIDTH, width);//set the width
	TIFFSetField(outTiff, TIFFTAG_IMAGELENGTH, height);//set the height
	TIFFSetField(outTiff, TIFFTAG_BITSPERSAMPLE, bpp);//set the size of the channels
	TIFFSetField(outTiff, TIFFTAG_SAMPLESPERPIXEL, spp);//set number of channels per pixel
	TIFFSetField(outTiff, TIFFTAG_ROWSPERSTRIP, width*spp);
	TIFFSetField(outTiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(outTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	TIFFSetField(outTiff, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
	TIFFSetField(outTiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(outTiff, TIFFTAG_XRESOLUTION, 96.0);
	TIFFSetField(outTiff, TIFFTAG_YRESOLUTION, 96.0);
	TIFFSetField(outTiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
 
	cout << width << endl;
	cout << height << endl;

	long int pixelCount = width*height;
	uint32* image = (uint32*)malloc(pixelCount * sizeof(uint32));
	TIFFReadRGBAImage(tifFile, width, height, image, 1);
	//BYTE* R = new BYTE[pixelCount];    //为存放数据分配内存空间
	//BYTE* G = new BYTE[pixelCount];    //为存放数据分配内存空间
	//BYTE* B = new BYTE[pixelCount];    //为存放数据分配内存空间
	uint32 *rowPointerToSrc = image + (height - 1)*width;
	//BYTE *rowPointerToR = R;
	//BYTE *rowPointerToG = G;
	//BYTE *rowPointerToB = B;

	Atts atts;
	atts.loadDataFromFile("D:/attcomma.txt");//读取属性文件
											 //rowPointerToSrc -= width * 4000;
	


	long  f_id = 1;
	//左上(270,65)，右下(321,117)
	//width = 1310;
	tsize_t linebytes = width*spp;
	 /*
	for (int y = height - 1; y >= 0; --y)
	{
		vector<vector<int>> RLCline;//存储每一行像素的游程编码
		uint32 *colPointerToSrc = rowPointerToSrc;
		

		unsigned char *prow = NULL;        // buffer used to store the row of pixel information for writing to file
										  //    Allocating memory to store the pixels of current row
		if (TIFFScanlineSize(outTiff)<linebytes)
			prow = (unsigned char *)_TIFFmalloc(linebytes);
		else
			prow = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(outTiff));

		//BYTE *colPointerToR = rowPointerToR;
		//BYTE *colPointerToG = rowPointerToG;
		//BYTE *colPointerToB = rowPointerToB;
		BYTE r;
		BYTE g;
		BYTE b;
		for (int x = 0; x <= width - 1; ++x)
		{
			 
				r = (BYTE)TIFFGetR(colPointerToSrc[0]);//获取R通道
				g = (BYTE)TIFFGetG(colPointerToSrc[0]);//获取G通道
				b = (BYTE)TIFFGetB(colPointerToSrc[0]);//获取B通道
													   //cout << "x=" << x << "," << "y=" << y << ":" << atts.getCropIdByRGB((int)colPointerToR[0], (int)colPointerToG[0], (int)colPointerToB[0]) << endl;
													   //cout << "(" << (int)colPointerToR[0] << "," << (int)colPointerToG[0] << "," << (int)colPointerToB[0] << "," << atts.getCropIdByRGB((int)colPointerToR[0], (int)colPointerToG[0], (int)colPointerToB[0])<< "),";
				int cropId = atts.getCropIdByRGB((int)r, (int)g, (int)b);
				if (cropId = 5)
				{
					prow[x*spp] = 1;
					//prow[x*spp+1] = 1;
				//	prow[x*spp+2] = 1;
				}
					
				else {
					prow[x*spp] = 255;
				//	prow[x*spp + 1] = 255;
				//	prow[x*spp + 2] = 255;
				}
					
		 
			colPointerToSrc++;
			f_id++;
		}
		 
		int back=TIFFWriteScanline(outTiff,prow, y, 0);
		cout << back<<endl;
		if(prow)
		   _TIFFfree(prow);
		rowPointerToSrc -= width;
		//rowPointerToR += width;
		//rowPointerToG += width;
		//rowPointerToB += width;

	}
	  */
	
	unsigned char *buf = NULL;        // buffer used to store the row of pixel information for writing to file
									  //    Allocating memory to store the pixels of current row
	if (TIFFScanlineSize(outTiff)<linebytes)
		buf = (unsigned char *)_TIFFmalloc(linebytes);
	else
		buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(outTiff));
	buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(outTiff));
	// We set the strip size of the file to be size of one row of pixels
	TIFFSetField(outTiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(outTiff, width*spp));

	//Now writing image to the file one strip at a time
	for (uint32 row = 0; row < height; row++)
	{
		memcpy(buf, &image[(height - row - 1)*width], width);    // check the index here, and figure out why not using h*linebytes
		if (TIFFWriteScanline(outTiff, buf, row, 0) < 0)
			break;
	}
		
		TIFFClose(outTiff);// Close the file
		TIFFClose(tifFile);// Close the file

	 
}
