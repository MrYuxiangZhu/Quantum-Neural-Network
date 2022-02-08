/********************************************************************************
****文件名：NEQR量子图像构建头文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/11/11
*********************************************************************************/
#ifndef __QUANTUMIMAGEPREPARATION_HPP
#define __QUANTUMIMAGEPREPARATION_HPP

#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <thread>
#include <numeric>
#include <algorithm>
#include <functional>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "../src/Camera/camera.hpp"

using namespace std;
using namespace cv;

#define THREAD_TIME 1
#define Action_Image 500
#define Action_Type 10


/********************************************************************************
*******硬件类
********************************************************************************/
class HardWare 
{
private:
	unsigned int hardware_threads;								//CPU线程数
public:
	HardWare();
	virtual ~HardWare();
	int Obtain_Thread_Num(void);								//获取CPU线程个数
};

/********************************************************************************
*******NEQR量子图像构建子模块
********************************************************************************/
struct NEQR_Image_Constructor_accumulate_block 
{
public:
	template<typename Iterator>
	void operator()(const cv::Mat& ClassicImage, Iterator first, Iterator last, Iterator colums, vector<vector<vector<char>>>& result);//NEQR量子图像构建子单元
	char QuantumState[2]={'0','1'};			//量子基态
};

/********************************************************************************
*******NEQR量子图像数据
********************************************************************************/
class NEQR : public HardWare, public NEQR_Image_Constructor_accumulate_block 
{
private:
    int QuantumNumTotal;										//总量子数量
	int QuantumNumX;											//X轴量子数量
	int QuantumNumY;											//Y轴量子数量
	int Height;													//图像高度
	int Width;													//图像宽度
	bool Debug;													//是否调试
	unsigned int hardware_threads;								//CPU线程数
	vector<vector<vector<char>>> QuantumImage;					//存储量子图像
	vector<vector<char>> QuantumX;								//量子图像X轴
	vector<vector<char>> QuantumY;								//量子图像Y轴
	vector<int> ClassicImageXaxis;								//经典图像X轴
	vector<int> ClassicImageYaxis;								//经典图像Y轴
	cv::Size dsize = Size(512, 512);							//重设图像大小
public:
    NEQR();
	NEQR(
		int total, 
		int numx, 
		int numy, 
		int height,
		int width, 
		unsigned int hardware, 
		vector<vector<vector<char>>>quantumimage, 
		vector<vector<char>>quantumx,
		vector<vector<char>>quantumy, 
		vector<int>classicxaxis, 
		vector<int>classicyaxis):
		QuantumNumTotal(total),
		QuantumNumX(numx),
		QuantumNumY(numy),
		Height(height),
		Width(width),
		hardware_threads(hardware),
		QuantumImage(quantumimage),
		QuantumX(quantumx),
		QuantumY(quantumy),
		ClassicImageXaxis(classicxaxis),
		ClassicImageYaxis(classicyaxis)
		{}	//构建NEQR图像
	~NEQR();
	void Image_Measure(const cv::Mat& ClassicImage);			//测量经典图像
	cv::Mat Resize_Classic_Image(cv::Mat& ClassicImage, int code, bool Debug);//重设图像大小
	cv::Mat Resize_Classic_Image(cv::Mat& ClassicImage, int Height, int Width, int code, bool Debug);//重设图像大小
	bool Process_First_Frame(bool First);							//第一帧判断
	void Quantum_Image_Init(const cv::Mat& ClassicImage, bool Debug);//初始化NEQR量子图像	
	template<typename Iterator>
	void NEQR_Image_Constructor_parallel_accumulate(const cv::Mat& ClassicImage, Iterator first, Iterator last, Iterator colums, bool Debug);//构建NEQR图像多线程并行计算
	void Constructor(const cv::Mat& ClassicImage, bool Debug);	//构建NEQR图像
    void Show_QuantumImage(void);								//显示NEQR图像
	void Show_QuantumImage(string name);						//显示NEQR图像
	void Show_ClassicImage(const cv::Mat& ClassicImage);		//显示经典图像
	void Show_ClassicImage(const cv::Mat& ClassicImage, string name);//显示经典图像
	vector<vector<char>> Get_Xaxis_Position()	{ return QuantumX; };	//获取X轴坐标
	vector<vector<char>> Get_Yaxis_Position()	{ return QuantumY; };	//获取Y轴坐标
	vector<int> Get_Classic_Xaxis_Position()	{ return ClassicImageXaxis; };	//获取X轴坐标
	vector<int> Get_Classic_Yaxis_Position()	{ return ClassicImageYaxis; };	//获取Y轴坐标
	vector<vector<vector<char>>> Get_Quantum_Image_Pixel(void)	{ return QuantumImage; };//获取NEQR量子图像像素
	void Set_Quantum_Image_Pixel(vector<vector<vector<char>>>& Pixel) { QuantumImage = Pixel; };//设置NEQR量子图像像素
	vector<int> size(void);								//获取量子图像大小
	int Get_Image_Height(void)		{ return Height; };					//获得图像高度
	int Get_Image_Width(void)		{ return Width; };					//获得图像高度
	int Get_QuantumNumX(void)		{ return QuantumNumX; };				//获得QuantumNumX
	int Get_QuantumNumY(void)		{ return QuantumNumY; };				//获得QuantumNumY
	int Get_QuantumNumTotal(void)	{ return QuantumNumX + QuantumNumY + 8; };//获得QuantumNumTotal
	NEQR Clone(void);											//量子图像克隆
	NEQR Zeros(void);											//空白量子图像
	NEQR Threshold(char value);									//阈值量子图像
	void Delete_QuantumImages();								//删除NEQR量子图像
};
/********************************************************************************
*******量子测量子模块
********************************************************************************/
struct Quantum_Measure_accumulate_block 
{
public:
	template<typename Iterator>
	void operator()(const vector<vector<vector<char>>>& QuantumImage, Iterator first, Iterator last, Iterator colums, vector<vector<unsigned char>>& ClassicImageArray);
	char QuantumState[2] = { '0', '1' };			//量子基态
};

