#include <vector>
#include <fstream>
#include <string>
using namespace std;
#ifndef ATT
#define ATT

extern float minGaps;

class Att {
private:
	int id;
	string background;
	float r;
	float g;
	float b;
	float a;
public:
	Att() {}
	Att(int id, string background, float r, float g, float b)
	{
		this->id = id;
		this->background = background;
		this->r = r;
		this->g = g;
		this->b = b;
	}
	bool equalsByRGB(float r, float g, float b);
	int getId() { return id; }
	friend ifstream& operator >> (ifstream &in, Att &att);

};

#endif


