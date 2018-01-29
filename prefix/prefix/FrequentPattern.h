#pragma once
//频繁序列
#include <string>
using namespace std;

class FrequentPattern
{
public:
	string pattern;
	float supportRat;//支持率
	int supportCount;//支持度:在序列数据库中出现几次
public:
	FrequentPattern();
	~FrequentPattern();
};

