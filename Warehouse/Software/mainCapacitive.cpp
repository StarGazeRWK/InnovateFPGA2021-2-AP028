#include "mainCapacitive.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ml.hpp"
#include <sys/time.h>
#include "unistd.h"
#include <iostream>
#include <fstream>
#include <string>

extern "C" {
	#include "mainFPGA.h"
}

using namespace cv;
using namespace std;
using namespace cv::ml;

ofstream fsCapacitive;
std::string filename_Capacitive = "CapacitiveData.csv";
std::string filename_SVMCapacitiveModel = "trained-svmCapacitive.xml";
string realCapacitive_SVM;

string SVMCap_Result;

float CapacitiveDataTest[1][40];

//=========================================================
//          GPIO_1_input
//=========================================================
int btn_read_cap =0x00;
int startBtn_cap = 0x01;
int stopBtn_cap = 0x02;

//=========================================================
//          GPIO_1_output
//=========================================================
int gpio1_cap;


//=========================================================
//          Capacitive
//=========================================================
float Cap_S0=0.0;
float Cap_S1=0.0;
float Cap_S2=0.0;
float Cap_S3=0.0;
float Cap_S4=0.0;
float Cap_S5=0.0;
float Cap_S6=0.0;
float Cap_S7=0.0;

float MaxCap_S0=0.0;
float MaxCap_S1=0.0;
float MaxCap_S2=0.0;
float MaxCap_S3=0.0;
float MaxCap_S4=0.0;
float MaxCap_S5=0.0;
float MaxCap_S6=0.0;
float MaxCap_S7=0.0;

float MinCap_S0=0.0;
float MinCap_S1=0.0;
float MinCap_S2=0.0;
float MinCap_S3=0.0;
float MinCap_S4=0.0;
float MinCap_S5=0.0;
float MinCap_S6=0.0;
float MinCap_S7=0.0;

float SumCap_S0=0.0;
float SumCap_S1=0.0;
float SumCap_S2=0.0;
float SumCap_S3=0.0;
float SumCap_S4=0.0;
float SumCap_S5=0.0;
float SumCap_S6=0.0;
float SumCap_S7=0.0;

float meanCap_S0=0.0;
float meanCap_S1=0.0;
float meanCap_S2=0.0;
float meanCap_S3=0.0;
float meanCap_S4=0.0;
float meanCap_S5=0.0;
float meanCap_S6=0.0;
float meanCap_S7=0.0;

float stdDevCap_S0=0.0;
float stdDevCap_S1=0.0;
float stdDevCap_S2=0.0;
float stdDevCap_S3=0.0;
float stdDevCap_S4=0.0;
float stdDevCap_S5=0.0;
float stdDevCap_S6=0.0;
float stdDevCap_S7=0.0;

float varCap_S0=0.0;
float varCap_S1=0.0;
float varCap_S2=0.0;
float varCap_S3=0.0;
float varCap_S4=0.0;
float varCap_S5=0.0;
float varCap_S6=0.0;
float varCap_S7=0.0;

float DataStdDevCap_S0=0;
float DataStdDevCap_S1=0;
float DataStdDevCap_S2=0;
float DataStdDevCap_S3=0;
float DataStdDevCap_S4=0;
float DataStdDevCap_S5=0;
float DataStdDevCap_S6=0;
float DataStdDevCap_S7=0;

int modeCapacitive =1;

