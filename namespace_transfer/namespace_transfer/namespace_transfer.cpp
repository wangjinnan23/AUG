// namespace_transfer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
class Global_variation{
public :
	int variation;
};
namespace savitch1
{
	int i;
	void greeting();
}
namespace savitch2
{
	int j;
	void greeting();
}
void big_greeting();
int _tmain(int argc, _TCHAR* argv[])
{
	Global_variation G;
	G.variation=5;
	savitch1::i=1;
	savitch1::greeting();
	savitch2::greeting();
	savitch1::greeting();
	savitch2::greeting();
	savitch1::i=1;
	savitch2::j=2;

	return 0;
}

namespace savitch1
{
	void greeting()
	{
		std::cout<<"savitch1 greering\n"<<savitch1::i;
		savitch1::i++;
	}
}
namespace savitch2
{
	int i=0;
	void greeting()
	{
		std::cout<<"savitch2 greering\n"<<savitch1::i;
		savitch1::i++;
	}
}