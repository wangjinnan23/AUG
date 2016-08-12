/*********************************************************************//**
* \file      Pixel.cpp
* \brief     图像像素自定义数据类型的实现
* \author    周茂民
* \version   1.0b
* \date      2016年4月
* \copyright 该文件版权归清华大学所有
**************************************************************************/

#include "stdafx.h"
#include "Pixel.h"


/*********************************************************************//**
* \brief   重载" + " 运算符，Pixel16 + Pixel16
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
Pixel32 operator+(const Pixel16 &m, const Pixel16 &n)
{
	Pixel32 v;
	v.datatmp = (float)m.data + (float)n.data;
	return(v);
}


/*********************************************************************//**
* \brief   重载" + " 运算符，Pixel32 + Pixel16
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
Pixel32 operator+(const Pixel32 &m, const Pixel16 &n)
{
	Pixel32 v;
	v.datatmp = m.datatmp + (float)n.data;
	return(v);
}

/*********************************************************************//**
* \brief   重载" + " 运算符，Pixel16 + Pixel32
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
Pixel32 operator+(const Pixel16 &m, const Pixel32 &n)
{
	Pixel32 v;
	v.datatmp = (float)m.data + n.datatmp;
	return(v);

}


/*********************************************************************//**
* \brief   重载" * " 运算符，Pixel16 * float
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
Pixel32 operator*(const Pixel16 &m ,const float &n)
{
	Pixel32 v;
	v.datatmp = (float)m.data * n;
	return(v);
}


/*********************************************************************//**
* \brief   重载" * " 运算符，float * Pixel16
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
Pixel32 operator*(const float &m ,const Pixel16 &n)
{
	Pixel32 v;
	v.datatmp = m * (float)n.data;
	return(v);
}


/*********************************************************************//**
* \brief   重载" / " 运算符，Pixel32 / ushort
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
Pixel32 operator/(const Pixel32 &m, const ushort &n)
{
	Pixel32 v;
	v.datatmp = m.datatmp / n;
	return(v);
}

/*********************************************************************//**
* \brief   类型转换，从Pixel32类型转换到Pixel16类型
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
void Pixel16::Pixel32to16(const Pixel32 &m)
{
	data = (ushort)(m.datatmp);
}