//==========================================================
//			Capacitive Task
//==========================================================
void readCapacitiveData() {
	cout << "Capacitive Data Start to Collect..." <<endl;

	fsCapacitive.open(filename_Capacitive,ios::out |ios::trunc);
	fsCapacitive << "S0" << ","
	   << "S1" << ","
	   << "S2" << ","
	   << "S3" << ","
	   << "S4" << ","
	   << "S5" << ","
	   << "S6" << ","
	   << "S7" << "\n";
	int i=0;

	Cap_S0=0.0;
	Cap_S1=0.0;
	Cap_S2=0.0;
	Cap_S3=0.0;
	Cap_S4=0.0;
	Cap_S5=0.0;
	Cap_S6=0.0;
	Cap_S7=0.0;
	
	SumCap_S0=0.0;
	SumCap_S1=0.0;
	SumCap_S2=0.0;
	SumCap_S3=0.0;
	SumCap_S4=0.0;
	SumCap_S5=0.0;
	SumCap_S6=0.0;
	SumCap_S7=0.0;
	
	meanCap_S0=0.0;
	meanCap_S1=0.0;
	meanCap_S2=0.0;
	meanCap_S3=0.0;
	meanCap_S4=0.0;
	meanCap_S5=0.0;
	meanCap_S6=0.0;
	meanCap_S7=0.0;
	
	stdDevCap_S0=0.0;
	stdDevCap_S1=0.0;
	stdDevCap_S2=0.0;
	stdDevCap_S3=0.0;
	stdDevCap_S4=0.0;
	stdDevCap_S5=0.0;
	stdDevCap_S6=0.0;
	stdDevCap_S7=0.0;
	
	varCap_S0=0.0;
	varCap_S1=0.0;
	varCap_S2=0.0;
	varCap_S3=0.0;
	varCap_S4=0.0;
	varCap_S5=0.0;
	varCap_S6=0.0;
	varCap_S7=0.0;
	
	DataStdDevCap_S0=0;
	DataStdDevCap_S1=0;
	DataStdDevCap_S2=0;
	DataStdDevCap_S3=0;
	DataStdDevCap_S4=0;
	DataStdDevCap_S5=0;
	DataStdDevCap_S6=0;
	DataStdDevCap_S7=0;
	
	MaxCap_S0=0.0;
	MaxCap_S1=0.0;
	MaxCap_S2=0.0;
	MaxCap_S3=0.0;
	MaxCap_S4=0.0;
	MaxCap_S5=0.0;
	MaxCap_S6=0.0;
	MaxCap_S7=0.0;
	
	MinCap_S0=1000.0;
	MinCap_S1=1000.0;
	MinCap_S2=1000.0;
	MinCap_S3=1000.0;
	MinCap_S4=1000.0;
	MinCap_S5=1000.0;
	MinCap_S6=1000.0;
	MinCap_S7=1000.0;

	do{	
		Cap_S0=readFPGA_ADC(0);
		Cap_S1=readFPGA_ADC(1);
		Cap_S2=readFPGA_ADC(2);
		Cap_S3=readFPGA_ADC(3);
		Cap_S4=readFPGA_ADC(4);
		Cap_S5=readFPGA_ADC(5);
		Cap_S6=readFPGA_ADC(6);
		Cap_S7=readFPGA_ADC(7);

		btn_read_cap=readFPGA_GPIO_1_input();

		if(~btn_read_cap & startBtn_cap)
		{
			if(Cap_S0>MaxCap_S0)	{ MaxCap_S0=Cap_S0; }
			if(Cap_S1>MaxCap_S1)	{ MaxCap_S1=Cap_S1; }
			if(Cap_S2>MaxCap_S2)	{ MaxCap_S2=Cap_S2; }
			if(Cap_S3>MaxCap_S3)	{ MaxCap_S3=Cap_S3; }
			if(Cap_S4>MaxCap_S4)	{ MaxCap_S4=Cap_S4; }
			if(Cap_S5>MaxCap_S5)	{ MaxCap_S5=Cap_S5; }
			if(Cap_S6>MaxCap_S6)	{ MaxCap_S6=Cap_S6; }
			if(Cap_S7>MaxCap_S7)	{ MaxCap_S7=Cap_S7; }

			if(Cap_S0<MinCap_S0)	{ MinCap_S0=Cap_S0; }
			if(Cap_S1<MinCap_S1)	{ MinCap_S1=Cap_S1; }
			if(Cap_S2<MinCap_S2)	{ MinCap_S2=Cap_S2; }
			if(Cap_S3<MinCap_S3)	{ MinCap_S3=Cap_S3; }
			if(Cap_S4<MinCap_S4)	{ MinCap_S4=Cap_S4; }
			if(Cap_S5<MinCap_S5)	{ MinCap_S5=Cap_S5; }
			if(Cap_S6<MinCap_S6)	{ MinCap_S6=Cap_S6; }
			if(Cap_S7<MinCap_S7)	{ MinCap_S7=Cap_S7; }

			SumCap_S0=SumCap_S0+Cap_S0;
			SumCap_S1=SumCap_S1+Cap_S1;
			SumCap_S2=SumCap_S2+Cap_S2;
			SumCap_S3=SumCap_S3+Cap_S3;
			SumCap_S4=SumCap_S4+Cap_S4;
			SumCap_S5=SumCap_S5+Cap_S5;
			SumCap_S6=SumCap_S6+Cap_S6;
			SumCap_S7=SumCap_S7+Cap_S7;

			meanCap_S0=(SumCap_S0)/(i+1);
			meanCap_S1=(SumCap_S1)/(i+1);
			meanCap_S2=(SumCap_S2)/(i+1);
			meanCap_S3=(SumCap_S3)/(i+1);
			meanCap_S4=(SumCap_S4)/(i+1);
			meanCap_S5=(SumCap_S5)/(i+1);
			meanCap_S6=(SumCap_S6)/(i+1);
			meanCap_S7=(SumCap_S7)/(i+1);
			
			DataStdDevCap_S0 = DataStdDevCap_S0 + pow(Cap_S0 - meanCap_S0, 2);
			varCap_S0=DataStdDevCap_S0/(i+1);
			stdDevCap_S0=sqrt(varCap_S0);
			
			DataStdDevCap_S1 = DataStdDevCap_S1 + pow(Cap_S1 - meanCap_S1, 2);
			varCap_S1=DataStdDevCap_S1/(i+1);
			stdDevCap_S1=sqrt(varCap_S1);
			
			DataStdDevCap_S2 = DataStdDevCap_S2 + pow(Cap_S2 - meanCap_S2, 2);
			varCap_S2=DataStdDevCap_S2/(i+1);
			stdDevCap_S2=sqrt(varCap_S2);

			DataStdDevCap_S3 = DataStdDevCap_S3 + pow(Cap_S3 - meanCap_S3, 2);
			varCap_S3=DataStdDevCap_S3/(i+1);
			stdDevCap_S3=sqrt(varCap_S3);

			DataStdDevCap_S4 = DataStdDevCap_S4 + pow(Cap_S4 - meanCap_S4, 2);
			varCap_S4=DataStdDevCap_S4/(i+1);
			stdDevCap_S4=sqrt(varCap_S4);

			DataStdDevCap_S5 = DataStdDevCap_S5 + pow(Cap_S5 - meanCap_S5, 2);
			varCap_S5=DataStdDevCap_S5/(i+1);
			stdDevCap_S5=sqrt(varCap_S5);

			DataStdDevCap_S6 = DataStdDevCap_S6 + pow(Cap_S6 - meanCap_S6, 2);
			varCap_S6=DataStdDevCap_S6/(i+1);
			stdDevCap_S6=sqrt(varCap_S6);

			DataStdDevCap_S7 = DataStdDevCap_S7 + pow(Cap_S7 - meanCap_S7, 2);
			varCap_S7=DataStdDevCap_S7/(i+1);
			stdDevCap_S7=sqrt(varCap_S7);
			
			fsCapacitive 
			   << MinCap_S0 << ","
			   << MinCap_S1 << ","
			   << MinCap_S2 << ","
			   << MinCap_S3 << ","
			   << MinCap_S4 << ","
			   << MinCap_S5 << ","
			   << MinCap_S6 << ","
			   << MinCap_S7 << ","

			   << MaxCap_S0 << ","
			   << MaxCap_S1 << ","
			   << MaxCap_S2 << ","
			   << MaxCap_S3 << ","
			   << MaxCap_S4 << ","
			   << MaxCap_S5 << ","
			   << MaxCap_S6 << ","
			   << MaxCap_S7 << ","

			   << meanCap_S0 << ","
			   << meanCap_S1 << ","
			   << meanCap_S2 << ","
			   << meanCap_S3 << ","
			   << meanCap_S4 << ","
			   << meanCap_S5 << ","
			   << meanCap_S6 << ","
			   << meanCap_S7 << ","
			   
			   << stdDevCap_S0 << ","
			   << stdDevCap_S1 << ","
			   << stdDevCap_S2 << ","
			   << stdDevCap_S3 << ","
			   << stdDevCap_S4 << ","
			   << stdDevCap_S5 << ","
                           << stdDevCap_S6 << ","
			   << stdDevCap_S7 << ","
			   
			   << varCap_S0 << ","
			   << varCap_S1 << ","
			   << varCap_S2 << ","
			   << varCap_S3 << ","
			   << varCap_S4 << ","
			   << varCap_S5 << ","
			   << varCap_S6 << ","
			   << varCap_S7 << "\n";

			cout << "Recorded " << i << endl;

			i++;
		}

		usleep(100*1000);

	}while(~btn_read_cap & stopBtn_cap);
	
	CapacitiveDataTest[0][0]=MinCap_S0;
	CapacitiveDataTest[0][1]=MinCap_S1;
	CapacitiveDataTest[0][2]=MinCap_S2;
	CapacitiveDataTest[0][3]=MinCap_S3;
	CapacitiveDataTest[0][4]=MinCap_S4;
	CapacitiveDataTest[0][5]=MinCap_S5;
	CapacitiveDataTest[0][6]=MinCap_S6;
	CapacitiveDataTest[0][7]=MinCap_S7;
		
	CapacitiveDataTest[0][8]=MaxCap_S0;
	CapacitiveDataTest[0][9]=MaxCap_S1;
	CapacitiveDataTest[0][10]=MaxCap_S2;
	CapacitiveDataTest[0][11]=MaxCap_S3;
	CapacitiveDataTest[0][12]=MaxCap_S4;
	CapacitiveDataTest[0][13]=MaxCap_S5;
	CapacitiveDataTest[0][14]=MaxCap_S6;
	CapacitiveDataTest[0][15]=MaxCap_S7;

	CapacitiveDataTest[0][16]=meanCap_S0;
	CapacitiveDataTest[0][17]=meanCap_S1;
	CapacitiveDataTest[0][18]=meanCap_S2;
	CapacitiveDataTest[0][19]=meanCap_S3;
	CapacitiveDataTest[0][20]=meanCap_S4;
	CapacitiveDataTest[0][21]=meanCap_S5;
	CapacitiveDataTest[0][22]=meanCap_S6;
	CapacitiveDataTest[0][23]=meanCap_S7;
	
	CapacitiveDataTest[0][24]=stdDevCap_S0;
	CapacitiveDataTest[0][25]=stdDevCap_S1;
	CapacitiveDataTest[0][26]=stdDevCap_S2;
	CapacitiveDataTest[0][27]=stdDevCap_S3;
	CapacitiveDataTest[0][28]=stdDevCap_S4;
	CapacitiveDataTest[0][29]=stdDevCap_S5;
	CapacitiveDataTest[0][30]=stdDevCap_S6;
	CapacitiveDataTest[0][31]=stdDevCap_S7;
	
	CapacitiveDataTest[0][32]=varCap_S0;
	CapacitiveDataTest[0][33]=varCap_S1;
	CapacitiveDataTest[0][34]=varCap_S2;
	CapacitiveDataTest[0][35]=varCap_S3;
	CapacitiveDataTest[0][36]=varCap_S4;
	CapacitiveDataTest[0][37]=varCap_S5;
	CapacitiveDataTest[0][38]=varCap_S6;
	CapacitiveDataTest[0][39]=varCap_S7;

	fsCapacitive.close();
	printf("Stop Capacitive Button Pressed...\n");
}

void testCapacitive_SVM()
{
	Ptr<SVM> svm = StatModel::load<SVM>(filename_SVMCapacitiveModel);
	SVMCap_Result = "";

	Mat sampleMat(1, 40, CV_32F, CapacitiveDataTest);
	float response = svm->predict(sampleMat);

	if (response == -1)
		SVMCap_Result = "Ripe";
	else if (response == 1)
		SVMCap_Result = "Unripe";
	
	realCapacitive_SVM = SVMCap_Result;
}

int CapacitiveSubroutine()
{
	cout << "Capacitive Button Pressed..." << endl;
	gpio1_cap=0xff;	
	writeFPGA_GPIO_1_output(gpio1_cap);

	readCapacitiveData();
	testCapacitive_SVM();

	if(realCapacitive_SVM == "Ripe") 	{ return(1); }
	if(realCapacitive_SVM == "Unripe") 	{ return(2); }

	return(0);
}