/*********************************************************************//**
* \file      Pixel.h
* \brief     �Զ���ͼ�����ص���������
* \author    ��ï��
* \version   1.0b
* \date      2016��4��
* \copyright ���ļ���Ȩ���廪��ѧ����
**************************************************************************/

#ifndef  PIXEL_H
#define  PIXEL_H

/*********************************************************************//**
* \brief   ����Pixel32�࣬���ڱ����м���������Ϊfloat��
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
class Pixel32
{
public:
	///���캯��
	Pixel32(float d = 0):datatmp(d) 
	{
	}
	///��������
	~Pixel32()
	{
	}
	float datatmp;   ///<��������Ϊfloat
};

/*********************************************************************//**
* \brief   ����Pixel16�࣬��Ϊͼ���������ݵ��������� �����س��������
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
class Pixel16
{
public:
	ushort data;    ///< �˴�ͼ��Ϊ16λ��ushort

	///���캯��
	Pixel16()
	{
		data = 0;
	};

	///�������Ĺ��캯��
	Pixel16(ushort a)
	{
		data = a;
	}

	///��������
	~Pixel16(void)
	{
	}

	///ʵ��" = "���أ��Ҷ�Ϊushort ���ͣ����� Pixel16  value = 10;
	Pixel16 & operator=(const ushort &a)
	{
		data = a;
		return *this;
	}

	///ʵ��" = "���أ��Ҷ�ΪPixel16l�����
	Pixel16 & operator=(const Pixel16 &m)
	{
		data = m.data;
		return *this;
	}
	///����" + " �������Pixel16 + Pixel16
	friend Pixel32 operator+(const Pixel16 &m ,const Pixel16 &n); 

	///����" + " �������Pixel32 + Pixel16
	friend Pixel32 operator+(const Pixel32 &m, const Pixel16 &n);

	///����" + " �������Pixel16 + Pixel32
	friend Pixel32 operator+(const Pixel16 &m, const Pixel32 &n);

	///����" * " �������Pixel16 * float
	friend Pixel32 operator*(const Pixel16 &m ,const float &n); 

	///����" * " �������float * Pixel16
	friend Pixel32 operator*(const float &m ,const Pixel16 &n); 

	///����" / " �������Pixel32 / ushort
	friend Pixel32 operator/(const Pixel32 &m, const ushort &n);   

	///����ת�������ڽ�Pixel32λ������ת��ΪPixel16λ���ݣ���ԭʼfloat��ushort
	void Pixel32to16(const Pixel32 & m);  

};

#endif