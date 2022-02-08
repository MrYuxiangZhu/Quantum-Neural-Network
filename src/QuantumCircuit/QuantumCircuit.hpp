/********************************************************************************
****文件名：量子电路头文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/11/15
*********************************************************************************/
#ifndef __QUANTUMCIRCUIT_HPP
#define __QUANTUMCIRCUIT_HPP

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

/********************************************************************************
*******量子操作
********************************************************************************/
struct Quantum_Operator 
{
public:
	char QuantumState[2] = {'0','1'};			//量子基态
	void Not_Gate(char& ctrl, bool Debug);//NOT门
    void Cnot_Gate(const char& ctrl, char& ctrled, bool Debug);//CNOT门
    void Toffoli_Gate(const char& ctrlone, const char& ctrltwo, char& ctrled, bool Debug);//Toffoli门
};
/********************************************************************************
*******量子加法子模块
********************************************************************************/
struct Quantum_Adder_accumulate_block : public Quantum_Operator 
{
public:
	void Quantum_Adder_Operator(const vector<char>& NumOne, const vector<char>& NumTwo, vector<char>& NumSum, vector<char>& Carry, char& OverFlow, bool Debug);//量子加法器
	template<typename Iterator>
	void operator()(const vector<vector<vector<char>>>& QuantumImageOne, const vector<vector<vector<char>>>& QuantumImageTwo, 
					Iterator first, Iterator last, Iterator colums, vector<vector<vector<char>>>& result, vector<vector<vector<char>>>& AdderCarry, 
					vector<vector<char>>& AdderOverFlow, bool Debug);//量子图像加法并行计算子单元
};

/********************************************************************************
*******量子全加器
********************************************************************************/
class Quantum_Adder : public Quantum_Adder_accumulate_block, public HardWare
{
private:
	int Height;													//图像高度
	int Width;													//图像宽度
	int QuantumNumX;											//X轴量子数量
	int QuantumNumY;											//Y轴量子数量
	vector<int>ClassicImageXaxis;								//经典图像X轴
	vector<int>ClassicImageYaxis;								//经典图像Y轴
	vector<vector<vector<char>>> QuantumImageOne;				//第一张量子图像
	vector<vector<vector<char>>> QuantumImageTwo;				//第二张量子图像
    vector<vector<vector<char>>> QuantumAdderResults;			//量子加法结果
	vector<vector<vector<char>>> QuantumAdderCarry;				//加法进位
	vector<vector<char>> QuantumAdderOverFlow;					//减法溢出位
	unsigned int hardware_threads;								//CPU线程数
public:
    Quantum_Adder();
    ~Quantum_Adder();
	void QuantumImage_Adder_Operator_Init(NEQR& neqr);			//初始化
	template<typename Iterator>
	void Adder_Operator_parallel_accumulate(const vector<vector<vector<char>>>& QuantumImageOne, const vector<vector<vector<char>>>& QuantumImageTwo, 
											Iterator first, Iterator last, Iterator colums, bool Debug);//量子加法器多线程并行计算
	void QuantumImage_Adder_Operator(NEQR& NeqrOne, NEQR& NeqrTwo, NEQR& Result, bool Debug);//量子图像加法器
	virtual void Reset_Vector(void);							//清空容器
	void size(NEQR& QuantumImage);								//获取量子图像大小
	virtual void Show_Results(void);							//显示结果
	virtual void Show_Results(string name);						//显示结果
	virtual void Delete();										//删除容器
};

/********************************************************************************
*******量子取反子模块
********************************************************************************/
struct Quantum_Reverse_accumulate_block:public Quantum_Adder 
{
public:
    void Reverse_Operator(vector<char>& NumOne, bool Debug);//取反码
	template<typename Iterator>
	void operator()(Iterator first, Iterator last, Iterator colums, vector<vector<vector<char>>>& QuantumImageReverse, bool Debug);//量子图像取反并行计算子单元
};

/********************************************************************************
*******量子取补子模块
********************************************************************************/
struct Quantum_Complement_accumulate_block : public Quantum_Reverse_accumulate_block 
{
public:
	void Complement_Operator(const vector<char>& Number, const vector<char>& Unit, vector<char>& Complement, vector<char>& Carry, char& OverFlow, bool Debug);//取补码
	template<typename Iterator>
	void operator()(const vector<vector<vector<char>>>& QuantumImageSrc, const vector<vector<vector<char>>>& QuantumImageUnit, 
					Iterator first, Iterator last, Iterator colums, vector<vector<vector<char>>>& QuantumImageComplement, 
					vector<vector<vector<char>>>& AdderCarry, vector<vector<char>>& AdderOverFlow, bool Debug);//量子图像取补并行计算子单元
};

