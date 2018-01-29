#pragma once
//序列数据库类
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "FrequentPattern.h"

using namespace std;

class prefixSpan
{
private:
	string inputFileName;//the location of file containing sequence databse
	string outputFileName;//the location of output file for storing sequential patterns序列数据库文件的地址
	ofstream outF;
	long int recordCount;//the total record counts of sequence database
	float sequenceCount;//total total number of sequences in sequence database
	float supportRat; 
	bool isGroup;//the flag indicates if the sequence database is produced by pixels-group clustering
public:	long int loopCount; 
		long int skipCount;
public:
	prefixSpan(string fileDir,float supportRat);
	~prefixSpan();
	void SplitString(const string& s, vector<string>& v, const string& c);
	void findFrequentPattern(vector<FrequentPattern> & result);
private:
	void doPrefixSpan(string prefix, int length, vector<vector<string>> &db);
	void produceProjectDB(string prefix, const vector<vector<string>>& db, vector<vector<string>>& projectDB);
};

