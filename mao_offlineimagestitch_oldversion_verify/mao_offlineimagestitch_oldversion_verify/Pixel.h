/*********************************************************************//**
* \file      Pixel.h
* \brief     自定义图像像素的数据类型
* \author    周茂民
* \version   1.0b
* \date      2016年4月
* \copyright 该文件版权归清华大学所有
**************************************************************************/

#ifndef  PIXEL_H
#define  PIXEL_H

/*********************************************************************//**
* \brief   定义Pixel32类，用于保存中间结果，数据为float型
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
class Pixel32
{
public:
	///构造函数
	Pixel32(float d = 0):datatmp(d) 
	{
	}
	///析构函数
	~Pixel32()
	{
	}
	float datatmp;   ///<数据类型为float
};

/*********************************************************************//**
* \brief   定义Pixel16类，作为图像像素数据的数据类型 ，重载常用运算符
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
class Pixel16
{
public:
	ushort data;    ///< 此处图像为16位，ushort

	///构造函数
	Pixel16()
	{
		data = 0;
	};

	///带参数的构造函数
	Pixel16(ushort a)
	{
		data = a;
	}

	///析构函数
	~Pixel16(void)
	{
	}

	///实现" = "重载，右端为ushort 类型，例如 Pixel16  value = 10;
	Pixel16 & operator=(const ushort &a)
	{
		data = a;
		return *this;
	}

	///实现" = "重载，右端为Pixel16l类对象
	Pixel16 & operator=(const Pixel16 &m)
	{
		data = m.data;
		return *this;
	}
	///重载" + " 运算符，Pixel16 + Pixel16
	friend Pixel32 operator+(const Pixel16 &m ,const Pixel16 &n); 

	///重载" + " 运算符，Pixel32 + Pixel16
	friend Pixel32 operator+(const Pixel32 &m, const Pixel16 &n);

	///重载" + " 运算符，Pixel16 + Pixel32
	friend Pixel32 operator+(const Pixel16 &m, const Pixel32 &n);

	///重载" * " 运算符，Pixel16 * float
	friend Pixel32 operator*(const Pixel16 &m ,const float &n); 

	///重载" * " 运算符，float * Pixel16
	friend Pixel32 operator*(const float &m ,const Pixel16 &n); 

	///重载" / " 运算符，Pixel32 / ushort
	friend Pixel32 operator/(const Pixel32 &m, const ushort &n);   

	///类型转换，用于将Pixel32位的数据转换为Pixel16位数据，即原始float到ushort
	void Pixel32to16(const Pixel32 & m);  

};

#endif