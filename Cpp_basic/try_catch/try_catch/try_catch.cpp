// try_catch.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<string>
using namespace std;
class Base 
{
protected:
	int Width;
	int Height;
public:
	void Set_Width(int a)
	{
		Width=a;
	}
	void Set_Height(int b)
	{
		Height=b;
	}
};
class Derived_Class:public Base
{
	public:
		int Get_Multiply()
		{
			return Width*Height;
		}
	 
};


class Result
{
	int State;
	std::string Errormessage;
};
Result CreatFile(std::string path)
{
	Result A;
	return A;
}

 
class Copy
{
public:
	std::string *ptr;
	std::string data;
	Copy():ptr(new std::string){}
	Copy(const std::string& str){ptr=new std::string(str);data=str;}
 	Copy(const Copy& str){ptr=new std::string(str.data);data=str.data;}
	Copy& operator=(const Copy str){delete ptr;ptr=new std::string (str.data);return *this;} 
	 std::string context() 
	{				
		return data;
	}

};

int _tmain(int argc, _TCHAR* argv[])
{
	//*/
	Derived_Class A;
	A.Set_Height(1);
	A.Set_Width(101);
	std::cout<<A.Get_Multiply()<<std::endl;;
	//*/
	std::string path="Hello";
	Result B=CreatFile(path);
 
	Copy WW("AA");
	Copy MM(WW);
	Copy TT(WW);

	//std::cout<<WW.data()<<std::endl;
		 
	 std::cout<<WW.data<<std::endl;
	 std::cout<<MM.data<<std::endl;
	 std::cout<<TT.data<<std::endl;
	  std::cout<<std::endl;

	  	 std::cout<<WW.ptr<<std::endl;
	 std::cout<<MM.ptr<<std::endl;
	 std::cout<<TT.ptr<<std::endl;
	  	MM=WW;
		TT=WW;
			  std::cout<<std::endl;

	 std::cout<<WW.context()<<std::endl;
	 std::cout<<MM.context()<<std::endl;
	 std::cout<<TT.context()<<std::endl;
	 	  std::cout<<std::endl;

	 std::cout<<WW.ptr<<std::endl;
	 std::cout<<MM.ptr<<std::endl;
	 std::cout<<TT.ptr<<std::endl;

	 std::string aa("Hell");
	 std::cout<<aa<<std::endl;
	return 0;
}
