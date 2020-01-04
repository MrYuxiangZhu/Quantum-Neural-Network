/********************************************************************************
****文件名：量子BP神经网络头文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2019/3/33
*********************************************************************************/
#ifndef __QUANTUMNEURALNET_HPP
#define __QUANTUMNEURALNET_HPP
#define ACTIVATION_RESPONSE 1.0
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <numeric>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;

#define PI 3.141592
typedef vector<float> iovector;
typedef vector<iovector > float_cell;

/********************************************************************************
*******数据
********************************************************************************/
class QuantumData {
private:
    vector<float_cell > SetIn;//记录训练用的数据
    vector<float_cell > SetOut;//记录训练用数据的目标输出
	int EachNums;			//每一帧多少数据
    int FrameNums;//帧数量
    int VideoNumVectorSize;//一种类型视频个数
    int VideoTypeVectorSize;//视频种类，动作种类
	bool Debug;//是否输出误差值
public:
	//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）  
	template <class Type>
	Type stringToNum(const string& str)
	{
		istringstream iss(str);
		Type num;
		iss >> num;
		return num;
	}
    QuantumData(int VideoType,int VideoNum, int Frames, int datas);
    void AddData(vector<float_cell > indata, vector<float_cell > outdata){
		SetIn = indata;
		SetOut = outdata;
    }
    vector<float_cell > GetInputSet() {return SetIn;}
    vector<float_cell > GetOutputSet(){return SetOut;}
	int GetVideoType(){return VideoTypeVectorSize;};
	int GetVideoNum(){return VideoNumVectorSize;};
	int GetFrameNums(){return FrameNums;};
	int GetEachNums(){return EachNums;};
	void Write_Data(string filename);	//数据写入硬盘
	void Read_Data(string filename);	//从硬盘中读数据
	void Create_Data(vector<float_cell >& input, vector<float_cell >& output);
	vector<float> ChangeToInput(vector<float>& input);//乘以PI / 2
	vector<float> ChangeToInput(vector<vector<float> >& input);//乘以PI / 2
	~QuantumData();
};
/********************************************************************************
*******单个神经元
********************************************************************************/
struct QuantumNeuron {
    int NumInputs;//神经元的输入量
    vector<float> VecWeight;//权重角度
	vector<float> VecTij;//Tij参数
	float ParaAj;//Aj参数
	float ParaBj;//Bj参数
	float Arctg;//arctg
	float ParaSigmoid;//Sigmoid权重
    float Activation;//这里是根据输入，通过某个线性函数确定，相当于神经元的输出
    float Error;//误差值
	//int NetLayer 代表是否是隐层，输入层与隐层,第一层网络之间权值是角度
    float RandomClamped(int NetLayer){
		//srand(time(NULL));//设置随机数种子，使每次产生的随机序列不同
		if (NetLayer == 1)
			return rand() / ((float)RAND_MAX + 0.0) * PI / 2;
		else
			return -1 + 2 * (rand() / ((float)RAND_MAX + 1));
    }
    QuantumNeuron(int inputs, int NetLayer){
        //NumInputs = inputs + 1;
		NumInputs = inputs;
		ParaAj = 0;
		ParaBj = 0;
		Arctg = 0;
        Activation = 0;
        Error = 0;
		if (NetLayer == 1) 
		{
			for(int i = 0; i < NumInputs; ++i){
				VecWeight.push_back(RandomClamped(NetLayer));//初始化权重角度
				VecTij.push_back(0);
			}
			ParaSigmoid = RandomClamped(2);//初始化Sigmoid权重			
		}
		else 
		{
			for(int i = 0; i < NumInputs; ++i){
				VecWeight.push_back(RandomClamped(NetLayer));//初始化权重
				VecTij.push_back(0);
			}
			ParaSigmoid = RandomClamped(2);//初始化Sigmoid权重	
		}
    }
};
/********************************************************************************
*******多个神经元
********************************************************************************/
struct QuantumNeuronLayer {
    int	NumNeurons;//每层拥有的神经元数
    vector<QuantumNeuron> VecNeurons;//神经元容器
	vector<float> ErrorLayer;
	float RandomClamped(void){
		return -1 + 2 * (rand() / ((float)RAND_MAX + 1));
    }
    QuantumNeuronLayer(int neurons, int perNeuron, int NetLayer):NumNeurons(neurons){
        for(int i = 0; i < NumNeurons; ++i){
			VecNeurons.push_back(QuantumNeuron(perNeuron, NetLayer));//每个神经元包含权值的个数，也就是与上一层神经元或者输出连接权值个数
			ErrorLayer.push_back(0);
		}
    }
};

/********************************************************************************
*******神经网络
********************************************************************************/
class QuantumNeuralNet {
private:
	int NumLayers;//总层数
    int NumInputs;//输入量
    int NumOutputs;//输出量
    int NumHiddenLayers;//隐藏层数
    int NeuronsPerHiddenLayer;//隐藏层拥有的神经元
    float LearningRate;//学习率
    float ErrorSum;//误差总值
    bool Trained;//是否已经训练过
    int NumEpochs;//代数
    float ERROR_THRESHOLD;     //误差阈值（什么时候停止训练）
    long int TrainCount;     //训练次数（什么时候停止训练）
    vector<QuantumNeuronLayer> VecLayers;//层数
    bool NetworkTrainingEpoch(vector<iovector >& SetIn, vector<iovector >& SetOut);//训练神经网络
    void CreateNet();//生成网络
    void InitializeNetwork();//初始化
    inline float Sigmoid(float activation, float response);
	inline float atanderivate(float activation);
    float RandomClamped(int NetLayer){
		if (NetLayer == 1)
			return rand() / ((float)RAND_MAX + 0.0) * PI / 2;
		else
			return -1 + 2 * (rand() / ((float)RAND_MAX + 1));
    }
    bool Debug;//是否输出误差值
public:
	//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）  
	template <class Type>
	Type stringToNum(const string& str)
	{
		istringstream iss(str);
		Type num;
		iss >> num;
		return num;
	}
    bool Train(QuantumData data, bool Debug);//开始训练
    enum STOPTYPE{COUNT,ERRORSUM}StopType;
	QuantumNeuralNet();
	QuantumNeuralNet(string filename);//通过文件地址打开一个已经训练好的网络
    QuantumNeuralNet(int inputlayer,int outputlayer,int hiddenneurons,int hiddenLayersnum,\
	float learningRate,float errorthresh,long int trainepochs,STOPTYPE type,bool debug,float errorsum,bool trained,int numepochs);//初始化网络
    void SetErrorThrehold(float num){ERROR_THRESHOLD = num;}//设置误差
    void SetCount(long int num){TrainCount = num;}//设置训练次数
    vector<float> Update(vector<float> inputs);//得到输出
    void SaveNet(string filename);  //保存已经训练的网络
	int Recongnition(vector<float>& inputs, vector<float>& output, bool Debug);//识别
	float RecognitionAccuracy(vector<float_cell >& SetIn, vector<float_cell >& SetOut, int Total, bool Debug);//识别图像准确率
	vector<float> ActualArray(vector<float>& input);//实际输出
	void Delete();
	~QuantumNeuralNet();
};
#endif // NEURALNET_H
