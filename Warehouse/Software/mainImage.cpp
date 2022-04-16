#include "mainImage.hpp"
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
std::string filename_Image = "/home/root/Project/MainFolder/Main/source.jpg";
std::string filename_SVMModel = "trained-svm.xml";
string real_SVM = "";

int modeImage=1;

Mat frameHSV, frameLAB, frameGRAY, frameBIN, frame_resize, frame, frameBGR;
Scalar meanRGB, meanHSV, meanLAB;

float meanRGB_R, meanRGB_G, meanRGB_B;
float meanLAB_L, meanLAB_A, meanLAB_B;
float meanHSV_H, meanHSV_S, meanHSV_V;

float stddevRGB_R, stddevRGB_G, stddevRGB_B;
float stddevLAB_L, stddevLAB_A, stddevLAB_B;
float stddevHSV_H, stddevHSV_S, stddevHSV_V;

Scalar stddevRGB, stddevHSV, stddevLAB; //0:1st channel, 1:2nd channel and 2:3rd channel
float signal[10][8];
Mat mask, bin, Kmean, HistogramImage;
double maxValB, minValB = 0;;
int maxIndexB, maxIndexG, maxIndexR;
double maxValG, minValG = 0;
double maxValR, minValR = 0;
float trainingData[60][26], testingData[1][26];
int remain_pixel, pixel_object;
String SVM_Result;

//=========================================================
//          GPIO_1
//=========================================================
int button_reading=0x00;
int startBtn_Mask = 0x01;
int stopBtn_Mask = 0x02;
int gpio1_mask=0x00;

//==========================================================
//			Image Task
//==========================================================
int CaptureImage()
{
    Mat src;
    VideoCapture *cap = new VideoCapture(0);
    if (!cap->isOpened()) {
        return -1;
    }

    char str[100];
    static struct timeval last_time;
    struct timeval current_time;
    static float last_fps;
    float t;
    float fps;

    namedWindow("camera");
    
    while (1) {
        if (!cap->read(src))
            return -1;

        //imwrite("source.jpg", src);

        gettimeofday(&current_time, NULL);
        t = (current_time.tv_sec - last_time.tv_sec)
                + (current_time.tv_usec - last_time.tv_usec) / 1000000.;
        fps = 1. / t;
        fps = last_fps * 0.8 + fps * 0.2;
        last_fps = fps;
        last_time = current_time;
        sprintf(str, "%2.2f, %2.6f", fps, t);
        //putText(src, str, Point(20, 40), FONT_HERSHEY_DUPLEX, 1,
                //Scalar(0, 255, 0));
	
	Mat src_resized;
	resize(src,src_resized,Size(300,300));
        imshow("camera", src_resized);
	moveWindow("camera",0,0);

        //-- bail out if escape was pressed
        int c = waitKey(10);
        if ((char) c == 27) {
            break;
        }
	
	button_reading=readFPGA_GPIO_1_input();

	if(button_reading & stopBtn_Mask)
	{
		waitKey(1);
		break;
	}
    }
    imwrite(filename_Image, src);
    delete cap;
    destroyAllWindows();
    
    for (int i=0; i<5; i++)
    {
	waitKey(1);
    }
	
    cout << "Done Capturing..." << endl;
    return(1);
}

void testImage_SVM()
{
	Ptr<SVM> svm = StatModel::load<SVM>(filename_SVMModel);
	SVM_Result = "";

	Mat sampleMat(1, 26, CV_32F, testingData);
	float response = svm->predict(sampleMat);
	if (response == -1)
		SVM_Result = "Ripe";
	else if (response == 1)
		SVM_Result = "Unripe";
}

