
#include <reg52.h> 	
#include <intrins.h>	

#include "device_eeprom.h"

typedef enum {	
	true=1,
	false=0,
} bool;

sbit scl=P3^6;		//�������ӿڶ���
sbit sda=P3^7;		//�������ӿڶ���

void delayus() 	//��Ҫ4����������,���4.34us
{
	;					//����Ƶ��11.0592M,��������Ϊ1.085΢��
}

void delayms(unsigned int ms)  //����ʱ����
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
	{
        for(j=113;j>0;j--)
        {}
	}
}

void iic_start()  //�����ź�
{
	sda=1;
	scl=1;
	delayus();		//sda��sclͬΪ�ߵ�ƽ����4.7us����
	_nop_();			//1.085us,��5.78us,����sda=0���½���,���ܼ�������ʱʱ����
	sda=0; 			//�½���
	delayus();		//sda�͵�ƽ����4us����	,������4.34us����Ҫ��
}

void iic_stop()	//ֹͣ�ź�
{
	sda=0;_nop_();	//׼��״̬
	scl=1;
	delayus();		//��״̬�ȶ�ʱ��Ҫ�󱣳�4us����
	sda=1;			//scl�ߵ�ƽ�ڼ�,sda��һ��������
	delayus();		//sda����4.7us����,4.34���Ϻ�������ʱ�����4.7us
						//ע:��ʱscl��sda��Ϊ1	
}

void iic_sendByte(unsigned char byteData) //mcu����һ���ֽ�
{
	unsigned int i;
	unsigned char temp=byteData;
	for(i=0;i<8;i++)
	{
		temp=temp<<1;   //�ƶ������λ����PSW�Ĵ�����CYλ��
		scl=0;			 //׼��
		_nop_();		    //�ȶ�һ��
		sda=CY;			 //�������͵�����һλλ�ķŵ�sda��
		_nop_();
		scl=1;    		 //ÿһ���ߵ�ƽ�ڼ�,ic�������Ὣ����ȡ��
		_nop_();		
	}

	scl=0;				//���д��scl=1;sda=1����ֹͣ�ź�,������ôд
	_nop_();				
	sda=1;				//�ͷ�����,�������߲���ʱҪ�ͷ�
	_nop_();
}

unsigned char iic_readByte() //��һ���ֽ�
{
	unsigned char i,temp;
	scl=0;				//׼��������
	_nop_();
	sda=1;				//�ͷ�����
	_nop_();

	for(i=0;i<8;i++)
	{
		scl=1;			//mcu��ʼȡ����
		delayus();		//sclΪ�ߵ�ƽ��,ic�����ͻὫ1λ�����͵�sda��
							//�ܹ���ʱ�������4.34us��,Ȼ��Ϳ�����mcu��sda��
		temp=(temp<<1)|sda; //��һλ���浽temp��
		scl=0;
		delayus();		
	}
	return temp;
}

bool iic_checkACK()		//����Ӧ���ź�
{
	unsigned char errCounts=255; //���峬ʱ��Ϊ255��
	scl=1;
	_nop_();
	
	while(sda)
	{	//��һ��ʱ���ڼ�⵽sda=0�Ļ���Ϊ��Ӧ���ź�
		if(0==errCounts)
		{
			scl=0;		  //ǯס����
			_nop_();
			return false; //û��Ӧ���ź�
		}
		errCounts--;
	}

	scl=0;			  //ǯס����,Ϊ��1��ͨ����׼�� 
	_nop_();
	return true;	  //�ɹ�����Ӧ���ź�
}

void AT24C02_init()	//���߳�ʼ��
{
	scl=1;
	sda=1;
	delayus();
}

void iic_sendACK(bool b_ACK)	//����Ӧ����Ӧ���ź�
{
	scl=0;			//׼��
	_nop_();

	if(b_ACK)		//ACK
	{
		sda=0;
	}
	else			   //unACK
	{
		sda=1;
	}

	_nop_();
	scl=1;
	delayus(); 		//����4us����ʱ
	scl=0;  	  		//ǯסscl,�Ա������������	
	_nop_();
}


void AT24C02_writeByte(unsigned char address,unsigned char dataByte)//��24c02дһ�ֽ�����
{
	iic_start();
	iic_sendByte(0xa0);//mcuд������,ǰ4λ�̶�1010,����λ��ַ0,ĩλ0��д
	iic_checkACK();		   //mcu����Ӧ���ź�
	iic_sendByte(address);  //׼����ָ����ַ��д��	
	iic_checkACK();
	iic_sendByte(dataByte); //д����
	iic_checkACK();
	iic_stop();
	delayms(2);	
	//���ֽ�д��ʱ,24c02�ڽ��յ�ֹͣ�źź����ݲ�д���ڲ�,����Ҫʱ��
	//���������ʱ���ڲ�����Ӧ�����ϵ��κ�����,����mcu��2�������ϵĵȴ�	
}

void AT24C02_writeData(unsigned char address,unsigned char numBytes,unsigned char* buf)//д�����ⳤ������
{
	while(numBytes--)
	{
		AT24C02_writeByte(address++,*buf++);
	}
}

void AT24C02_readData(unsigned char beginAddr,unsigned char dataSize,unsigned char* buf)//��ȡ���ⳤ���ֽ�
{
	iic_start();					//��ʼ�ź�
	iic_sendByte(0xa0);			//������,д
	iic_checkACK();				//����Ӧ���ź�
	iic_sendByte(beginAddr);	//���͵�ַ
	iic_checkACK();				//����Ӧ���ź�	
	iic_start();			   	//������ʼ�ź�
	iic_sendByte(0xa1);			//������,��
	iic_checkACK();				//����Ӧ���ź�
	while(dataSize--)				//��ȡdataSize���ֽ�
	{
		*buf++=iic_readByte();	//��ȡһ�����ֽڲ����浽������buf��
		iic_sendACK(dataSize);  //����Ӧ��,��dataSizeΪ0ʱ���ͷ�Ӧ��
	}
	iic_stop();						//����ֹͣ�ź�
}