#ifndef ATTS
#define ATTS
#include <iostream>
#include <vector>
#include <string>
#include <att.h>

using namespace std;
class Atts {
private:
	vector<Att> data;
public:
	bool loadDataFromFile(string fileName);
	int getCropIdByRGB(int r, int g, int b);
};
void SplitString(const string& s, vector<string>& v, const string& c);

#endif