void rgb_mean(Mat image)
{
	meanStdDev(image, meanRGB, stddevRGB, Mat());
	//cout << "Standard deviation channel R, G, B: " << stddevRGB << endl;
	//cout << "Mean R, G, B: " << meanRGB << endl;
	meanRGB_R = meanRGB.val[0];
	meanRGB_G = meanRGB.val[1];
	meanRGB_B = meanRGB.val[2];

	stddevRGB_R = stddevRGB.val[0];
	stddevRGB_G = stddevRGB.val[1];
	stddevRGB_B = stddevRGB.val[2];
}

void hsv_mean(Mat image)
{
	meanStdDev(image, meanHSV, stddevHSV, Mat());
	//cout << "Standard deviation channel H, S, V: " << stddevHSV << endl;
	//cout << "Mean H, S, V: " << meanHSV << endl;
	meanHSV_H = meanHSV.val[0];
	meanHSV_S = meanHSV.val[1];
	meanHSV_V = meanHSV.val[2];

	stddevHSV_H = stddevHSV.val[0];
	stddevHSV_S = stddevHSV.val[1];
	stddevHSV_V = stddevHSV.val[2];
}

void LAB_mean(Mat image)
{
	meanStdDev(image, meanLAB, stddevLAB, Mat());
	//cout << "Standard deviation channel L, A, B: " << stddevLAB << endl;
	//cout << "Mean L, A, B: " << meanLAB << endl;
	meanLAB_L = meanLAB.val[0];
	meanLAB_A = meanLAB.val[1];
	meanLAB_B = meanLAB.val[2];

	stddevLAB_L = stddevLAB.val[0];
	stddevLAB_A = stddevLAB.val[1];
	stddevLAB_B = stddevLAB.val[2];
}

void convert_RGB2BIN(Mat gray)
{
	// Transform it to binary and invert it. White on black is needed.
	threshold(gray, bin, 40, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

	vector<Point> black_pixels;   // output, locations of non-zero pixels
	findNonZero(bin, black_pixels);

	//cout << "Binary Image all white pixels: " << black_pixels.size() << endl; // amount of black pixels is returned from the size

	// Show binary image
	//imshow("binary", bin);

	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
	// extract only the external blob
	findContours(bin.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	mask = Mat::zeros(bin.size(), CV_8UC1);

	// draw the contours as a solid blob, and create a mask of the cloud
	for (size_t i = 0; i < contours.size(); i++)
		drawContours(mask, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy, 0, Point());

	//imshow("mask", mask);

	vector<Point> all_pixels;   // output, locations of non-zero pixels
	cv::findNonZero(mask, all_pixels);

	//cout << "Mask Image all white pixels: " << all_pixels.size() << endl; // amount of all pixels is returned from the size

	pixel_object = all_pixels.size();
	int white_pixels = all_pixels.size() - black_pixels.size(); // amount of white pixels is returned from the subtraction of all - black ;-)
	remain_pixel = white_pixels;

	//cout << "Binary Image all black pixels: " << white_pixels << endl;

	//waitKey(0);
}

void image_resize(Mat resize_img)
{
	resize(resize_img, frame_resize, Size(), 0.75, 0.75);
	imshow("RGB", resize_img);
	imshow("Resize", frame_resize);
}

void K_mean_clustering(Mat src)
{
	
	Mat samples(src.rows * src.cols, 3, CV_32F);
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			for (int z = 0; z < 3; z++)
				samples.at<float>(y + x * src.rows, z) = src.at<Vec3b>(y, x)[z];


	int clusterCount = 2;
	Mat labels;
	int attempts = 5;
	Mat centers;
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);


	Mat new_image(src.size(), src.type());
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x * src.rows, 0);
			new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
			new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
			new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
		}
	//imshow("clustered image", new_image);
	Kmean = new_image;
	
}

