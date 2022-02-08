/********************************************************************************
****文件名：VIBE头文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/12/20
*********************************************************************************/
#ifndef __QUANTUMVIBE_HPP
#define __QUANTUMVIBE_HPP

#include <iostream>
#include <cstdio>
#include <cmath>
#include <thread>
#include <numeric>
#include <algorithm>
#include <functional>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../src/Camera/camera.hpp"
#include "../src/QuantumCircuit/QuantumCircuit.hpp"
#include "../src/QuantumImagePreparation/QuantumImagePreparation.hpp"

using namespace cv;
using namespace std;

// 每个像素点的样本个数默认值
// the Default Number of pixel's samples
#define DEFAULT_NUM_SAMPLES  20

// #min指数默认值
// the Default Match Number of make pixel as Background
#define DEFAULT_MIN_MATCHES  2

// Sqthere半径默认值
// the Default Radius of pixel value
#define DEFAULT_RADIUS 20

// 子采样概率默认值
// the Default the probability of random sample
#define DEFAULT_RANDOM_SAMPLE 16

// 前景检测次数
// the times of foreground detection
#define DEFAULT_FOREGROUND_TIME 50

struct Process_Frames_accumulate_block 
{
public:
	Process_Frames_accumulate_block();
	template<typename Iterator>
    void operator()(const Mat& img, Iterator first, Iterator last, Iterator colums, vector<vector<uchar>>& FGModel, vector<vector<vector<uchar>>>& result); 
	 // x的邻居点
    // x's neighborhood points
    int c_xoff[9];

    // y的邻居点
    // y's neighborhood points
    int c_yoff[9];
	
    // 每个像素点的样本个数
    // Number of pixel's samples
    int num_samples;

    // #min指数
    // Match Number of make pixel as Background
    int num_min_matches;

    // Sqthere半径
    // Radius of pixel value
    int radius;

    // 子采样概率
    // the probability of random sample
    int random_sample;
	
	// 前景检测次数
    // the times of foreground detection
	int fgtimes;
	
	//量子基态
	char QuantumState[2]={'0','1'};			
};

class QViBe : public Process_Frames_accumulate_block, public HardWare
{
private:
	int Height;													//图像高度
	int Width;													//图像宽度
	vector<int> ClassicImageXaxis;								//经典图像X轴
	vector<int> ClassicImageYaxis;								//经典图像Y轴
    // 样本库
    // Sample Library, size = img.rows * img.cols *  DEFAULT_NUM_SAMPLES
    //unsigned char ***samples;
	vector<vector<vector<unsigned char> > >samples;
	vector<vector<unsigned char> >fgmodel;
	// 前景模型二值图像
    // Foreground Model Binary Image
    cv::Mat FGModel;
	//CPU线程数
	unsigned long hardware_threads;
    //量子测量
    Quantum_Measure measure;
public:
    QViBe(int num_sam = DEFAULT_NUM_SAMPLES,
         int min_match = DEFAULT_MIN_MATCHES,
         int r = DEFAULT_RADIUS,
         int rand_sam = DEFAULT_RANDOM_SAMPLE,
		 int times = DEFAULT_FOREGROUND_TIME);
    ~QViBe(void);
	void size(NEQR &QuantumImage);								//获取量子图像大小
    // 背景模型初始化
    // Init Background Model.
    void Init(NEQR& qimg);
    // 处理第一帧量子图像
    // Process First Frame of Video Query
    void ProcessFirstFrame(NEQR& qimg);
    // 运行 ViBe 算法，提取前景区域并更新背景模型样本库
    // Run the ViBe Algorithm: Extract Foreground Areas & Update Background Model Sample Library.
    void Run(NEQR& qimg, bool Debug);
	//显示samples
	void Show();
    // 获取前景模型二值图像
    // get Foreground Model Binary Image.
    cv::Mat Get_FGModel(void);
    // 删除样本库
    // Delete Sample Library.
    void DeleteSamples();
	template<typename Iterator>
	void Process_Frames_parallel_accumulate(const Mat& img, Iterator first, Iterator last,Iterator colums, bool Debug);
};
#endif

