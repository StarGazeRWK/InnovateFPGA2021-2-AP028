#include "mainOdour.hpp"
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

ofstream fsOdour;
std::string filename_Odour = "OdourData.csv";
std::string filename_SVMOdourModel = "trained-svmOdour.xml";
string realOdour_SVM;
string SVM_Result;

float OdourDataTest[1][32];

//=========================================================
//          GPIO_1_input
//=========================================================
int btn_read =0x00;
int startBtn = 0x01;
int stopBtn = 0x02;

//=========================================================
//          GPIO_1_output
//=========================================================
int gpio1;

//=========================================================
//          Odour
//=========================================================
float Odr_S0=0.0;
float Odr_S1=0.0;
float Odr_S2=0.0;
float Odr_S3=0.0;
float Odr_S4=0.0;
float Odr_S5=0.0;
float Odr_S6=0.0;
float Odr_S7=0.0;

float SumOdr_S0=0.0;
float SumOdr_S1=0.0;
float SumOdr_S2=0.0;
float SumOdr_S3=0.0;
float SumOdr_S4=0.0;
float SumOdr_S5=0.0;
float SumOdr_S6=0.0;
float SumOdr_S7=0.0;

float meanOdr_S0=0.0;
float meanOdr_S1=0.0;
float meanOdr_S2=0.0;
float meanOdr_S3=0.0;
float meanOdr_S4=0.0;
float meanOdr_S5=0.0;
float meanOdr_S6=0.0;
float meanOdr_S7=0.0;

float stdDevOdr_S0=0.0;
float stdDevOdr_S1=0.0;
float stdDevOdr_S2=0.0;
float stdDevOdr_S3=0.0;
float stdDevOdr_S4=0.0;
float stdDevOdr_S5=0.0;
float stdDevOdr_S6=0.0;
float stdDevOdr_S7=0.0;

float varOdr_S0=0.0;
float varOdr_S1=0.0;
float varOdr_S2=0.0;
float varOdr_S3=0.0;
float varOdr_S4=0.0;
float varOdr_S5=0.0;
float varOdr_S6=0.0;
float varOdr_S7=0.0;

float DataStdDev_S0=0;
float DataStdDev_S1=0;
float DataStdDev_S2=0;
float DataStdDev_S3=0;
float DataStdDev_S4=0;
float DataStdDev_S5=0;
float DataStdDev_S6=0;
float DataStdDev_S7=0;

int modeOdour =1;

