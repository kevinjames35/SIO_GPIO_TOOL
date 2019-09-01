
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include <dlfcn.h>

#define int8_t  		char
#define int16_t 		short
#define int32_t 		int
#define int64_t  		long long
#define uint8_t  		unsigned char
#define uint16_t 		unsigned short
#define uint32_t 		unsigned int
#define uint64_t 		unsigned long long


#define SIO_CFGINDEX	0x2E
#define SIO_CFGDATA	0x2F


#define REG_GPIO_ENABLE	0x30

#define REG_GPIO_0_OUT_EN	0xF0	//0:input/1:output
#define REG_GPIO_0_OUT_DATA	0xF1	//output value
#define REG_GPIO_0_PIN_STAT	0xF2	//read only
#define REG_GPIO_0_DRIVE_EN	0xF3	//0:open drain/1:push pull

#define REG_GPIO_1_OUT_EN	0xE0	//0:input/1:output
#define REG_GPIO_1_OUT_DATA	0xE1	//output value
#define REG_GPIO_1_PIN_STAT	0xE2	//read only
#define REG_GPIO_1_DRIVE_EN	0xE3	//0:open drain/1:push pull

#define REG_GPIO_2_OUT_EN	0xD0	//0:input/1:output
#define REG_GPIO_2_OUT_DATA	0xD1	//output value
#define REG_GPIO_2_PIN_STAT	0xD2	//read only
#define REG_GPIO_2_DRIVE_EN	0xD3	//0:open drain/1:push pull

#define REG_GPIO_3_OUT_EN	0xC0	//0:input/1:output
#define REG_GPIO_3_OUT_DATA	0xC1	//output value
#define REG_GPIO_3_PIN_STAT	0xC2	//read only
#define REG_GPIO_3_DRIVE_EN	0xC3	//0:open drain/1:push pull

#define REG_GPIO_4_OUT_EN	0xB0	//0:input/1:output
#define REG_GPIO_4_OUT_DATA	0xB1	//output value
#define REG_GPIO_4_PIN_STAT	0xB2	//read only
#define REG_GPIO_4_DRIVE_EN	0xB3	//0:open drain/1:push pull

#define REG_GPIO_5_OUT_EN	0xA0	//0:input/1:output
#define REG_GPIO_5_OUT_DATA	0xA1	//output value
#define REG_GPIO_5_PIN_STAT	0xA2	//read only
#define REG_GPIO_5_DRIVE_EN	0xA3	//0:open drain/1:push pull

#define REG_GPIO_6_OUT_EN	0x90	//0:input/1:output
#define REG_GPIO_6_OUT_DATA	0x91	//output value
#define REG_GPIO_6_PIN_STAT	0x92	//read only
#define REG_GPIO_6_DRIVE_EN	0x93	//0:open drain/1:push pull

#define REG_GPIO_7_OUT_EN	0x80	//0:input/1:output
#define REG_GPIO_7_OUT_DATA	0x81	//output value
#define REG_GPIO_7_PIN_STAT	0x82	//read only
#define REG_GPIO_7_DRIVE_EN	0x83	//0:open drain/1:push pull

#define REG_GPIO_8_OUT_EN	0x88	//0:input/1:output
#define REG_GPIO_8_OUT_DATA	0x89	//output value
#define REG_GPIO_8_PIN_STAT	0x8A	//read only
#define REG_GPIO_8_DRIVE_EN	0x8B	//0:open drain/1:push pull

void __sio_unlock(void);
void __sio_lock(void);
void __sio_logic_device(char num);






/********************************************************************/
/***** SuperIO access functions *************************************/
void __sio_unlock(void)
{
	ioperm(SIO_CFGINDEX, 2, 1);
	outb(0x87 , SIO_CFGINDEX);
	outb(0x87 , SIO_CFGINDEX);
}
/***********/
void __sio_lock(void)
{
	outb(0xaa , SIO_CFGINDEX);
	ioperm(SIO_CFGINDEX, 2, 0);
}
/***********/
void __sio_logic_device(char num)
{
	outb(0x7 , SIO_CFGINDEX);
	outb(num , SIO_CFGDATA);
}
/************/
uint8_t read_sio_reg(uint8_t LDN, uint8_t reg)
{
        outb(0x07, SIO_CFGINDEX); //LDN register
        outb(LDN, SIO_CFGDATA);
        outb(reg , SIO_CFGINDEX);
        return inb(SIO_CFGDATA);
}
/************/
uint8_t write_sio_reg(uint8_t LDN, uint8_t reg, uint8_t value)
{	
        outb(0x07, SIO_CFGINDEX); //LDN register
        outb(LDN, SIO_CFGDATA);
        outb(reg, SIO_CFGINDEX);
        outb(value, SIO_CFGDATA);
        return 0;
}
/****************/


void __printf_usage()
{
	printf("Usage: rs_set -com1/2/3/4/5/6 -232/-422/-485 	--> setting comport1/2/3/4/5/6 interface RS232/422/485\n");
	
}

void _show_delay(int32_t sec)
{
int32_t index=sec;
	while ( index>0) {
		printf("%d. ", index--);
		fflush(stdout);
		sleep(1);
	}
	printf("%d.\n", index);
}

