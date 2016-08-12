/*********************************************************************//**
* \file      Pixel.cpp
* \brief     ͼ�������Զ����������͵�ʵ��
* \author    ��ï��
* \version   1.0b
* \date      2016��4��
* \copyright ���ļ���Ȩ���廪��ѧ����
**************************************************************************/

#include "stdafx.h"
#include "Pixel.h"


/*********************************************************************//**
* \brief   ����" + " �������Pixel16 + Pixel16
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
Pixel32 operator+(const Pixel16 &m, const Pixel16 &n)
{
	Pixel32 v;
	v.datatmp = (float)m.data + (float)n.data;
	return(v);
}


/*********************************************************************//**
* \brief   ����" + " �������Pixel32 + Pixel16
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
Pixel32 operator+(const Pixel32 &m, const Pixel16 &n)
{
	Pixel32 v;
	v.datatmp = m.datatmp + (float)n.data;
	return(v);
}

/*********************************************************************//**
* \brief   ����" + " �������Pixel16 + Pixel32
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
Pixel32 operator+(const Pixel16 &m, const Pixel32 &n)
{
	Pixel32 v;
	v.datatmp = (float)m.data + n.datatmp;
	return(v);

}


/*********************************************************************//**
* \brief   ����" * " �������Pixel16 * float
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
Pixel32 operator*(const Pixel16 &m ,const float &n)
{
	Pixel32 v;
	v.datatmp = (float)m.data * n;
	return(v);
}


/*********************************************************************//**
* \brief   ����" * " �������float * Pixel16
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
Pixel32 operator*(const float &m ,const Pixel16 &n)
{
	Pixel32 v;
	v.datatmp = m * (float)n.data;
	return(v);
}


/*********************************************************************//**
* \brief   ����" / " �������Pixel32 / ushort
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
Pixel32 operator/(const Pixel32 &m, const ushort &n)
{
	Pixel32 v;
	v.datatmp = m.datatmp / n;
	return(v);
}

/*********************************************************************//**
* \brief   ����ת������Pixel32����ת����Pixel16����
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
void Pixel16::Pixel32to16(const Pixel32 &m)
{
	data = (ushort)(m.datatmp);
}