void histogram_RGB(Mat src)
{
	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	maxValB = 0;
	minValB = 0;
	minMaxLoc(b_hist, NULL, &maxValB, NULL, NULL);
	maxValG = 0;
	minValG = 0;
	minMaxLoc(g_hist, NULL, &maxValG, NULL, NULL);
	maxValR = 0;
	minValR = 0;
	minMaxLoc(r_hist, NULL, &maxValR, NULL, NULL);

	//cout << maxValB << " " << maxValG << " " << maxValR << endl;
	

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	
	maxIndexR = 0;
	maxIndexG = 0;
	maxIndexB = 0;
	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		//cout << "b" << bin_w * (i-1) << "," << hist_h - cvRound(b_hist.at<float>(i-1)) << endl;
		//cout << bin_w * (i) << ","  << hist_h - cvRound(b_hist.at<float>(i)) << endl;

		if (maxIndexB == 0 && hist_h - cvRound(b_hist.at<float>(i-1)) != hist_h - cvRound(b_hist.at<float>(i)))
		{
			maxIndexB = bin_w * (i - 1);
		}

		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);

		//cout << "g" << bin_w * (i - 1) << "," << hist_h - cvRound(g_hist.at<float>(i - 1)) << endl;
		//cout << bin_w * (i) << "," << hist_h - cvRound(g_hist.at<float>(i)) << endl;
		
		if (maxIndexG == 0 && hist_h - cvRound(g_hist.at<float>(i-1)) != hist_h - cvRound(g_hist.at<float>(i)))
		{
			maxIndexG = bin_w * (i - 1);
		}

		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);

		//cout << "r" << bin_w * (i - 1) << "," << hist_h - cvRound(r_hist.at<float>(i - 1)) << endl;
		//cout << bin_w * (i) << "," << hist_h - cvRound(r_hist.at<float>(i)) << endl;

		if (maxIndexR == 0 && hist_h - cvRound(r_hist.at<float>(i-1)) != hist_h - cvRound(r_hist.at<float>(i)))
		{
			maxIndexR = bin_w * (i - 1);
		}
	}

	std::cout << maxIndexB << " " << maxIndexG << " " << maxIndexR << endl;

	HistogramImage = histImage;
	/// Display
	//namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	//imshow("calcHist Demo", histImage);
}

