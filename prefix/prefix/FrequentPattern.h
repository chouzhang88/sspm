#pragma once
//Ƶ������
#include <string>
using namespace std;

class FrequentPattern
{
public:
	string pattern;
	float supportRat;//֧����
	int supportCount;//֧�ֶ�:���������ݿ��г��ּ���
public:
	FrequentPattern();
	~FrequentPattern();
};

