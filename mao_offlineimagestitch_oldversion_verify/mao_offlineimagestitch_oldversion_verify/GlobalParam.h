#ifndef GLOBALPARAM_H
#define GLOBALPARAM_H

const int		m_nHeightGlobal = 2796;    ///< ȫͼ�ĸ�
const int		m_nWidthGlobal  = 2518;    ///< ȫͼ�Ŀ�
const int		m_nHeightBlock  = 2796;		///< �ֿ�ĸ�    //1100  2160
const int		m_nWidthBlock   = 2518;		///< �ֿ�Ŀ�    //2000  2560


#define IN
#define OUT

#define Pixel  Pixel16   


///�ú���ģ�������ͷź������Զ���ĸ����������͵�ָ�����
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