#include "stdafx.h"
#include "prefixSpan.h"
#include <map>

prefixSpan::prefixSpan(string fileDir, float supportRat)
{
	this->inputFileName = fileDir+"/data_prefixSpan.txt";
	this->outputFileName = fileDir + "/sequences.txt";
	recordCount = 0;
	sequenceCount = 0;
	this->supportRat = supportRat;
	isGroup = false;
}


prefixSpan::~prefixSpan()
{
}
void prefixSpan::SplitString(const string& s, vector<string>& v, const string& c)
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

void prefixSpan::findFrequentPattern(vector<FrequentPattern>& result)
{
	ifstream in;
	in.open(inputFileName, ios::in);
	if (!in)
		return;
	
	outF.open(outputFileName);

	const int LINE_LENGTH = 100;
	char str[LINE_LENGTH];
	string currentSID="";
	
	vector<vector<string>> db;
	//load data into memory from disk file
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
	cout << "total record count is:" << recordCount << endl;
	cout << "total sequence count is:" << sequenceCount << endl;
	doPrefixSpan("", 0, db);
	
	in.close();
	outF.close();
}

void prefixSpan::doPrefixSpan(string prefix, int length, vector<vector<string>> &db)
{
	
	if (db.size() == 0)
		return;
	map<string, int> frequent1;
	string currentSID = "";
	string itemsOfSameSID = "";


	for (int i = 0; i < db.size(); i++)
	{
		//read a line at each iteration
		vector<string> tempStr = (vector<string>)db[i];
		string sid = tempStr[0];
		string eid = tempStr[1];
		string item = tempStr[2];
		 
		if (sid != currentSID)
		{
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
				itemsOfSameSID += ","+item+",";
		}

		if (frequent1.count(item) > 0)
		{
			if(isGroup)
				frequent1.find(item)->second+= atoi(tempStr[3].c_str());
			else {
                frequent1.find(item)->second++;
				
			}
			   
		}
		else
		{
			if (isGroup)
				frequent1.insert(make_pair(item, atoi(tempStr[3].c_str())));//被这行的失误搞死
			else
			    frequent1.insert(make_pair(item, 1));
		}
	}

	//delete non-frequent items from candidate frequent item sets
	map<string, int>::iterator it = frequent1.begin();
	while (it != frequent1.end())
	{
		
		if ((it->second / sequenceCount) < supportRat)
			frequent1.erase(it++);
		else
			it++;
	}
	//delete non-frequent items from original database(in memory)
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
		cout <<prefix << "->" << it->first << ":" << it->second << ";"<<endl;
		outF << prefix << "->" << it->first << ":" << it->second << "("<< (it->second / sequenceCount)<<")"<<";" << "\n";
		vector<vector<string>> projectDB;
		produceProjectDB(it->first,db, projectDB);
		
		
		doPrefixSpan(prefix + "->" +it->first, length++, projectDB);
	}
}
//construct projection database
void prefixSpan::produceProjectDB(string prefix, const vector<vector<string>>& db, vector<vector<string>>& projectDB)
{
	string currentSID = "";
	string itemsOfSameSID = "";
	vector<vector<string>>::const_iterator it = db.begin();
	while (it!=db.end())
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
			while (it != db.end() && ((vector<string>)*it)[0] == sid)//add items of suffix to projection database
			{
				projectDB.push_back(*it);
				it++;
			}
		}

	}
	
}