/********************************************************************************
*******量子减法子模块
********************************************************************************/
struct Quantum_Subtracter_accumulate_block:public Quantum_Complement_accumulate_block 
{
public:
	void Quantum_Subtracter_Operator(const vector<char>& NumOne, const vector<char>& NumTwo, vector<char>& NumSum, vector<char>& Carry, char& OverFlow, bool Debug);//量子加法器
	template<typename Iterator>
	void operator()(const vector<vector<vector<char>>>& QuantumImageOne, const vector<vector<vector<char>>>& QuantumImageTwo, 
					Iterator first, Iterator last, Iterator colums, vector<vector<vector<char>>>& QuantumSubtracterResults, 
					vector<vector<vector<char>>>& QuantumSubtracterCarry, vector<vector<char>>& QuantumSubtracterOverFlow, bool Debug);//量子图像减法并行计算子单元
};

/********************************************************************************
*******量子减法器
********************************************************************************/
class Quantum_Subtracter : public Quantum_Subtracter_accumulate_block 
{
private:
	int Height;													//图像高度
	int Width;													//图像宽度
	int QuantumNumX;											//X轴量子数量
	int QuantumNumY;											//Y轴量子数量
	vector<int>ClassicImageXaxis;								//经典图像X轴
	vector<int>ClassicImageYaxis;								//经典图像Y轴
	vector<vector<vector<char>>> QuantumImageOne;				//第一张量子图像
	vector<vector<vector<char>>> QuantumImageTwo;				//第二张量子图像
	vector<vector<vector<char>>> QuantumUnitImage;		  		//量子图像像素值全为1
	vector<vector<vector<char>>> QuantumImageReverse;			//量子图像反码
	vector<vector<vector<char>>> QuantumImageComplement;		//量子图像补码
	vector<vector<vector<char>>> QuantumSubtracterResults;		//量子减法结果，绝对值
	vector<vector<vector<char>>> QuantumSubtracterCarry;		//减法进位
	vector<vector<char>> QuantumSubtracterOverFlow;				//减法溢出位
	unsigned int hardware_threads;								//CPU线程数
public:
    Quantum_Subtracter();
    ~Quantum_Subtracter();
	void QuantumImage_Subtracter_Operator_Init(NEQR& neqr);														//初始化
	template<typename Iterator>
	void Reverse_Operator_parallel_accumulate(vector<vector<vector<char>>>& QuantumImageReverse, Iterator first, Iterator last, Iterator colums, bool Debug);//量子取反多线程并行计算
	template<typename Iterator>
	void Complement_Operator_parallel_accumulate(const vector<vector<vector<char>>>& QuantumImage, Iterator first, Iterator last, Iterator colums, bool Debug);//量子取补多线程并行计算
	template<typename Iterator>
	void Subtracter_Operator_parallel_accumulate(const vector<vector<vector<char>>>& QuantumImageOne, vector<vector<vector<char>>>& QuantumImageTwo, Iterator first, Iterator last, Iterator colums, bool Debug);//量子减法器多线程并行计算
	void QuantumImage_Subtracter_Operator(NEQR& NeqrOne, NEQR& NeqrTwo, NEQR& Result, bool Debug);//量子图像减法器
	vector<vector<vector<char>>> Get_Subtracter_Results(void) 	{ return QuantumSubtracterResults; };//获取减法器结果，绝对值
	vector<vector<vector<char>>> Get_Subtracter_Carry(void) 	{ return QuantumSubtracterCarry; };//获取减法器进位
	vector<vector<char>> Get_Subtracter_OverFlow(void) 			{ return QuantumSubtracterOverFlow; };//获取减法器溢出位
	void Reset_Image_Reverse(void);								//清空反码容器
	void Reset_Image_Complement(void);							//清空补码容器
	virtual void Reset_Vector(void);							//清空容器
	void size(NEQR &QuantumImage);								//获取量子图像大小
	virtual void Show_Results(void);							//显示结果
	virtual void Show_Results(string name);						//显示结果
	virtual void Delete();										//删除容器
};
#endif