//==========================================================
//			Odour Task
//==========================================================
void readOdourData() {
	cout << "Odour Data Start to Collect..." << endl;
	
	fsOdour.open(filename_Odour,ios::out|ios::trunc);
	fsOdour << "S0" << ","
		<< "S1" << ","
		<< "S2" << ","
		<< "S3" << ","
		<< "S4" << ","
		<< "S5" << ","
		<< "S6" << ","
	 	<< "S7" << "\n";

	int i=0;
	
	Odr_S0=0.0;
	Odr_S1=0.0;
	Odr_S2=0.0;
	Odr_S3=0.0;
	Odr_S4=0.0;
	Odr_S5=0.0;
	Odr_S6=0.0;
	Odr_S7=0.0;
	
	SumOdr_S0=0.0;
	SumOdr_S1=0.0;
	SumOdr_S2=0.0;
	SumOdr_S3=0.0;
	SumOdr_S4=0.0;
	SumOdr_S5=0.0;
	SumOdr_S6=0.0;
	SumOdr_S7=0.0;
	
	meanOdr_S0=0.0;
	meanOdr_S1=0.0;
	meanOdr_S2=0.0;
	meanOdr_S3=0.0;
	meanOdr_S4=0.0;
	meanOdr_S5=0.0;
	meanOdr_S6=0.0;
	meanOdr_S7=0.0;
	
	stdDevOdr_S0=0.0;
	stdDevOdr_S1=0.0;
	stdDevOdr_S2=0.0;
	stdDevOdr_S3=0.0;
	stdDevOdr_S4=0.0;
	stdDevOdr_S5=0.0;
	stdDevOdr_S6=0.0;
	stdDevOdr_S7=0.0;
	
	varOdr_S0=0.0;
	varOdr_S1=0.0;
	varOdr_S2=0.0;
	varOdr_S3=0.0;
	varOdr_S4=0.0;
	varOdr_S5=0.0;
	varOdr_S6=0.0;
	varOdr_S7=0.0;
	
	DataStdDev_S0=0;
	DataStdDev_S1=0;
	DataStdDev_S2=0;
	DataStdDev_S3=0;
	DataStdDev_S4=0;
	DataStdDev_S5=0;
	DataStdDev_S6=0;
	DataStdDev_S7=0;

	do{	
		Odr_S0=readFPGA_ADC(0);
		Odr_S1=readFPGA_ADC(1);
		Odr_S2=readFPGA_ADC(2);
		Odr_S3=readFPGA_ADC(3);
		Odr_S4=readFPGA_ADC(4);
		Odr_S5=readFPGA_ADC(5);
		Odr_S6=readFPGA_ADC(6);
		Odr_S7=readFPGA_ADC(7);

		btn_read =readFPGA_GPIO_1_input();

		if(~btn_read & startBtn)
		{
			SumOdr_S0=SumOdr_S0+Odr_S0;
			SumOdr_S1=SumOdr_S1+Odr_S1;
			SumOdr_S2=SumOdr_S2+Odr_S2;
			SumOdr_S3=SumOdr_S3+Odr_S3;
			SumOdr_S4=SumOdr_S4+Odr_S4;
			SumOdr_S5=SumOdr_S5+Odr_S5;
			SumOdr_S6=SumOdr_S6+Odr_S6;
			SumOdr_S7=SumOdr_S7+Odr_S7;

			meanOdr_S0=(SumOdr_S0)/(i+1);
			meanOdr_S1=(SumOdr_S1)/(i+1);
			meanOdr_S2=(SumOdr_S2)/(i+1);
			meanOdr_S3=(SumOdr_S3)/(i+1);
			meanOdr_S4=(SumOdr_S4)/(i+1);
			meanOdr_S5=(SumOdr_S5)/(i+1);
			meanOdr_S6=(SumOdr_S6)/(i+1);
			meanOdr_S7=(SumOdr_S7)/(i+1);
			
			DataStdDev_S0 = DataStdDev_S0 + pow(Odr_S0 - meanOdr_S0, 2);
			varOdr_S0=DataStdDev_S0/(i+1);
			stdDevOdr_S0=sqrt(varOdr_S0);
			
			DataStdDev_S1 = DataStdDev_S1 + pow(Odr_S1 - meanOdr_S1, 2);
			varOdr_S1=DataStdDev_S1/(i+1);
			stdDevOdr_S1=sqrt(varOdr_S1);
			
			DataStdDev_S2 = DataStdDev_S2 + pow(Odr_S2 - meanOdr_S2, 2);
			varOdr_S2=DataStdDev_S2/(i+1);
			stdDevOdr_S2=sqrt(varOdr_S2);

			DataStdDev_S3 = DataStdDev_S3 + pow(Odr_S3 - meanOdr_S3, 2);
			varOdr_S3=DataStdDev_S3/(i+1);
			stdDevOdr_S3=sqrt(varOdr_S3);

			DataStdDev_S4 = DataStdDev_S4 + pow(Odr_S4 - meanOdr_S4, 2);
			varOdr_S4=DataStdDev_S4/(i+1);
			stdDevOdr_S4=sqrt(varOdr_S4);

			DataStdDev_S5 = DataStdDev_S5 + pow(Odr_S5 - meanOdr_S5, 2);
			varOdr_S5=DataStdDev_S5/(i+1);
			stdDevOdr_S5=sqrt(varOdr_S5);

			DataStdDev_S6 = DataStdDev_S6 + pow(Odr_S6 - meanOdr_S6, 2);
			varOdr_S6=DataStdDev_S6/(i+1);
			stdDevOdr_S6=sqrt(varOdr_S6);

			DataStdDev_S7 = DataStdDev_S7 + pow(Odr_S7 - meanOdr_S7, 2);
			varOdr_S7=DataStdDev_S7/(i+1);
			stdDevOdr_S7=sqrt(varOdr_S7);

			fsOdour << Odr_S0 << ","
			   	<< Odr_S1 << ","
				<< Odr_S2 << ","
				<< Odr_S3 << ","
				<< Odr_S4 << ","
				<< Odr_S5 << ","
				<< Odr_S6 << ","
				<< Odr_S7 << ","

				<< meanOdr_S0 << ","
				<< meanOdr_S1 << ","
				<< meanOdr_S2 << ","
				<< meanOdr_S3 << ","
				<< meanOdr_S4 << ","
				<< meanOdr_S5 << ","
				<< meanOdr_S6 << ","
				<< meanOdr_S7 << ","

				<< stdDevOdr_S0 << ","
				<< stdDevOdr_S1 << ","
				<< stdDevOdr_S2 << ","
				<< stdDevOdr_S3 << ","
				<< stdDevOdr_S4 << ","
				<< stdDevOdr_S5 << ","
				<< stdDevOdr_S6 << ","
				<< stdDevOdr_S7 << ","

				<< varOdr_S0 << ","
				<< varOdr_S1 << ","
				<< varOdr_S2 << ","
				<< varOdr_S3 << ","
				<< varOdr_S4 << ","
				<< varOdr_S5 << ","
				<< varOdr_S6 << ","
				<< varOdr_S7 << "\n";
			cout << "Recorded "<< i << endl;
			i++;
		}

		usleep(1000*1000);

	}while((~btn_read & stopBtn) || (i==100));
	
	OdourDataTest[0][0]=Odr_S0;
	OdourDataTest[0][1]=Odr_S1;
	OdourDataTest[0][2]=Odr_S2;
	OdourDataTest[0][3]=Odr_S3;
	OdourDataTest[0][4]=Odr_S4;
	OdourDataTest[0][5]=Odr_S5;
	OdourDataTest[0][6]=Odr_S6;
	OdourDataTest[0][7]=Odr_S7;
	
	OdourDataTest[0][8]=meanOdr_S0;
	OdourDataTest[0][9]=meanOdr_S1;
	OdourDataTest[0][10]=meanOdr_S2;
	OdourDataTest[0][11]=meanOdr_S3;
	OdourDataTest[0][12]=meanOdr_S4;
	OdourDataTest[0][13]=meanOdr_S5;
	OdourDataTest[0][14]=meanOdr_S6;
	OdourDataTest[0][15]=meanOdr_S7;
	
	OdourDataTest[0][16]=stdDevOdr_S0;
	OdourDataTest[0][17]=stdDevOdr_S1;
	OdourDataTest[0][18]=stdDevOdr_S2;
	OdourDataTest[0][19]=stdDevOdr_S3;
	OdourDataTest[0][20]=stdDevOdr_S4;
	OdourDataTest[0][21]=stdDevOdr_S5;
	OdourDataTest[0][22]=stdDevOdr_S6;
	OdourDataTest[0][23]=stdDevOdr_S7;
	
	OdourDataTest[0][24]=varOdr_S0;
	OdourDataTest[0][25]=varOdr_S1;
	OdourDataTest[0][26]=varOdr_S2;
	OdourDataTest[0][27]=varOdr_S3;
	OdourDataTest[0][28]=varOdr_S4;
	OdourDataTest[0][29]=varOdr_S5;
	OdourDataTest[0][30]=varOdr_S6;
	OdourDataTest[0][31]=varOdr_S7;

	fsOdour.close();
	cout << "Stop Odour Button Pressed..." << endl;
}

void testOdour_SVM()
{
	Ptr<SVM> svm = StatModel::load<SVM>(filename_SVMOdourModel);
	SVM_Result = "";

	Mat sampleMat(1, 32, CV_32F, OdourDataTest);
	float response = svm->predict(sampleMat);
	if (response == -1)
		SVM_Result = "Ripe";
	else if (response == 1)
		SVM_Result = "Unripe";
	
	realOdour_SVM = SVM_Result;
}

int OdourSubroutine()
{
	cout << "Odour Button Pressed..." << endl;
	gpio1=0x00;	
	writeFPGA_GPIO_1_output(gpio1);
	
	readOdourData();
	testOdour_SVM();

	if(realOdour_SVM == "Ripe") 		{ return 1; }
	if(realOdour_SVM == "Unripe") 		{ return 2; }

	return 0;
}