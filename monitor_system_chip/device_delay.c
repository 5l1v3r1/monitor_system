
#include "device_delay.h"

//  ��12MHz �µľ�ȷ��ʱ
void delay_50us(unsigned int time_delay_count) {
	unsigned char j=19;  
	for(;time_delay_count>0;time_delay_count--)   
		for(;j>0;j--);
}