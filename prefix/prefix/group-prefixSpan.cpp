#include "stdafx.h"
#include "group-prefixSpan.h"
#include <map>

groupPrefixSpan::groupPrefixSpan(string fileDir, float supportRat)
{
	this->inputFileName = fileDir + "/data_prefixSpan_sort.txt";
	this->outputFileName = fileDir + "/sequences_sort.txt";
	recordCount = 0;
	sequenceCount = 0;
	this->supportRat = supportRat;
	isGroup = false;
	loopCount = 0;
	skipCount = 0;
}


groupPrefixSpan::~groupPrefixSpan()
{
}
void groupPrefixSpan::SplitString(const string& s, vector<string>& v, const string& c)
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

void groupPrefixSpan::findFrequentPattern(vector<FrequentPattern>& result)
{
	ifstream in;
	in.open(inputFileName, ios::in);
	if (!in)
		return;

	outF.open(outputFileName);

	const int LINE_LENGTH = 100;
	char str[LINE_LENGTH];
	string currentSID = "";

	vector<vector<string>> db;
 
	while (in.getline(str, LINE_LENGTH))
	{
		recordCount++;
		vector<string> tempStr;
		string sstr(str);
		string seperator = "";
		if (sstr.find(" ") != string::npos)
			seperator = " ";
		else if (sstr.find("\t") != string::npos)
			seperator = "\t";
		SplitString(str, tempStr, seperator);
		db.push_back(tempStr);
		string sid = tempStr[0];

		if (tempStr.size() == 4)
			isGroup = true;
		if (currentSID != sid)
		{
			currentSID = sid;
			if (isGroup)
				sequenceCount += atoi(tempStr[3].c_str());
			else
				sequenceCount++;
		}
	}
	cout << "total record count is:" << recordCount << ",";
	cout << "total sequence count is:" << sequenceCount << ",";
	cout << "support rate is:" << supportRat << endl;
	doPrefixSpan("", 0, db,sequenceCount);

	in.close();
	outF.close();
}

void groupPrefixSpan::doPrefixSpan(string prefix, int length, vector<vector<string>> &db, double sequenceCount)
{

	if (db.size() == 0)
		return;
	map<string, int> frequent1;
	string currentSID = "";
	string itemsOfSameSID = "";

	double dealedSequenceCount = 0;
	double currentMiniSequenceCount = this->sequenceCount; 
	map<string, int>::iterator it;
	
	for (int i = 0; i < db.size(); i++)
	{

		 
		vector<string> tempStr = (vector<string>)db[i];
		string sid = tempStr[0];
		string eid = tempStr[1];
		string item = tempStr[2];

		//check the prunning criteria
		if (sid != currentSID)
		{
			double remainSequenceCount = sequenceCount - dealedSequenceCount;
			bool hasPotentialPrefix = false;
			
			it = frequent1.begin();

			
			while (it != frequent1.end())
			{
				if ((it->second < this->sequenceCount*supportRat && it->second + remainSequenceCount > this->sequenceCount*supportRat) ||
					(it->second >= this->sequenceCount*supportRat &&  remainSequenceCount > this->sequenceCount*supportRat)
					)
				{
					hasPotentialPrefix = true;
					break;
				}
				it++;
				loopCount++;
			}
		 
			if (hasPotentialPrefix == false && frequent1.size() > 0)
			{
				cout << "remain " << remainSequenceCount << ", skip " << db.size() - i << endl;
				skipCount += db.size() - i;
				break;
			}
		}

		
		 
		if (sid != currentSID)
		{
			 
			if (isGroup && currentSID!="")
				dealedSequenceCount += atoi(tempStr[3].c_str());

			currentSID = sid;
			itemsOfSameSID = "," + item + ",";
			
			
		}
		else   
		{
			if (itemsOfSameSID.find("," + item + ",") != string::npos)  
			{
				continue;
			}
			else
				itemsOfSameSID += "," + item + ",";
		}

		if (frequent1.count(item) > 0)
		{
			if (isGroup)
				frequent1.find(item)->second += atoi(tempStr[3].c_str());
			else {
				frequent1.find(item)->second++;

			}

		}
		else
		{
			if (isGroup)
				frequent1.insert(make_pair(item, atoi(tempStr[3].c_str()))); 
			else
				frequent1.insert(make_pair(item, 1));
		}
		
	}

	 
	it = frequent1.begin();
	while (it != frequent1.end())
	{

		if ((it->second / this->sequenceCount) < supportRat)
			frequent1.erase(it++);
		else
		{
			if (it->second < currentMiniSequenceCount)
				currentMiniSequenceCount = it->second;
			it++;
		}
			
	}
	 
	vector<vector<string>>::iterator dbIT = db.begin();
	while (dbIT != db.end())
	{
		vector<string> tempStr = (vector<string>)*dbIT;
		if (frequent1.count(tempStr[2]) <= 0)
			dbIT = db.erase(dbIT);
		else
			dbIT++;
	}
	 
	for (it = frequent1.begin(); it != frequent1.end(); it++)
	{
		cout << prefix << "->" << it->first << ":" << it->second << ";" << endl;
		outF << prefix << "->" << it->first << ":" << it->second << "(" << (it->second / sequenceCount) << ")" << ";" << "\n";
		vector<vector<string>> projectDB;
		double totalSequenceCount=0;
		produceProjectDB(it->first, db, projectDB, totalSequenceCount);

		//if the overall number of sequences of project database is less than support threshold, then project database would not contains frequent items
		if (totalSequenceCount < this->sequenceCount*supportRat)
			continue;

		doPrefixSpan(prefix + "->" + it->first, length++, projectDB, totalSequenceCount);
	}
}
 
void groupPrefixSpan::produceProjectDB(string prefix, const vector<vector<string>>& db, vector<vector<string>>& projectDB, double& totalSequenceCount)
{
	string currentSID = "";
	string itemsOfSameSID = "";
	vector<vector<string>>::const_iterator it = db.begin();
	while (it != db.end())
	{
		vector<string> tempStr = (vector<string>)*it;
		string sid = tempStr[0];
		string eid = tempStr[1];
		string item = tempStr[2];
		if (item != prefix)
		{
			it++;
			continue;
		}
		else {
			currentSID = sid;
			it++; 
			
			if(it != db.end() && ((vector<string>)*it)[0] == sid)
			   totalSequenceCount += atoi(tempStr[3].c_str());
			

			while (it != db.end() && ((vector<string>)*it)[0] == sid) 
			{
				projectDB.push_back(*it);
				it++;
			}
		}

	}

}

