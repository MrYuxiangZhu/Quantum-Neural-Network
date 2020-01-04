/********************************************************************************
****文件名：BP神经网络头文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/
#ifndef __NEURALNET_HPP
#define __NEURALNET_HPP
#define ACTIVATION_RESPONSE 1.0
#include<string>
#include<vector>
#include<math.h>
#include<stdlib.h>
#include<iostream>
#include<stdio.h>
#include <fstream>
#include <sstream>    //使用stringstream需要引入这个头文件

using namespace std;

typedef vector<float> iovector;
typedef vector<iovector > float_cell;

/********************************************************************************
*******数据
********************************************************************************/
class Data{
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
    Data(int VideoType,int VideoNum, int Frames, int datas);
    void AddData(vector<float_cell > indata, vector<float_cell > outdata){
		SetIn = indata;
		SetOut = outdata;
    }
    vector<float_cell > GetInputSet() {return SetIn;}
    vector<float_cell > GetOutputSet(){return SetOut;}
	void Create_Data(vector<float_cell >& input, vector<float_cell >& output);
    void Write_Data(string filename);	//数据写入硬盘
	void Read_Data(string filename);	//从硬盘中读数据
	~Data();
};
/********************************************************************************
*******单个神经元
********************************************************************************/
struct Neuron{
    int NumInputs;//神经元的输入量
    vector<float> vecWeight;//权重
    float Activation;//这里是根据输入，通过某个线性函数确定，相当于神经元的输出
    float Error;//误差值
    float RandomClamped(){
        return -1+2*(rand()/((float)RAND_MAX+1));
    }
    Neuron(int inputs){
        NumInputs=inputs+1;
        Activation=0;
        Error=0;
        for(int i=0;i<NumInputs+1;i++){
			vecWeight.push_back(RandomClamped());//初始化权重
		}
    }
};
/********************************************************************************
*******多个神经元
********************************************************************************/
struct NeuronLayer{
    int	NumNeurons;//每层拥有的神经元数
    vector<Neuron>	vecNeurons;
    NeuronLayer(int neurons, int perNeuron):NumNeurons(neurons){
        for(int i=0;i<NumNeurons;i++){
			vecNeurons.push_back(Neuron(perNeuron));				//每个神经元包含权值的个数，也就是与上一层神经元或者输出连接权值个数
		}
    }
};

typedef vector<float> iovector;
/********************************************************************************
*******神经网络
********************************************************************************/
class NeuralNet{
private:
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
    vector<NeuronLayer> vecLayers;//层数
    bool NetworkTrainingEpoch(vector<iovector > &SetIn,vector<iovector > &SetOut);//训练神经网络
    void CreateNet();//生成网络
    void InitializeNetwork();//初始化
    inline float Sigmoid(float activation, float response);
	inline float atanderivate(float activation);
    float RandomClamped(){
        return -1+2*(rand()/((float)RAND_MAX+1));
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
    bool Train(Data data);//开始训练
    enum STOPTYPE{COUNT,ERRORSUM}StopType;
    NeuralNet(int inputlayer,int outputlayer,int hiddenneurons,int hiddenLayersnum,\
	float learningRate,float errorthresh,long int trainepochs,STOPTYPE type,bool debug,float errorsum,bool trained,int numepochs);//初始化网络
    void SetErrorThrehold(float num){ERROR_THRESHOLD=num;}//设置误差
    void SetCount(long int num){TrainCount=num;}//设置训练次数
    vector<float> Update(vector<float> inputs);//得到输出
    NeuralNet(string filename);//通过文件地址打开一个已经训练好的网络
    void saveNet(string filename);//保存已经训练的网络
    float RecognitionAccuracy(vector<float_cell >& SetIn, vector<float_cell >& SetOut, int Total, bool Debug);//识别图像准确率
	vector<float> ActualArray(vector<float>& input);//实际输出
    int Recongnition(vector<float>& inputs, vector<float>& output, bool Debug);//识别
	~NeuralNet();
};
#endif // NEURALNET_H
