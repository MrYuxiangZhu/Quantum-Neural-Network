/********************************************************************************
****文件名：特征提取头文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/
#ifndef __QUANTUMCONVOLUTION_HPP
#define __QUANTUMCONVOLUTION_HPP
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include "../QuantumImagePreparation/QuantumImagePreparation.hpp"
#include "../QuantumImageFeatureExtract/QuantumImageFeatureExtract.hpp"

using namespace std;
using namespace cv;
/********************************************************************************
*******量子图像边缘横向提取子模块
********************************************************************************/
struct Convolution_accumulate_block {
public:
	template<typename Iterator>
	void operator()(vector<vector<float> >& QuantumImage, Iterator first, Iterator last, Iterator colums, int thread_num, int WindowsHeight, int WindowsWidth, vector<vector<float> >& feature);//卷积提取并行计算子单元
};
class Convolution_Operator:public Convolution_accumulate_block, public HardWare{
private:
    int FeatureHeight;						//特征图像高度
	int FeatureWidth;						//特征图像宽度
    bool Debug;								//是否调试
    int WindowsHeight;                      //设置模板窗口高度
    int WindowsWidth;                       //设置模板窗口宽度
    int SrcHeight;                          //原图像高低
    int SrcWidth;                           //原图像宽度
	vector<int>ClassicImageXaxis;			//经典图像X轴
	vector<int>ClassicImageYaxis;			//经典图像Y轴
	vector<vector<float> > QuantumImageSrc;	//提取后的值
	vector<vector<float> > FeatureImage;	//特征图像
	unsigned int hardware_threads;								//CPU线程数
public:
    Convolution_Operator();
    ~Convolution_Operator();
    void Init(vector<vector<float> >& QuantumImage, int winheight, int winwidth);  //提取特征图像
	void ConvertArray(vector<vector<float> >& QuantumImage);					//转换成矩阵
	template<typename Iterator>
	void Convolution_accumulate(Iterator first, Iterator last, Iterator colums, bool Debug);//卷积提取多线程并行计算
    void Run(vector<vector<float> >& QuantumImage, vector<vector<float> >& feature, bool Debug);	//运行特征提取
	void Run(vector<vector<float> >& QuantumImage, vector<float>& feature, bool Debug);	//运行特征提取
	void SetWindowsSize(int height, int width);   //设置窗口大小
	vector<vector<float> > Get_Feature(void){ return FeatureImage; };//获取特征图像
	vector<float> Get_Dimension_Feature(void);		//获取特征图像
    void ShowFeatureImage(void);            //显示特征图像
	void ShowFeatureImage(string name);
	void Reset_Feature(void);				//清空特征图像
	void Delete();							//删除容器	
};
#endif
