#ifndef GLOBALPARAM_H
#define GLOBALPARAM_H

const int		m_nHeightGlobal = 2796;    ///< 全图的高
const int		m_nWidthGlobal  = 2518;    ///< 全图的宽
const int		m_nHeightBlock  = 2796;		///< 分块的高    //1100  2160
const int		m_nWidthBlock   = 2518;		///< 分块的宽    //2000  2560


#define IN
#define OUT

#define Pixel  Pixel16   


///该函数模板用于释放函数中自定义的各种数据类型的指针变量
template<class T>
inline void safeDelete(T* p)
{
	if(p != nullptr)
	{
		delete []p;
		p = nullptr;
	}
}

//#define RETURN_IF_NULL(p)  {if(p == NULL) return false;}



#endif