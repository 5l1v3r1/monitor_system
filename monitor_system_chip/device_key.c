
#include <reg52.h>

#include "device_key.h"

sbit H1=P0^4;			  //����λ�ֱ��ӦP04-P07
sbit H2=P0^5;
sbit H3=P0^6;
sbit H4=P0^7;

int scankey(void)
{ 
		P0=0xfe;		  		/// ��Ҫ�����˿ڵ�ʱ�����ҲҪ�޸�
		if(H1==0)return 0;	
		if(H2==0)return 1;			
		if(H3==0)return 2;			
		if(H4==0)return 3;			 
		P0=0xfd;
		if(H1==0)return 4;		
		if(H2==0)return 5;		
		if(H3==0)return 6;		
		if(H4==0)return 7;		
		P0=0xfb;
		if(H1==0)return 8;			
		if(H2==0)return 9;			
		if(H3==0)return 10;			
		if(H4==0)return 11;			
		P0=0xf7;
		if(H1==0)return 12;			 
		if(H2==0)return 13;		 
		if(H3==0)return 14;		
		if(H4==0)return 15;		
     	return -1;
}	 
