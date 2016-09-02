// Virtual_func.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
class ImageFetch{
public:
	virtual void Run()=0;
	int a,b,c;

};
class ImageFetchLocal:public ImageFetch{
public:
	void Run(){
		std::cout<<"This is ImageFetchLocal \n";
	}
};
class ImageFetchGlobal:public ImageFetch{
public:
	void Run(){
	std::cout<<"This is ImageFetchGlobal \n";
	}
};
class Parents{
public :
	int a;
	int show()
	{
		return 1;
	}
	virtual void run(){
		std::cout<<" Virtual function \n TP"<<std::endl;
	}

};
class Sons :public Parents{
public:
	void run(){
		std::cout<<" Virtual function \n op"<<std::endl;
	}

};

int _tmain(int argc, _TCHAR* argv[])
{
	ImageFetch *hehe;
	hehe=new ImageFetchLocal();
	hehe->Run();
	delete hehe;
	hehe=new ImageFetchGlobal();
	hehe->Run();
	delete hehe;

	Parents *P;
	P=new Sons();
	P->run();
	Parents P1;
	P1.run();
	return 0;
}


//#include "stdafx.h"
//#include <iostream>  
//using namespace std;  
//   
//class Base1 {  
//public:  
//            virtual void f() { cout << "Base1::f" << endl; }  
//            virtual void g() { cout << "Base1::g" << endl; }  
//            virtual void h() { cout << "Base1::h" << endl; }  
//   
//};  
//class Base2 {  
//public:  
//            virtual void f() { cout << "Base2::f" << endl; }  
//            virtual void g() { cout << "Base2::g" << endl; }  
//            virtual void h() { cout << "Base2::h" << endl; }  
//};  
//class Base3 {  
//public:  
//            virtual void f() { cout << "Base3::f" << endl; }  
//            virtual void g() { cout << "Base3::g" << endl; }  
//            virtual void h() { cout << "Base3::h" << endl; }  
//};  
//class Derive : public Base1, public Base2, public Base3 {  
//public:  
//            virtual void f() { cout << "Derive::f" << endl; }  
//            virtual void g1() { cout << "Derive::g1" << endl; }  
//};  
//int bigger(int a,int b){
//	if(a>b)
//		return a;
//	return b;
//}
//void smaller(int a,int b){
//	if(a>b)
//		std::cout<< b;
//	std::cout<< a;
//}
//void what(void){
//	std::cout<< "Hello \n";
//}   
//   
// typedef void(*Fun)(void);  //函数指针
//   
//int main()  
//{  
////函数指针
//	int (*p)(int,int);
//	p=bigger;
//	std::cout<<(*p)(3,100)<<std::endl;
//	void (*smal)(int,int);
//	smal=smaller;
//	(smaller)(-10,10);
//	(*smal)(1,100);
//	void (*wh)(void);
//	wh=&what;
//	wh();
//
//			void(*pFun)(void);
//			pFun=NULL;
////            Fun pFun = NULL;  
//   
//            Derive d;  
//            int** pVtab = (int**)&d;  
//   
//            //Base1's vtable  
//            //pFun = (Fun)*((int*)*(int*)((int*)&d+0)+0);  
//            pFun = (Fun)pVtab[0][0];  
//            pFun();  
//   
//            //pFun = (Fun)*((int*)*(int*)((int*)&d+0)+1);  
//            pFun = (Fun)pVtab[0][1];  
//            pFun();  
//   
//            //pFun = (Fun)*((int*)*(int*)((int*)&d+0)+2);  
//            pFun = (Fun)pVtab[0][2];  
//            pFun();  
//   
//            //Derive's vtable  
//            //pFun = (Fun)*((int*)*(int*)((int*)&d+0)+3);  
//            pFun = (Fun)pVtab[0][3];  
//            pFun();  
//   
//            //The tail of the vtable  
//            pFun = (Fun)pVtab[0][4];  
//            cout<<pFun<<endl;  
//   
//   
//            //Base2's vtable  
//            //pFun = (Fun)*((int*)*(int*)((int*)&d+1)+0);  
//            pFun = (Fun)pVtab[1][0];  
//            pFun();  
//   
//            //pFun = (Fun)*((int*)*(int*)((int*)&d+1)+1);  
//            pFun = (Fun)pVtab[1][1];  
//            pFun();  
//   
//            pFun = (Fun)pVtab[1][2];  
//            pFun();  
//   
//            //The tail of the vtable  
//            pFun = (Fun)pVtab[1][3];  
//            cout<<pFun<<endl;  
//   
//   
//   
//            //Base3's vtable  
//            //pFun = (Fun)*((int*)*(int*)((int*)&d+1)+0);  
//            pFun = (Fun)pVtab[2][0];  
//            pFun();  
//   
//            //pFun = (Fun)*((int*)*(int*)((int*)&d+1)+1);  
//            pFun = (Fun)pVtab[2][1];  
//            pFun();  
//   
//            pFun = (Fun)pVtab[2][2];  
//            pFun();  
//   
//            //The tail of the vtable  
//            pFun = (Fun)pVtab[2][3];  
//            cout<<pFun<<endl;  
//            return 0;  
//}  