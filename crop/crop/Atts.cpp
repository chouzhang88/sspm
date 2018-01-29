#include "stdafx.h"

#include <iostream>
#include <vector>
#include <string>
#include <Atts.h>


/*
从文件中加载CDL属性文件
*/
bool Atts::loadDataFromFile(string fileName) {
	ifstream in;
	in.open(fileName, ios::in);
	if (!in)
		return false;
	char buf[1024];
	in.getline(buf, 1024);//读掉第一行
	

	while (in.getline(buf, 1024))
	{
		string str(buf);
		vector<string> tempVector;
		SplitString(str,tempVector,",");
		if (tempVector.size()==6)
		{
			Att att(atoi(tempVector[0].c_str()), tempVector[1], atof(tempVector[2].c_str()), atof(tempVector[3].c_str()), atof(tempVector[4].c_str()));
			data.push_back(att);
		}
	}
	return true;
}
/*
根据像素的RGB值，查找对应的地物目标的编号；
*/
int Atts::getCropIdByRGB(int r, int g, int b) {
	int result = -1;
	if (data.size() > 1)
	{
		vector<Att>::iterator it = data.begin();
		for (; it < data.end(); it++)
		{
			Att att = (Att)*it;
			if (att.equalsByRGB(r, g, b))
			{
				//cout << "matched:" << r << "," << g << "," << b << "," << att.getId()<<endl;
				result = att.getId();
				return result;
			}
		}
	}
	return result;
}
void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}