#include <unistd.h> 
#include "opencv/cv.h"

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <time.h>

using namespace cv;
using namespace std;

int N = 10;//颜色差值
int R, G, B;
int R0,G0,B0;
int width,height;
Point posXY = Point(0,0);


//计算两张图像复制位置
Point copyPosion(int x0,int y0,int x1,int y1,int pos){
	int y = y1-y0;
	int x = x1-x0;
	printf("\n\r位置参数==================================%d\n\r",pos);
	switch (pos)
	{
		case 1:
			//水平居中，垂直居中
			posXY.x = (int)(x/2);
			posXY.y = (int)(y/2);
			break;
		case 2:
			//水平居中 （靠上）
			posXY.x = (int)(x/2);
			posXY.y = 0;
			break;
		case 3:
			//水平居中 （靠下）
			posXY.x = (int)(x/2);
			posXY.y = y;
			break;
		case 4:
			//垂直居中 (靠左)
			posXY.x = 0;
			posXY.y = (int)(y/2);
			break;
		case 5:
			//垂直居中 (靠右)
			posXY.x = x;
			posXY.y = (int)(y/2);
			break;
		case 6:
			//左上角
			posXY.x = 0;
			posXY.y = 0;
			break;
		case 7:
			//右上角
			posXY.x = x;
			posXY.y = 0;
			break;
		case 8:
			//左下角
			posXY.x = 0;
			posXY.y = y;
			break;

		case 9:
			//右下角
			posXY.x = x;
			posXY.y = y;
			break;
		default:
			//默认居中
			posXY.x = (int)(x/2);
			posXY.y = (int)(y/2);
	}
	return posXY;
}

bool isDiffColor(int R,int G,int B){
	if(abs(R0-R)>=N || abs(G0-G)>=N || abs(B0-B)>=N){
		return true;
	}
	return false;
}

int topPoint(Mat &img){

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			R = (int)img.at<Vec3b>(i,j)[0];
			G = (int)img.at<Vec3b>(i,j)[1];
			B = (int)img.at<Vec3b>(i,j)[2];
			if(isDiffColor(R,G,B)){
				return i;
			}
		}
	}
	return 0;
}

int leftPoint(Mat &img){
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			R = (int)img.at<Vec3b>(j,i)[0];
			G = (int)img.at<Vec3b>(j,i)[1];
			B = (int)img.at<Vec3b>(j,i)[2];
			if(isDiffColor(R,G,B)){
				return i;
			}
		}
	}
	return 0;
}

int rightPoint(Mat &img){

	for (int i = width-1; i >= 0; i--)
	{
		for (int j = 0; j < height; j++)
		{
			R = (int)img.at<Vec3b>(j,i)[0];
			G = (int)img.at<Vec3b>(j,i)[1];
			B = (int)img.at<Vec3b>(j,i)[2];
			if(isDiffColor(R,G,B)){
				return i;
			}
		}
	}

	return width;
}

int bottomPoint(Mat &img){

	for (int i = height-1; i >= 0; i--)
	{
		for (int j = 0; j < width; j++)
		{
			R = (int)img.at<Vec3b>(i,j)[0];
			G = (int)img.at<Vec3b>(i,j)[1];
			B = (int)img.at<Vec3b>(i,j)[2];
			if(isDiffColor(R,G,B)){
				return i;
			}
		}
	}
	return height;
}

#define show_debug(tip,message) if(debug){ cout << tip << message << endl;}

void show_help() {
	//// -s /home/img/img.jpg -d /home/img/img2.jpg -h 500 -w 500 -m 10 -p 1;
    cout << "Usage: coreimgclip [options] [-s] <source_file> [--] [args...]" << endl;
	cout << "-s<path>	the path of source file" << endl;
	cout << "-d<path>	the path of destination file" << endl;
	cout << "-w<int>	the width of destination file. default value is 300" << endl;
	cout << "-h<int>	the height of destination file. default value is 200" << endl;
	cout << "-m<int>	the margin of the file. default value is 10 (px)" << endl;
	cout << "-p<int>	the position of the file.1-center & middle,2-left" << endl;	//TODO p explain
	cout << "-v<int>	the different color value" << endl;

} 