int main( int argc, char** argv )
{
int iRet;
uint8_t bsel=0,ubRead=0xFF;
int32_t dwValue;
int32_t dwRead;	
int32_t dwDelay=2;
uint8_t* pubStatus;
uint16_t wAddr =0, xData=0;
uint8_t xch;
uint32_t udwReg; 
uint8_t bData;
	if ( argc < 2 ) {
		__printf_usage();
		return -1;
	}

	if 	( strcmp("-com1", argv[1]) == 0 ) bsel = 0;
	else if ( strcmp("-com2", argv[1]) == 0 ) bsel = 1;
	else if ( strcmp("-com3", argv[1]) == 0 ) bsel = 2;
	else if ( strcmp("-com4", argv[1]) == 0 ) bsel = 3;
	else if ( strcmp("-com5", argv[1]) == 0 ) bsel = 4;
	else if ( strcmp("-com6", argv[1]) == 0 ) bsel = 5;
	else {
		printf("<Error> Input parameter invaild \n");
		__printf_usage();
		return -1;
	}
__sio_unlock();
__sio_logic_device(0x06);
	outb( 0x30, SIO_CFGINDEX);
	xch = inb(SIO_CFGDATA)| 0x01 ;  
	outb( 0x30, SIO_CFGINDEX);
	outb( xch , SIO_CFGDATA);	//
switch ( bsel ) {
	case 0:
		if 	( strcmp("-232", argv[2]) == 0 )
		{		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xFC;
			xData |=0x01;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else if ( strcmp("-422", argv[2]) == 0 )
		{
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xFC;
			xData |=0x03;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else if ( strcmp("-485", argv[2]) == 0 )
		{
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xFC;
			xData |=0x02;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else {
			printf("<Error> Input parameter invaild \n");
			__printf_usage();
			return -1;
		}
	break;
	case 1:
		if 	( strcmp("-232", argv[2]) == 0 )
		{		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xE7;
			xData |=0x08;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else if ( strcmp("-422", argv[2]) == 0 )
		{
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xE7;
			xData |=0x18;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else if ( strcmp("-485", argv[2]) == 0 )
		{
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xE7;
			xData |=0x10;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else {
			printf("<Error> Input parameter invaild \n");
			__printf_usage();
			return -1;
		}
	break;
	case 2:
		if 	( strcmp("-232", argv[2]) == 0 )
		{		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0x3F;
			xData |=0x40;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else if ( strcmp("-422", argv[2]) == 0 )
		{
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0x3F;
			xData |=0xC0;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else if ( strcmp("-485", argv[2]) == 0 )
		{
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0x3F;
			xData |=0x80;		
			outb(REG_GPIO_7_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_7_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_7_PIN_STAT,xData);
		}
		else {
			printf("<Error> Input parameter invaild \n");
			__printf_usage();
			return -1;
		}
	break;
	case 3:
		if 	( strcmp("-232", argv[2]) == 0 )
		{		
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xF9;
			xData |=0x02;		
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
			outb(REG_GPIO_8_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_8_PIN_STAT,xData);
		}
		else if ( strcmp("-422", argv[2]) == 0 )
		{
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xF9;
			xData |=0x06;		
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_8_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_8_PIN_STAT,xData);
		}
		else if ( strcmp("-485", argv[2]) == 0 )
		{
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xF9;
			xData |=0x04;		
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_8_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_8_PIN_STAT,xData);
		}
		else {
			printf("<Error> Input parameter invaild \n");
			__printf_usage();
			return -1;
		}
	break;
	case 4:
		if 	( strcmp("-232", argv[2]) == 0 )
		{		
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xCF;
			xData |=0x10;		
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
			outb(REG_GPIO_8_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_8_PIN_STAT,xData);
		}
		else if ( strcmp("-422", argv[2]) == 0 )
		{
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xCF;
			xData |=0x30;		
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_8_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_8_PIN_STAT,xData);
		}
		else if ( strcmp("-485", argv[2]) == 0 )
		{
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xCF;
			xData |=0x20;		
			outb(REG_GPIO_8_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_8_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_8_PIN_STAT,xData);
		}
		else {
			printf("<Error> Input parameter invaild \n");
			__printf_usage();
			return -1;
		}
	break;
	case 5:
		if 	( strcmp("-232", argv[2]) == 0 )
		{		
			outb(REG_GPIO_1_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xFC;
			xData |=0x01;		
			outb(REG_GPIO_1_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
			outb(REG_GPIO_1_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_1_PIN_STAT,xData);
		}
		else if ( strcmp("-422", argv[2]) == 0 )
		{
			outb(REG_GPIO_1_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xFC;
			xData |=0x03;		
			outb(REG_GPIO_1_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_1_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_1_PIN_STAT,xData);
		}
		else if ( strcmp("-485", argv[2]) == 0 )
		{
			outb(REG_GPIO_1_OUT_DATA, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA) & 0xFC;
			xData |=0x02;		
			outb(REG_GPIO_1_OUT_DATA, SIO_CFGINDEX);
			outb(xData,SIO_CFGDATA);
	
	
			outb(REG_GPIO_1_PIN_STAT, SIO_CFGINDEX);
			xData=(uint16_t)inb(SIO_CFGDATA);
			printf("0x%X = 0x%X\n",REG_GPIO_1_PIN_STAT,xData);
		}
		else {
			printf("<Error> Input parameter invaild \n");
			__printf_usage();
			return -1;
		}
	break;
	default:
	break;

}

__sio_lock();
return 0;
}
/*
int main( int argc, char** argv )
{
uint16_t wAddr =0, xData=0;
	
	
	

for(int xi=0;xi<=255;xi++)
{
	outb(xi, SIO_CFGINDEX);
	xData=(uint16_t)inb(SIO_CFGDATA);
printf("0x%X = 0x%X\n",xi,xData);
}

	
	
	
	
}
*/
