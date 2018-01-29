#include "stdafx.h"
#include <att.h>

float minGaps = 0.001f;

ifstream& operator >> (ifstream &in, Att &att) {
	in >> att.id >> att.background >> att.r >> att.g >> att.b>> att.a;
	return in;
}

bool Att::equalsByRGB(float r, float g, float b)
{
	if (abs(this->r - r / 255) < minGaps && abs(this->g - g / 255) < minGaps && abs(this->b - b / 255) < minGaps)
		return true;
	return false;
}
