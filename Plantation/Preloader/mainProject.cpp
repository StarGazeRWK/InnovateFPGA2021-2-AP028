#include "mainProject.hpp"
#include "mainImage.hpp"
#include "mainOdour.hpp"
#include "mainCapacitive.hpp"
#include <sys/time.h>
#include "unistd.h"
#include <iostream>
#include <fstream>
#include <string>

extern "C" {
	#include "mainFPGA.h"
}

using namespace std;
//=========================================================
//          GPIO_1_input
//=========================================================
int btn_reading=0x00;
int startBtn_Msk = 0x01;
int stopBtn_Msk = 0x02;

//=========================================================
//          GPIO_1_output
//=========================================================
int gpio1_msk;

//=========================================================
//          GPIO_0_output
//=========================================================
int gpio0_msk;
int gpio0_Imgmsk = 0x01;
int gpio0_Odrmsk = 0x02;
int gpio0_Capmsk = 0x04;
int gpio0_Hybridmsk = 0x08;
int gpio0_Unripemsk = 0x10;
int gpio0_Ripemsk = 0x20;

int ImageBtn_Msk = 0x80;
int OdourBtn_Msk = 0x40;
int CapacitiveBtn_Msk = 0x20;
int HybridBtn_Msk = 0x10;

int resultOdour;
int resultImage;
int resultCapacitive;
int resultHybrid;

//==========================================================
//			Hybrid Task
//==========================================================
int HybridSubroutine()
{
	if(resultImage==0) { return(0); }
	if(resultOdour==0) { return(0); }
	if(resultCapacitive==0) { return(0); }

	if(resultImage==1 && resultOdour==1 && resultCapacitive==1) { return(1); }
	if(resultImage==1 && resultOdour==1 && resultCapacitive==2) { return(3); }
	if(resultImage==1 && resultOdour==2 && resultCapacitive==1) { return(3); }
	if(resultImage==1 && resultOdour==2 && resultCapacitive==2) { return(4); }
	if(resultImage==2 && resultOdour==1 && resultCapacitive==1) { return(3); }
	if(resultImage==2 && resultOdour==1 && resultCapacitive==2) { return(4); }
	if(resultImage==2 && resultOdour==2 && resultCapacitive==1) { return(4); }
	if(resultImage==2 && resultOdour==2 && resultCapacitive==2) { return(2); }
	return(-1);
}

//==========================================================
//			Simple Task
//==========================================================
void print_menu()
{
	cout << "1. Image " 		<< endl;
	cout << "2. Odour "		<< endl;
	cout << "3. Capacitive "	<< endl;
	cout << "4. Hybrid "		<< endl;
}

//==========================================================
//			Main Task
//==========================================================
int main()
{
	if(boardInitialization() != 1)	{ return(0); }

	gpio1_msk = 0x01;
	gpio0_msk = 0x00;
	writeFPGA_GPIO_0_output(gpio0_msk);

	print_menu();

	while(1){
		
		btn_reading=readFPGA_GPIO_1_input();

		if(btn_reading & ImageBtn_Msk) 		
		{ 	
			resultImage=0;
			gpio0_msk = 0x00 | gpio0_Imgmsk; 
			writeFPGA_GPIO_0_output(gpio0_msk);
			usleep(2000*1000);

			resultImage=ImageSubroutine();
			
			cout << "Image Result...";
			if(resultImage==1)
			{
				gpio0_msk = gpio0_msk | gpio0_Ripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Ripe..." << endl;
			}
			
			if(resultImage==2)
			{
				gpio0_msk = gpio0_msk | gpio0_Unripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Unripe..." << endl;
			}
			
			print_menu(); 
		} 

		if(btn_reading & OdourBtn_Msk) 		
		{ 
			resultOdour=0;
			gpio0_msk = 0x00 | gpio0_Odrmsk; 
			writeFPGA_GPIO_0_output(gpio0_msk);
			usleep(2000*1000);

			resultOdour=OdourSubroutine();
			
			cout << "Odour Result...";
			if(resultOdour==1)
			{
				gpio0_msk = gpio0_msk | gpio0_Ripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Ripe..." << endl;
			}
			
			if(resultOdour==2)
			{
				gpio0_msk = gpio0_msk | gpio0_Unripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Unripe..." << endl;
			}
			
			print_menu(); 
		}

		if(btn_reading & CapacitiveBtn_Msk) 	
		{ 	
			resultCapacitive=0;
			gpio0_msk = 0x00 | gpio0_Capmsk;
			writeFPGA_GPIO_0_output(gpio0_msk);
			usleep(2000*1000);

			resultCapacitive=CapacitiveSubroutine(); 
			
			cout << "Capacitive Result...";
			if(resultCapacitive==1)
			{
				gpio0_msk = gpio0_msk | gpio0_Ripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Ripe..." << endl;
			}
			
			if(resultCapacitive==2)
			{
				gpio0_msk = gpio0_msk | gpio0_Unripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Unripe..." << endl;
			}

			print_menu(); 
		}

		if(btn_reading & HybridBtn_Msk) 	
		{ 
			resultHybrid=0;
			resultImage=0;
			resultOdour=0;
			resultCapacitive=0;
			gpio1_msk=0x00;	
			writeFPGA_GPIO_1_output(gpio1_msk);
			usleep(2000*1000);

			cout << "Hybrid Button Pressed..." << endl;
			
			gpio0_msk = 0x00 | gpio0_Imgmsk;
			writeFPGA_GPIO_0_output(gpio0_msk);
			usleep(2000*1000);

			resultImage=ImageSubroutine();
			
			gpio0_msk = gpio0_msk | gpio0_Odrmsk;
			writeFPGA_GPIO_0_output(gpio0_msk);
			usleep(2000*1000);

			resultOdour=OdourSubroutine();

			gpio0_msk = gpio0_msk | gpio0_Capmsk;
			writeFPGA_GPIO_0_output(gpio0_msk);
			usleep(2000*1000);

			resultCapacitive=CapacitiveSubroutine();

			gpio0_msk = gpio0_msk | gpio0_Hybridmsk;
			writeFPGA_GPIO_0_output(gpio0_msk);
			usleep(2000*1000);

			resultHybrid=HybridSubroutine();
			
			cout << "Hybrid Result...";
			if(resultHybrid==1)
			{
				gpio0_msk = gpio0_msk | gpio0_Ripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Ripe..." << endl;
			}
			
			if(resultHybrid==2)
			{
				gpio0_msk = gpio0_msk | gpio0_Unripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Unripe..." << endl;
			}
			
			if(resultHybrid==3)
			{
				gpio0_msk = gpio0_msk | gpio0_Ripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Probably Ripe..." << endl;
			}
			
			if(resultHybrid==4)
			{
				gpio0_msk = gpio0_msk | gpio0_Unripemsk;
				writeFPGA_GPIO_0_output(gpio0_msk);
				cout << " -> Probably Unripe..." << endl;
			}
			
			if(resultHybrid==-1)
			{
				cout << " -> System Error..." << endl;
				int count =0;
				gpio0_msk = gpio0_msk | gpio0_Unripemsk | gpio0_Ripemsk;
				while(count<10)
				{
					writeFPGA_GPIO_0_output(gpio0_msk);
					usleep(500*1000);
					gpio0_msk=~gpio0_msk;
				}
			}

			print_menu(); 
		}
		
		// wait 100ms
		usleep( 100*1000 );
    	}

	if(boardMemoryCleanUp() != 1) { return(0); }
	return(1);
}