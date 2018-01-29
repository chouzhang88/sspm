// prefix.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "prefixSpan.h"
#include "group-prefixSpan.h"
#include <iostream>
#include <time.h>
using namespace std;

int main()
{
	
	
	clock_t startTime, endTime;
	startTime = clock();

	groupPrefixSpan  ps("C:/Users/93866/Documents/Visual Studio 2015/Projects/crop/crop/butler/",0.01f);
	vector<FrequentPattern> result;
	ps.findFrequentPattern(result);
	cout << "extra examination："<<ps.loopCount << endl;
	cout << "rows to skip：" << ps.skipCount << endl;
 
	endTime = clock();
	cout << "Totle Time : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	system("pause");
    return 0;
}

