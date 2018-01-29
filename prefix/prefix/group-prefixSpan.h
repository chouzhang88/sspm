#pragma once
//序列数据库类
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "FrequentPattern.h"

using namespace std;

class groupPrefixSpan
{
private:
	string inputFileName; 
	string outputFileName; 
	ofstream outF;
	float recordCount; 
	float sequenceCount; 
	float supportRat; 
	bool isGroup; 
public:	long int loopCount; 
		long int skipCount;
public:
	groupPrefixSpan(string fileDir, float supportRat);
	~groupPrefixSpan();
	void SplitString(const string& s, vector<string>& v, const string& c);
	void findFrequentPattern(vector<FrequentPattern> & result);
private:
	void doPrefixSpan(string prefix, int length, vector<vector<string>> &db,double sequenceCount);
	void produceProjectDB(string prefix, const vector<vector<string>>& db, vector<vector<string>>& projectDB, double& totalSequenceCount);
};