void test_image()
{
	char filename[1000];
	sprintf(filename, "/home/root/Project/MainFolder/Main/source.jpg");
	IplImage* img = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
	if (!img)
	{
		cout << " -> Error: Image not found." << endl;
	}
	Mat frame = imread(filename);

	cvtColor(frame, frameBGR, COLOR_RGB2BGR);
	char filename_writeBGR[1000];
	sprintf(filename_writeBGR, "/home/root/Project/MainFolder/Main/BGR.jpg");
	imwrite(filename_writeBGR, frameBGR);

	cvtColor(frame, frameHSV, COLOR_RGB2HSV);
	char filename_writeHSV[1000];
	sprintf(filename_writeHSV, "/home/root/Project/MainFolder/Main/HSV.jpg");
	imwrite(filename_writeHSV, frameHSV);

	cvtColor(frame, frameLAB, COLOR_RGB2Lab);
	char filename_writeLab[1000];
	sprintf(filename_writeLab, "/home/root/Project/MainFolder/Main/Lab.jpg");
	imwrite(filename_writeLab, frameLAB);

	cvtColor(frame, frameGRAY, COLOR_RGB2GRAY);
	char filename_writeGray[1000];
	sprintf(filename_writeGray, "/home/root/Project/MainFolder/Main/Gray.jpg");
	imwrite(filename_writeGray, frameGRAY);

	convert_RGB2BIN(frameGRAY);
	char filename_writeMask1[1000];
	sprintf(filename_writeMask1, "/home/root/Project/MainFolder/Main/Mask1.jpg");
	imwrite(filename_writeMask1, bin);

	char filename_writeMask2[1000];
	sprintf(filename_writeMask2, "/home/root/Project/MainFolder/Main/Mask2.jpg");
	imwrite(filename_writeMask2, mask);
	
	K_mean_clustering(frame);
	char filename_writeKMean[1000];
	sprintf(filename_writeKMean, "/home/root/Project/MainFolder/Main/KMean.jpg");
	imwrite(filename_writeKMean, Kmean);
	
	rgb_mean(frame);
	hsv_mean(frameHSV);
	LAB_mean(frameLAB);

	histogram_RGB(Kmean);
	char filename_Histo[1000];
	sprintf(filename_Histo, "/home/root/Project/MainFolder/Main/Histo.jpg");
	imwrite(filename_Histo, HistogramImage);
	
	testingData[0][0] = meanRGB_R;
	testingData[0][1] = meanRGB_G;
	testingData[0][2] = meanRGB_B;
	testingData[0][3] = stddevRGB_R;
	testingData[0][4] = stddevRGB_G;
	testingData[0][5] = stddevRGB_B;

	testingData[0][6] = meanHSV_H;
	testingData[0][7] = meanHSV_S;
	testingData[0][8] = meanHSV_V;
	testingData[0][9] = stddevHSV_H;
	testingData[0][10] = stddevHSV_S;
	testingData[0][11] = stddevHSV_V;

	testingData[0][12] = meanLAB_L;
	testingData[0][13] = meanLAB_A;
	testingData[0][14] = meanLAB_B;
	testingData[0][15] = stddevLAB_L;
	testingData[0][16] = stddevLAB_A;
	testingData[0][17] = stddevLAB_B;

	testingData[0][18] = (float)maxValR;
	testingData[0][19] = (float)maxValG;
	testingData[0][20] = (float)maxValB;

	testingData[0][21] = (float)pixel_object;
	testingData[0][22] = (float)remain_pixel;
	
	testingData[0][23] = (float)maxIndexR;
	testingData[0][24] = (float)maxIndexG;
	testingData[0][25] = (float)maxIndexB;
	
	Mat src_resized;
	
	resize(frame,src_resized,Size(250,250));
        imshow("Original", src_resized);
	moveWindow("Original",0,0);
	
	resize(frameHSV,src_resized,Size(250,250));
        imshow("HSV", src_resized);
	moveWindow("HSV",250,0);
	
	resize(frameLAB,src_resized,Size(250,250));
        imshow("LAB", src_resized);
	moveWindow("LAB",500,0);
	
	resize(frameGRAY,src_resized,Size(250,250));
        imshow("GRAY", src_resized);
	moveWindow("GRAY",0,250);
	
	resize(Kmean,src_resized,Size(250,250));
        imshow("Kmean", src_resized);
	moveWindow("Kmean",250,250);
	
	resize(HistogramImage,src_resized,Size(250,250));
        imshow("HISTO", src_resized);
	moveWindow("HISTO",500,250);
	
	cout << "Press ESC to quit view processed image..." << endl;
        while(1) {
		int c = waitKey(10);
	        if ((char) c == 27) {
	            break;
        	}
		
		button_reading=readFPGA_GPIO_1_input();

		if(button_reading & stopBtn_Mask)
		{
			waitKey(1);
			break;
		}
	}
	destroyAllWindows();
    
        for (int i=0; i<100; i++)
        {
		waitKey(1);
        }

	cout << "Start Test SVM " << endl;
	testImage_SVM();
	cout << "SVM Result : " << SVM_Result << endl;
	cout << "Done Test SVM" << endl;
}

int ImageSubroutine()
{
	cout << "Image Button Pressed..." << endl;
	gpio1_mask=0x00;	
	writeFPGA_GPIO_1_output(gpio1_mask);
	
	if(modeImage==1)
	{
		if(CaptureImage()!=1)	{ 
			cout << "Error in Image" << endl; 
		};
		
		test_image();
		if(real_SVM == "Ripe") 		{ return(1); }
		if(real_SVM == "Unripe") 	{ return(2); }
	}
	
	if(modeImage ==0)
	{
		train_image();
		return(0);
	}
	return(0);
}