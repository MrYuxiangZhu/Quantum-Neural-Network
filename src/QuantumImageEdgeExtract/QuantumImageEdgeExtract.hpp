/********************************************************************************
****文件名：Sobel头文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/

#ifndef __QUANTUM_IMAGE_EDGE_EXTRACT_HPP
#define __QUANTUM_IMAGE_EDGE_EXTRACT_HPP
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
#include "../src/QuantumImagePreparation/QuantumImagePreparation.hpp"

using namespace std;
using namespace cv;

/********************************************************************************
*******量子图像边缘横向提取子模块
********************************************************************************/
struct Landscape_Edge_Extract_accumulate_block 
{
public:
	char QuantumState[2] = {'0','1'};			//量子基态
	template<typename Iterator>
	void operator()(const vector<vector<unsigned char>>& QuantumImage, Iterator first, Iterator last, Iterator colums, vector<vector<unsigned char>>& extract);//量子图像边缘检测并行计算子单元
};

/********************************************************************************
*******量子图像边缘纵向向提取子模块
********************************************************************************/
struct Portrait_Edge_Extract_accumulate_block : public Landscape_Edge_Extract_accumulate_block 
{
public:
	template<typename Iterator>
	void operator()(const vector<vector<unsigned char>>& QuantumImage, Iterator first, Iterator last, Iterator colums, vector<vector<unsigned char>>& extract);//量子图像边缘检测并行计算子单元
};

/********************************************************************************
*******量子图像边缘提取类
********************************************************************************/
class Quantum_Image_Edge_Extract : public Portrait_Edge_Extract_accumulate_block, public HardWare 
{
private:
	int Height;													//经典图像高度
	int Width;													//经典图像宽度
	vector<int> ClassicImageXaxis;								//经典图像X轴
	vector<int> ClassicImageYaxis;								//经典图像Y轴
	vector<vector<unsigned char>> QuantumImageSrc;				//量子原图像，量子测量后的图像
	vector<vector<unsigned char>> ExtractedImage;				//提取后的量子边缘图像
	cv::Mat EdgeImage;											//边缘图像
	unsigned int hardware_threads;								//CPU线程数
public:
	Quantum_Image_Edge_Extract(void);
	~Quantum_Image_Edge_Extract(void);
	void Quantum_Image_Edge_Extract_Init(cv::Mat& srcimg);			//初始化
	void ConvertArray(cv::Mat& srcimg);								//转换成矩阵
	template<typename Iterator>
	void Landscape_Edge_Extract_parallel_accumulate(const vector<vector<unsigned char>>& QuantumImageSrc, Iterator first, Iterator last, Iterator colums, bool Debug);//量子图像边缘横向检测多线程并行计算
	template<typename Iterator>
	void Portrait_Edge_Extract_parallel_accumulate(const vector<vector<unsigned char>>& QuantumImageSrc, Iterator first, Iterator last, Iterator colums, bool Debug);//量子图像边缘纵向检测多线程并行计算
	void Image_Edge_Extract(cv::Mat& srcimg, cv::Mat& edge, bool Debug);		//提取过程
	cv::Mat Get_Edges(void);													//获取边缘图像
	NEQR Get_Qedge(void);														//获取量子边缘图像
	void Show_Edge(void);														//显示边缘
	void Show_Edge(string name);												//显示边缘
	void Reset_Edge(void);														//清空边缘图像
	void Delete();																//删除容器
};
#endif