int main(int argc, char** argv)
{  
	//计算程序运行时间
	clock_t start,finish;
    double totaltime;
    start=clock();
    /////////////
    
	if (argc == 1) {
		show_help();
		return 1;
	}

	//初始化参数
	string sourcePath = ""; // 原图地址 用户输入
	string destPath = "";  //图片保存地址 用户输入
	int newWidth    = 300; //生成图的宽度 用户输入
	int newHeight   = 200; //生成图的高度 用户输入
	int pos 		= 1;   //1-居中 图片叠加位置 用户输入 
	int margin      = 10;  //图片边距 用户输入
	N = 10; 
	posXY = Point(margin,margin);//剪切位置

	//获取用户输入参数
	int param;
	while( (param = getopt(argc, argv, "H:s:d:w:h:p:m:v:")) != -1 )
	{
		if ( param == 's' ) {
			sourcePath = optarg;
		}else if ( param == 'd' ) {
			destPath = optarg;
		}else if ( param == 'w' ) {
			sscanf (optarg, "%d", &newWidth);
		}else if ( param == 'h' ) {
			sscanf (optarg, "%d", &newHeight);
		}else if ( param == 'p' ) {
			sscanf (optarg, "%d", &pos);
		}else if ( param == 'm' ) {
			sscanf (optarg, "%d", &margin);
		}else if ( param == 'v' ) {
			sscanf (optarg, "%d", &N);
		}
	}
	printf("用户输入参数:%d，%d,%d,%d \n\r",newWidth,newHeight,pos,margin);
	printf("\n\r==================================\n\r");

	if ( sourcePath == "" ||  destPath == "" ) {
		show_help();
		return 1;
	}
	
	Mat img = imread(sourcePath,-1);
	//如果读入图像失败
	if( !img.data ){
		printf("[error] do not load pic \n");
		return -1;
	}
	
	//左上角像素作为底图的颜色
    R0 = (int)img.at<Vec3b>(0,0)[0];
    G0 = (int)img.at<Vec3b>(0,0)[1];
    B0 = (int)img.at<Vec3b>(0,0)[2];
    //原图的宽高
    width  = img.cols;
    height = img.rows;
    
    printf("底图RGB：%d,%d,%d\n\r",R0,G0,B0);
    printf("\n\r==================================%d\n\r",1);
    
    //获取核心图坐标数据
    int top = topPoint(img);
    int left = leftPoint(img);
    int right = rightPoint(img);
    int bottom = bottomPoint(img);
    printf("核心图坐标：%d，%d,%d,%d \n\r",top,left,right,bottom);
    printf("\n\r==================================%d\n\r",2);

    //计算核心图片的宽高
    int coreWidth = right-left;
    int coreHeight = bottom-top;
    
    printf("核心图片的宽高：%d，%d \n\r",coreWidth,coreHeight);
    printf("\n\r==================================%d\n\r",3);

    //计算可用宽高
    int activeWidth = newWidth-2*margin;
    int activeHeight = newHeight-2*margin;

    printf("可用宽高：%d，%d \n\r",activeWidth,activeHeight);
    printf("\n\r==================================%d\n\r",4);

    //缩放后的核心图片宽和高
    int resizeCoreWidth,resizeCoreHeight;

    //计算实际图像大小
    if(coreHeight>coreWidth){
    	//如果高大于宽
    	resizeCoreHeight = activeHeight;
    	resizeCoreWidth = resizeCoreHeight * coreWidth / coreHeight;
    	//如果比例算出来的宽比可用宽度大,则以宽度为准
    	if(resizeCoreWidth>activeWidth){
    		resizeCoreWidth = activeWidth;
    		resizeCoreHeight = resizeCoreWidth * coreHeight / coreWidth;
    	}
    }else{
    	//同理
    	resizeCoreWidth = activeWidth;
    	resizeCoreHeight = resizeCoreWidth * coreHeight / coreWidth;
    	if(resizeCoreHeight>activeHeight){
    		resizeCoreHeight = activeHeight;
    		resizeCoreWidth = resizeCoreHeight * coreWidth / coreHeight;
    	}
    }

    printf("缩放后的宽高：%d，%d \n\r",resizeCoreWidth,resizeCoreHeight);
    printf("\n\r==================================%d\n\r",5);

    //截取核心图 共用img内存
    Mat coreImg = img(Rect(left,top,coreWidth,coreHeight));

    namedWindow("Core image", CV_WINDOW_AUTOSIZE);
    imshow("Core image",coreImg);
    
    //缩放图片
    Mat resizeImg(newWidth,newHeight,CV_8UC4,cv::Scalar(R0,G0,B0));
    resize(coreImg,resizeImg,Size(resizeCoreWidth,resizeCoreHeight),0,0,INTER_LINEAR);

    //计算合成图片的坐标
    copyPosion(resizeCoreWidth,resizeCoreHeight,activeWidth,activeHeight,pos);

    printf("合成坐标：%d，%d \n\r",posXY.x,posXY.y);
    printf("\n\r==================================%d\n\r",6);

    printf("参数：%d，%d,%d,%d,%d,%d\n\r",posXY.x,posXY.y,newWidth,newHeight,resizeCoreWidth,resizeCoreHeight);
    printf("\n\r==================================%d\n\r",7);
    //合成图片
    Mat outImg(newHeight,newWidth,resizeImg.type(),cv::Scalar(R0,G0,B0));
    resizeImg.copyTo(outImg(cv::Rect((posXY.x+margin),(posXY.y+margin),resizeCoreWidth,resizeCoreHeight)));
   
    namedWindow("image2", CV_WINDOW_AUTOSIZE);
    imshow("image2",outImg);

    //保存图像
    imwrite(destPath, outImg);
    
    //释放资源
	img.release();
	coreImg.release();
	outImg.release();
	cout << img << endl;
	cout << coreImg << endl;
	cout << outImg << endl;
	
	//输出运行时间
	finish=clock();
	totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
	cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<endl;
	waitKey(0);
    return 0;  
}   