/********************************************************************************
*******量子测量
********************************************************************************/
class Quantum_Measure: public HardWare, public Quantum_Measure_accumulate_block 
{
private:
	int Height;													//经典图像高度
	int Width;													//经典图像宽度
	int QuantumNumTotal;										//总量子数量
	int QuantumNumX;											//X轴量子数量
	int QuantumNumY;											//Y轴量子数量
	vector<int> ClassicImageXaxis;								//经典图像X轴
	vector<int> ClassicImageYaxis;								//经典图像Y轴
	vector<vector<unsigned char>>ClassicImageArray;				//经典图像二维数组
	vector<vector<vector<char>>>QuantumImages;					//量子图像
	vector<vector<char>> QuantumOverFlow;						//溢出位
	cv::Mat CIReconstructor;									//重建的经典图像
	bool Debug;													//是否调试
	unsigned int hardware_threads;								//CPU线程数
public:
	Quantum_Measure();
	~Quantum_Measure();
	void Init(NEQR& QuantumImage);								//初始化
	template<typename Iterator>
	void Quantum_Measure_parallel_accumulate(NEQR& neqr, Iterator first, Iterator last, Iterator colums, bool Debug);//量子测量多线程并行计算
	void Measure(NEQR &neqr, bool Debug);											//测量图像
	vector<vector<unsigned char>> Get_Classic_Image_Array(void) { return ClassicImageArray; };//取重建后的经典图像数组
	void size(NEQR &QuantumImage);								//获取量子图像大小
	void Show_ClassicImage(void);								//显示经典图像
	void Show_ClassicImage(string name);						//显示经典图像
	cv::Mat Get_Classic_Image_Reconstructor(void);				//获取重建后的经典图像
	void Delete_QuantumImages();								//删除内存
};
const float PI = 3.141592;	//圆周率
const float PA = 3.141592/2;	//角度

/********************************************************************************
*******FRQI量子图像构建子模块
********************************************************************************/
struct FRQI_Image_Constructor_accumulate_block 
{
public:
	template<typename Iterator>
	void operator()(const vector<vector<float>>& Parameter, Iterator first, Iterator last, Iterator colums, vector<vector<float>>& result);//FRQI量子图像构建子单元
	char QuantumState[2] = {'0','1'};			//量子基态
};
/********************************************************************************
*******FRQI量子图像数据
********************************************************************************/
class FRQI : public HardWare, public FRQI_Image_Constructor_accumulate_block 
{
private:
	int QuantumNumTotal;						//总量子数量
	int QuantumNumX;							//X轴量子数量
	int QuantumNumY;							//Y轴量子数量
	int Height;									//图像高度
	int Width;									//图像宽度
	bool Debug;									//是否调试
	float param = 1 / 2;						//系数
	unsigned int hardware_threads;				//CPU线程数
	vector<vector<float>> QuantumImage;			//存储量子图像
	vector<vector<char>> QuantumX;				//量子图像X轴
	vector<vector<char>> QuantumY;				//量子图像Y轴
	vector<int> ClassicImageXaxis;				//经典图像X轴
	vector<int> ClassicImageYaxis;				//经典图像Y轴
public:
	FRQI();
	~FRQI();
	void Image_Measure(vector<vector<float>>& Parameter, bool Debug);	//测量图像系数
	bool Process_First_Frame(bool First);							//第一帧判断
	template<typename Iterator>
	void FRQI_Image_Constructor_parallel_accumulate(const vector<vector<float>>& Parameter, Iterator first, Iterator last,Iterator colums, bool Debug);//构建FRQI图像多线程并行计算
	void Constructor(vector<vector<float> >& Parameter, bool Debug);//构建FRQI图像
	void Show_QuantumImage(void);				//显示FRQI图像
	void Show_QuantumImage(string name);		//显示FRQI图像
	vector<int> size(void);						//获取量子图像大小（经典图像长和宽）
	void Delete_QuantumImages();				//删除FRQI量子图像
};

int NEQR_Save(string filename, string video);  //保存NEQR图像数据
int NEQR_Read(string filename, vector<NEQR>& neqr_data);	//读取NEQR数据

#endif