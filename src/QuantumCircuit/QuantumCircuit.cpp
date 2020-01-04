/********************************************************************************
****文件名：量子电路源文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/12/23
*********************************************************************************/
#include "QuantumCircuit.hpp"

/******************************************************************************
****函数名：Not_Gate()
****说  明：NOT取反门
****参  数：
****char& ctrl：控制量子位
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
void Quantum_Operator::Not_Gate(char& ctrl, bool Debug) {
    if (ctrl == '1') {
        ctrl = '0';
    }
    else {
        ctrl = '1';
    }
}
/******************************************************************************
****函数名：Cnot_Gate()
****说  明：CNOT门
****参  数：
****char& ctrl：控制量子位
****char& ctrled：受控量子位
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
void Quantum_Operator::Cnot_Gate(const char& ctrl, char& ctrled, bool Debug){
    if (ctrl == '1') {
		if (ctrled  == '1') {
			ctrled = '0';
		}
		else {
			ctrled = '1';
		}
	}
}
/******************************************************************************
****函数名：Toffoli_Gate()
****说  明：Toffoli门
****参  数：
****char& ctrlone：控制量子位
****char& ctrltwo：控制量子位
****char& ctrled：受控量子位
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
void Quantum_Operator::Toffoli_Gate(const char& ctrlone, const char& ctrltwo, char& ctrled, bool Debug){
    if (ctrlone == '1' && ctrltwo == '1') {
		if (ctrled == '1') {
			ctrled = '0';
		}
		else {
			ctrled = '1';
		}
	}
}
/********************************************************************************
****函数名：Quantum_Adder_Operator()
****说  明：量子加法器子模块
****参  数：
****vector<char>& NumOne: 加数1
****vector<char>& NumTwo: 加数2
****vector<char>& NumSum: 和
****vector<char>& Carry：进位
****char& OverFlow：溢出标志位
****bool Debug：是否调试打印
****返回值：无
********************************************************************************/
void Quantum_Adder_accumulate_block::Quantum_Adder_Operator(const vector<char>& NumOne, const vector<char>& NumTwo, vector<char>& NumSum, vector<char>& Carry, char& OverFlow, bool Debug) {
    int NumOneSize = NumOne.size();
    int NumTwoSize = NumTwo.size();
    int HalfAdder = NumOneSize;		//半加器个数
    //执行半加器计算,数组中从高到低位排序
    for (int half = HalfAdder - 1; half >= 0; --half) {
        Cnot_Gate(NumOne[half], NumSum[half], Debug);
        Cnot_Gate(NumTwo[half], NumSum[half], Debug);
        Toffoli_Gate(NumOne[half], NumTwo[half], Carry[half], Debug);
    }
    //串行进位
    for (int serial = HalfAdder - 2; serial >= 0; --serial) {
        Toffoli_Gate(NumSum[serial], Carry[serial + 1], Carry[serial], Debug);
        Cnot_Gate(Carry[serial + 1], NumSum[serial], Debug);
    }
    //输出溢出标志位
    Cnot_Gate(Carry[0], OverFlow, Debug);
}
/******************************************************************************
****函数名：Quantum_Adder_accumulate_block::operator()
****说  明：量子图像加法并行计算子单元
****参数：
****const vector<vector<vector<char> > >& QuantumImageOne: 第一张量子图像
****const vector<vector<vector<char> > >& QuantumImageTwo: 第二张量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****vector<vector<vector<char> > >& result：结果值
****vector<vector<vector<char> > >& AdderCarry：进位
****vector<vector<char> >& AdderOverFlow：溢出位
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Quantum_Adder_accumulate_block::operator()(const vector<vector<vector<char> > >& QuantumImageOne, const vector<vector<vector<char> > >& QuantumImageTwo, Iterator first, Iterator last, Iterator colums, 
				vector<vector<vector<char> > >& result, vector<vector<vector<char> > >& AdderCarry, vector<vector<char> >& AdderOverFlow, bool Debug) {
    for (int m = *first; m <= *(last-1); ++m) {
		for (int n = 0; n <= *(colums-1); ++n) {
            Quantum_Adder_Operator(QuantumImageOne[m][n], QuantumImageTwo[m][n], result[m][n], AdderCarry[m][n], AdderOverFlow[m][n], Debug);
		}
	}
}
/******************************************************************************
****函数名：Quantum_Adder()
****说  明：Quantum_Adder构造函数
****参  数：无
****返回值：无
*******************************************************************************/
Quantum_Adder::Quantum_Adder()
{
	hardware_threads = Obtain_Thread_Num();
	//std::cout << dec << "Quantum_Adder_hardware_threads= " << hardware_threads << std::endl;
}
/******************************************************************************
********量子全加器析构函数
*******************************************************************************/
Quantum_Adder::~Quantum_Adder()
{
	Delete();
}
/********************************************************************************
****函数名：QuantumImage_Adder_Operator_Init()
****说  明：量子加法初始化
****参  数：
****NEQR &neqr:量子图像
****返回值：无
********************************************************************************/
void Quantum_Adder::QuantumImage_Adder_Operator_Init(NEQR& neqr) {
    size(neqr);
	//初始化经典图像X轴坐标
	for(int i = 0; i < Width; ++i) {
        ClassicImageXaxis.push_back(i);
    }
    //初始化经典图像Y轴坐标
	for(int j = 0; j < Height; ++j) {
        ClassicImageYaxis.push_back(j);
    }
    //初始化量子图像
    vector<char> pixel_temp;
	vector<vector<char> > quantum_temp;
	//初始化量子图像
	for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			for (int k = 0; k < 8; ++k) {
				pixel_temp.push_back(QuantumState[0]);        
			}
			quantum_temp.push_back(pixel_temp);
			pixel_temp.clear();
		}
		QuantumImageOne.push_back(quantum_temp);
        QuantumImageTwo.push_back(quantum_temp);
        QuantumAdderResults.push_back(quantum_temp);
        QuantumAdderCarry.push_back(quantum_temp);
		quantum_temp.clear();
	}
    //初始化溢出位
    vector<char> overflow_temp;
	for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			overflow_temp.push_back(QuantumState[0]);
		}
        QuantumAdderOverFlow.push_back(overflow_temp);
        overflow_temp.clear();
	}
    //删除容器
	vector<char>().swap(pixel_temp);
    vector<char>().swap(overflow_temp);
	vector<vector<char> >().swap(quantum_temp);
}
/********************************************************************************
****函数名：Adder_Operator_parallel_accumulate()
****说  明：量子加法器多线程并行计算
****参  数：
****const vector<vector<vector<char> > >& QuantumImageOne: 第一张量子图像
****const vector<vector<vector<char> > >& QuantumImageTwo: 第二张量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug：是否显示
****返回值：无
********************************************************************************/
template<typename Iterator>
void Quantum_Adder::Adder_Operator_parallel_accumulate(const vector<vector<vector<char> > >& QuantumImageOne, const vector<vector<vector<char> > >& QuantumImageTwo, Iterator first, Iterator last, Iterator colums, bool Debug) {
	unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) {//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Quantum_Adder_accumulate_block(), std::ref(QuantumImageOne), std::ref(QuantumImageTwo), block_start, block_end, colums, 
                                std::ref(QuantumAdderResults), std::ref(QuantumAdderCarry), std::ref(QuantumAdderOverFlow), Debug);//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Quantum_Adder_accumulate_block()(QuantumImageOne, QuantumImageTwo, block_start, last, colums, 
                                    QuantumAdderResults, QuantumAdderCarry, QuantumAdderOverFlow, Debug);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
    if (Debug) {

    }
}
/********************************************************************************
****函数名：QuantumImage_Adder_Operator()
****说  明：量子加法器
****参  数：
****NEQR& NeqrOne: 第一张量子图像
****NEQR& NeqrTwo: 第二张量子图像
****NEQR& Result：结果值
****bool Debug：是否显示
****返回值：无
********************************************************************************/
void Quantum_Adder::QuantumImage_Adder_Operator(NEQR& NeqrOne, NEQR& NeqrTwo, NEQR& Result, bool Debug) {
    if (Debug) {
		double time;
    	double start;
		std::cout << "NEQR Quantum Image Adder Operator!" << std::endl;
		start = static_cast<double>(getTickCount());
        Reset_Vector();          //清空结果和标志
		QuantumImageOne = NeqrOne.Get_Quantum_Image_Pixel();
        QuantumImageTwo = NeqrTwo.Get_Quantum_Image_Pixel();
		Adder_Operator_parallel_accumulate(QuantumImageOne, QuantumImageTwo, ClassicImageXaxis.begin(), ClassicImageXaxis.end(), ClassicImageYaxis.end(), false);
		Result.Set_Quantum_Image_Pixel(QuantumAdderResults);
        time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Adder Operator = " << time << " ms" << std::endl;
	}
	else {
        Reset_Vector();          //清空结果和标志
        QuantumImageOne = NeqrOne.Get_Quantum_Image_Pixel();
        QuantumImageTwo = NeqrTwo.Get_Quantum_Image_Pixel();
		Adder_Operator_parallel_accumulate(QuantumImageOne, QuantumImageTwo, ClassicImageXaxis.begin(), ClassicImageXaxis.end(), ClassicImageYaxis.end(), false);
        Result.Set_Quantum_Image_Pixel(QuantumAdderResults);
    }
}
/********************************************************************************
****函数名：Reset_Vector()
****说  明：清空容器
****参  数：无
****返回值：无
********************************************************************************/
void Quantum_Adder::Reset_Vector(void) {
    for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			for (int k = 0; k < 8; ++k) {
				QuantumAdderResults[i][j][k] = QuantumState[0];
                QuantumAdderCarry[i][j][k] = QuantumState[0];         
			}
            QuantumAdderOverFlow[i][j] = QuantumState[0];   
		}
	}
}
/********************************************************************************
****函数名：size()
****说  明：获取NEQR量子图像大小
****参  数：
****NEQR &QuantumImage:量子图像
****返回值：无
********************************************************************************/
void Quantum_Adder::size(NEQR& QuantumImage) {
	Height = QuantumImage.Get_Image_Height();
	Width = QuantumImage.Get_Image_Width();
	QuantumNumX = QuantumImage.Get_QuantumNumX();
	QuantumNumY = QuantumImage.Get_QuantumNumY();
	//std::cout << "Height,Width= " << Height << "," << Width << std::endl;
	//std::cout << "QuantumNumX,QuantumNumY,QuantumNumTotal= " << QuantumNumX << "," << QuantumNumY << "," << QuantumNumTotal << std::endl;
}
/******************************************************************************
****函数名：Show_Results()
****说  明：显示结果
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Adder::Show_Results(void) {
    for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
            std::cout << "QuantumAdderResults[" << i << "][" << j << "] = |" << QuantumAdderOverFlow[i][j] << ">" << "|";
			for (int k = 0; k < 8; ++k) {
				std::cout << QuantumAdderResults[i][j][k];      
			}
            std::cout << ">  ";
            std::cout << "QuantumAdderCarry[" << i << "][" << j << "] = |";
            for (int k = 0; k < 8; ++k) {
				std::cout << QuantumAdderCarry[i][j][k];      
			}
            std::cout << ">  ";
            std::cout << "QuantumAdderOverFlow[" << i << "][" << j << "] = " << QuantumAdderOverFlow[i][j] << std::endl;
		}
	}
}
/******************************************************************************
****函数名：Show_Results()
****说  明：显示结果
****参  数：
****string name：名字
****返回值：无
*******************************************************************************/
void Quantum_Adder::Show_Results(string name) {
    for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
            std::cout << name << "_Results[" << i << "][" << j << "] = |" << QuantumAdderOverFlow[i][j] << ">" << "|";
			for (int k = 0; k < 8; ++k) {
				std::cout << QuantumAdderResults[i][j][k];      
			}
            std::cout << ">  ";
            std::cout << name << "_Carry[" << i << "][" << j << "] = |";
            for (int k = 0; k < 8; ++k) {
				std::cout << QuantumAdderCarry[i][j][k];      
			}
            std::cout << ">  ";
            std::cout << name << "_OverFlow[" << i << "][" << j << "] = " << QuantumAdderOverFlow[i][j] << std::endl;
		}
	}
}
/******************************************************************************
****函数名：Delete()
****说  明：删除容器
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Adder::Delete() {
	vector<int>().swap(ClassicImageXaxis);								
	vector<int>().swap(ClassicImageYaxis);
    vector<vector<char> >().swap(QuantumAdderOverFlow);
    vector<vector<vector<char> > >().swap(QuantumImageOne);	
    vector<vector<vector<char> > >().swap(QuantumImageTwo);	
    vector<vector<vector<char> > >().swap(QuantumAdderCarry);								
	vector<vector<vector<char> > >().swap(QuantumAdderResults);
}	
/********************************************************************************
****函数名：Reverse_Operator()
****说  明：量子取反子模块
****参  数：
****vector<char>& NumOne: 多位量子
****bool Debug：是否显示
****返回值：无
********************************************************************************/
void Quantum_Reverse_accumulate_block::Reverse_Operator(vector<char>& NumOne, bool Debug) {
    for (int size = 0; size < NumOne.size(); size++) {
        Not_Gate(NumOne[size], Debug);
    }
}
/******************************************************************************
****函数名：Quantum_Reverse_accumulate_block::operator()
****说  明：量子图像取反并行计算子单元
****参数：
****vector<vector<vector<char> > >& QuantumImageReverse: 量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Quantum_Reverse_accumulate_block::operator()(Iterator first, Iterator last, Iterator colums, vector<vector<vector<char> > >& QuantumImageReverse, bool Debug) {
    for (int m = *first; m <= *(last-1); ++m) {
		for (int n = 0; n <= *(colums-1); ++n) {
            Reverse_Operator(QuantumImageReverse[m][n], Debug);
		}
	}
}
/******************************************************************************
****函数名：Complement_Operator()
****说  明：取补操作
****参  数：
****const vector<char>& Number: 需要取补的量子数
****const vector<char>& Unit: 单位量子
****vector<char>& Complement: 补码
****vector<char>& Carry：进位
****char& OverFlow：溢出标志位
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
void Quantum_Complement_accumulate_block::Complement_Operator(const vector<char>& Number, const vector<char>& Unit, vector<char>& Complement, vector<char>& Carry, char& OverFlow, bool Debug) {
    Quantum_Adder_Operator(Number, Unit, Complement, Carry, OverFlow, Debug);
}
/******************************************************************************
****函数名：Quantum_Complement_accumulate_block::operator()
****说  明：量子取补多线程并行计算子模块
****参数：
****const vector<vector<vector<char> > >& QuantumImageSrc: 原量子图像
****const vector<vector<vector<char> > >& QuantumImageUnit: 单位量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****const vector<vector<vector<char> > >& QuantumImageComplement: 量子图像补码
****vector<vector<vector<char> > >& AdderCarry：进位
****vector<vector<char> >& AdderOverFlow：溢出标志位
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Quantum_Complement_accumulate_block::operator()(const vector<vector<vector<char> > >& QuantumImageSrc, const vector<vector<vector<char> > >& QuantumImageUnit, 
Iterator first, Iterator last, Iterator colums, vector<vector<vector<char> > >& QuantumImageComplement, vector<vector<vector<char> > >& AdderCarry, vector<vector<char> >& AdderOverFlow, bool Debug) {
    for (int m = *first; m <= *(last-1); ++m) {
		for (int n = 0; n <= *(colums-1); ++n) {
            Complement_Operator(QuantumImageSrc[m][n], QuantumImageUnit[m][n], QuantumImageComplement[m][n], AdderCarry[m][n], AdderOverFlow[m][n], Debug);
		}
	}
}
/********************************************************************************
****函数名：Quantum_Subtracter_Operator()
****说  明：量子减法器子模块
****参  数：
****vector<char>& NumOne: 被减数
****vector<char>& NumTwo: 减数
****vector<char>& NumSum: 差
****vector<char>& Carry：进位
****char& OverFlow：溢出标志位
****bool Debug：是否调试打印
****返回值：无
********************************************************************************/
void Quantum_Subtracter_accumulate_block::Quantum_Subtracter_Operator(const vector<char>& NumOne, const vector<char>& NumTwo, vector<char>& NumSum, vector<char>& Carry, char& OverFlow, bool Debug) {
    int NumOneSize = NumOne.size();
    int NumTwoSize = NumTwo.size();
    int HalfAdder = NumOneSize;		//半加器个数
    //执行半加器计算,数组中从高到低位排序
    for (int half = HalfAdder - 1; half >= 0; --half) {
        Cnot_Gate(NumOne[half], NumSum[half], Debug);
        Cnot_Gate(NumTwo[half], NumSum[half], Debug);
        Toffoli_Gate(NumOne[half], NumTwo[half], Carry[half], Debug);
    }
    //串行进位
    for (int serial = HalfAdder - 2; serial >= 0; --serial) {
        Toffoli_Gate(NumSum[serial], Carry[serial + 1], Carry[serial], Debug);
        Cnot_Gate(Carry[serial + 1], NumSum[serial], Debug);
    }
    //输出溢出标志位
    Cnot_Gate(Carry[0], OverFlow, Debug);
    if (OverFlow == QuantumState[0])                    //OverFlow是|0>则取反取补
    {
        vector<char> unit_temp(NumOneSize);
        vector<char> result_temp(NumOneSize);
        vector<char> carry_temp(NumOneSize);
        char flow_temp = QuantumState[0];
        for (int m = 0; m < NumOneSize - 1; ++m) {
            unit_temp[m] = QuantumState[0];
            result_temp[m] = QuantumState[0];
            carry_temp[m] = QuantumState[0];
        }
        unit_temp[NumOneSize - 1] = QuantumState[1];
        result_temp[NumOneSize - 1] = QuantumState[0];
        carry_temp[NumOneSize - 1] = QuantumState[0];

        Reverse_Operator(NumSum, Debug);
        Complement_Operator(NumSum, unit_temp, result_temp, carry_temp, flow_temp, Debug);
        NumSum = result_temp;                         //对差值取反取补
        vector<char>().swap(unit_temp);
        vector<char>().swap(result_temp);
        vector<char>().swap(carry_temp);
    }
}
/******************************************************************************
****函数名：Quantum_Subtracter_accumulate_block::operator()
****说  明：量子减法多线程并行计算子模块
****参数：
****const vector<vector<vector<char> > >& QuantumImageOne: 第一张量子图像
****const vector<vector<vector<char> > >& QuantumImageTwo: 第二张量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****vector<vector<vector<char> > >& QuantumSubtracterResults：结果值
****vector<vector<vector<char> > >& QuantumSubtracterCarry：进位
****vector<vector<char> >& QuantumSubtracterOverFlow：溢出位
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Quantum_Subtracter_accumulate_block::operator()(const vector<vector<vector<char> > >& QuantumImageOne, const vector<vector<vector<char> > >& QuantumImageTwo, 
Iterator first, Iterator last, Iterator colums, vector<vector<vector<char> > >& QuantumSubtracterResults, vector<vector<vector<char> > >& QuantumSubtracterCarry, vector<vector<char> >& QuantumSubtracterOverFlow, bool Debug) {
    for (int m = *first; m <= *(last-1); ++m) {
		for (int n = 0; n <= *(colums-1); ++n) {
            Quantum_Subtracter_Operator(QuantumImageOne[m][n], QuantumImageTwo[m][n], QuantumSubtracterResults[m][n], QuantumSubtracterCarry[m][n], QuantumSubtracterOverFlow[m][n], Debug);
		}
	}
}
/******************************************************************************
****函数名：Quantum_Subtracter()
****说  明：量子减法构造函数
****参  数：无
****返回值：无
*******************************************************************************/
Quantum_Subtracter::Quantum_Subtracter()
{
	hardware_threads = Obtain_Thread_Num();
	//std::cout << dec << "Quantum_Subtracter_hardware_threads= " << hardware_threads << std::endl;
}
/******************************************************************************
****函数名：~Quantum_Subtracter()
****说  明：量子减法析构函数
****参  数：无
****返回值：无
*******************************************************************************/
Quantum_Subtracter::~Quantum_Subtracter()
{
	Delete();
}
/********************************************************************************
****函数名：QuantumImage_Subtracter_Operator_Init()
****说  明：量子减法初始化
****参  数：
****NEQR &neqr:量子图像
****返回值：无
********************************************************************************/
void Quantum_Subtracter::QuantumImage_Subtracter_Operator_Init(NEQR& neqr) {
    size(neqr);
	//初始化经典图像X轴坐标
	for(int i = 0; i < Width; ++i) {
        ClassicImageXaxis.push_back(i);
    }
    //初始化经典图像Y轴坐标
	for(int j = 0; j < Height; ++j) {
        ClassicImageYaxis.push_back(j);
    }
    //初始化量子图像
    vector<char> pixel_temp;
	vector<vector<char> > quantum_temp;
	//初始化量子图像
	for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			for (int k = 0; k < 8; ++k) {
				pixel_temp.push_back(QuantumState[0]);        
			}
			quantum_temp.push_back(pixel_temp);
			pixel_temp.clear();
		}
		QuantumImageOne.push_back(quantum_temp);
        QuantumImageTwo.push_back(quantum_temp);
        QuantumImageReverse.push_back(quantum_temp);
        QuantumImageComplement.push_back(quantum_temp);
        QuantumSubtracterResults.push_back(quantum_temp);
        QuantumSubtracterCarry.push_back(quantum_temp);
		quantum_temp.clear();
	}
    //单位量子图像
    char value = 1;
    vector<char> unit_temp;
	vector<vector<char> > quantum_unit;
	for (int k = 0; k < 8; ++k) {
		if ((0x80 & value) == 0x80) {
			unit_temp.push_back(QuantumState[1]);          //赋值量子基态1
		}
		else {
			unit_temp.push_back(QuantumState[0]);          //赋值量子基态0
		}
		value = value << 1;
	}
    //经典图像转换成量子图像
	for (int m = 0; m < Height; ++m) {
		for (int n = 0; n < Width; ++n) {
			quantum_unit.push_back(unit_temp);
		}
		QuantumUnitImage.push_back(quantum_unit);
		quantum_unit.clear();
	}
    //初始化溢出位
    vector<char> overflow_temp;
	for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			overflow_temp.push_back(QuantumState[0]);
		}
        QuantumSubtracterOverFlow.push_back(overflow_temp);
        overflow_temp.clear();
	}

	vector<char>().swap(pixel_temp);
    vector<char>().swap(unit_temp);
    vector<char>().swap(overflow_temp);
	vector<vector<char> >().swap(quantum_temp);
	vector<vector<char> >().swap(quantum_unit);
}
/********************************************************************************
****函数名：Reverse_Operator_parallel_accumulate()
****说  明：量子取反多线程并行计算
****参  数：
****vector<vector<vector<char> > >& QuantumImageReverse: 量子图像取反码
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug：是否显示
****返回值：无
********************************************************************************/
template<typename Iterator>
void Quantum_Subtracter::Reverse_Operator_parallel_accumulate(vector<vector<vector<char> > >& QuantumImageReverse, Iterator first, Iterator last, Iterator colums, bool Debug) {
    unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) {//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Quantum_Reverse_accumulate_block(), block_start, block_end, colums, std::ref(QuantumImageReverse), Debug);//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Quantum_Reverse_accumulate_block()(block_start, last, colums, QuantumImageReverse, Debug);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
    if (Debug) {

    }
}
/********************************************************************************
****函数名：Complement_Operator_parallel_accumulate()
****说  明：;量子取补多线程并行计算
****参  数：
****const vector<vector<vector<char> > >& QuantumImage: 量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug：是否显示
****返回值：无
********************************************************************************/
template<typename Iterator>
void Quantum_Subtracter::Complement_Operator_parallel_accumulate(const vector<vector<vector<char> > >& QuantumImage, Iterator first, Iterator last, Iterator colums, bool Debug) {
    unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) {//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Quantum_Complement_accumulate_block(), std::ref(QuantumImage), std::ref(QuantumUnitImage), block_start, block_end, colums, 
                                std::ref(QuantumImageComplement), std::ref(QuantumSubtracterCarry), std::ref(QuantumSubtracterOverFlow), Debug);//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Quantum_Complement_accumulate_block()(QuantumImage, QuantumUnitImage, block_start, last, colums, 
                                        QuantumImageComplement, QuantumSubtracterCarry, QuantumSubtracterOverFlow, Debug);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
}
/********************************************************************************
****函数名：Subtracter_Operator_parallel_accumulate()
****说  明：量子减法器多线程并行计算
****参  数：
****const vector<vector<vector<char> > >& QuantumImageOne: 第一张量子图像
****vector<vector<vector<char> > >& QuantumImageTwo: 第二张量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug：是否显示
****返回值：无
********************************************************************************/
template<typename Iterator>
void Quantum_Subtracter::Subtracter_Operator_parallel_accumulate(const vector<vector<vector<char> > >& QuantumImageOne, vector<vector<vector<char> > >& QuantumImageTwo, Iterator first, Iterator last, Iterator colums, bool Debug) {
	//Reset_Image_Reverse();  //清空反码
    //Reset_Image_Complement();//清空补码
    Reset_Vector();          //清空结果和标志
    QuantumImageReverse = QuantumImageTwo;
    Reverse_Operator_parallel_accumulate(QuantumImageReverse, first, last, colums, false); //反码操作
    Complement_Operator_parallel_accumulate(QuantumImageReverse, first, last, colums, false);//补码操作
	Reset_Vector();          //清空结果和标志
    unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) {//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Quantum_Subtracter_accumulate_block(), std::ref(QuantumImageOne), std::ref(QuantumImageComplement), block_start, block_end, colums, 
                                std::ref(QuantumSubtracterResults), std::ref(QuantumSubtracterCarry), std::ref(QuantumSubtracterOverFlow), Debug);//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Quantum_Subtracter_accumulate_block()(QuantumImageOne, QuantumImageComplement, block_start, last, colums, 
                                    QuantumSubtracterResults, QuantumSubtracterCarry, QuantumSubtracterOverFlow, Debug);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
}
/********************************************************************************
****函数名：QuantumImage_Adder_Operator()
****说  明：量子减法器，数组中从高到低位排序
****参  数：
****NEQR& NeqrOne: 第一张量子图像
****NEQR& NeqrTwo: 第二张量子图像
****NEQR& Result：结果值
****bool Debug：是否显示
****返回值：无
********************************************************************************/
void Quantum_Subtracter::QuantumImage_Subtracter_Operator(NEQR& NeqrOne, NEQR& NeqrTwo, NEQR& Result, bool Debug) {
    if (Debug) {
		double time;
    	double start;
		std::cout << "NEQR Quantum Image Subtracter Operator!" << std::endl;
		start = static_cast<double>(getTickCount());
		QuantumImageOne = NeqrOne.Get_Quantum_Image_Pixel();
        QuantumImageTwo = NeqrTwo.Get_Quantum_Image_Pixel();
        Subtracter_Operator_parallel_accumulate(QuantumImageOne, QuantumImageTwo, ClassicImageXaxis.begin(), ClassicImageXaxis.end(), ClassicImageYaxis.end(), false);//减法操作
		Result.Set_Quantum_Image_Pixel(QuantumSubtracterResults);
        time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Subtracter Operator = " << time << " ms" << std::endl;
	}
	else {
        QuantumImageOne = NeqrOne.Get_Quantum_Image_Pixel();
        QuantumImageTwo = NeqrTwo.Get_Quantum_Image_Pixel();
        Subtracter_Operator_parallel_accumulate(QuantumImageOne, QuantumImageTwo, ClassicImageXaxis.begin(), ClassicImageXaxis.end(), ClassicImageYaxis.end(), false);//减法操作
		Result.Set_Quantum_Image_Pixel(QuantumSubtracterResults);
    }
}
/********************************************************************************
****函数名：Reset_Image_Reverse()
****说  明：清空反码容器
****参  数：无
****返回值：无
********************************************************************************/
void Quantum_Subtracter::Reset_Image_Reverse(void) {
    for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			for (int k = 0; k < 8; ++k) {
				QuantumImageReverse[i][j][k] = QuantumState[0];   
			}
		}
	}
}
/********************************************************************************
****函数名：Reset_Image_Complement()
****说  明：清空补码容器
****参  数：无
****返回值：无
********************************************************************************/
void Quantum_Subtracter::Reset_Image_Complement(void) {
    for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			for (int k = 0; k < 8; ++k) {
				QuantumImageComplement[i][j][k] = QuantumState[0];   
			}
		}
	}
}
/********************************************************************************
****函数名：Reset_Vector()
****说  明：清空容器
****参  数：无
****返回值：无
********************************************************************************/
void Quantum_Subtracter::Reset_Vector(void) {
    for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			for (int k = 0; k < 8; ++k) {
				QuantumSubtracterResults[i][j][k] = QuantumState[0];
                QuantumSubtracterCarry[i][j][k] = QuantumState[0];         
			}
            QuantumSubtracterOverFlow[i][j] = QuantumState[0];   
		}
	}
}
/********************************************************************************
****函数名：size()
****说  明：获取NEQR量子图像大小
****参  数：
****NEQR &QuantumImage:量子图像
****返回值：无
********************************************************************************/
void Quantum_Subtracter::size(NEQR &QuantumImage) {
	Height = QuantumImage.Get_Image_Height();
	Width = QuantumImage.Get_Image_Width();
	QuantumNumX = QuantumImage.Get_QuantumNumX();
	QuantumNumY = QuantumImage.Get_QuantumNumY();
	//std::cout << "Height,Width= " << Height << "," << Width << std::endl;
	//std::cout << "QuantumNumX,QuantumNumY,QuantumNumTotal= " << QuantumNumX << "," << QuantumNumY << "," << QuantumNumTotal << std::endl;
}
/******************************************************************************
****函数名：Show_Results()
****说  明：显示结果
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Subtracter::Show_Results(void) {
    for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
            std::cout << "QuantumSubtracterResults[" << i << "][" << j << "] = |" << QuantumSubtracterOverFlow[i][j] << ">" <<"|";
			for (int k = 0; k < 8; ++k) {
				std::cout << QuantumSubtracterResults[i][j][k];      
			}
            std::cout << ">  ";
            std::cout << "QuantumSubtracterCarry[" << i << "][" << j << "] = |";
            for (int k = 0; k < 8; ++k) {
				std::cout << QuantumSubtracterCarry[i][j][k];      
			}
            std::cout << ">  ";
            std::cout << "QuantumSubtracterOverFlow[" << i << "][" << j << "] = " << QuantumSubtracterOverFlow[i][j] << std::endl;
		}
	}
}
/******************************************************************************
****函数名：Show_Results()
****说  明：显示结果
****参  数：
****string name：名字
****返回值：无
*******************************************************************************/
void Quantum_Subtracter::Show_Results(string name) {
    for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
            std::cout << name << "_Results[" << i << "][" << j << "] = |" << QuantumSubtracterOverFlow[i][j] << ">" << "|";
			for (int k = 0; k < 8; ++k) {
				std::cout << QuantumSubtracterResults[i][j][k];      
			}
            std::cout << ">  ";
            std::cout << name << "_Carry[" << i << "][" << j << "] = |";
            for (int k = 0; k < 8; ++k) {
				std::cout << QuantumSubtracterCarry[i][j][k];      
			}
            std::cout << ">  ";
            std::cout << name << "_OverFlow[" << i << "][" << j << "] = " << QuantumSubtracterOverFlow[i][j] << std::endl;
		}
	}
}
/******************************************************************************
****函数名：Delete()
****说  明：删除容器
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Subtracter::Delete() {
	vector<int>().swap(ClassicImageXaxis);								
	vector<int>().swap(ClassicImageYaxis);
    vector<vector<char> >().swap(QuantumSubtracterOverFlow);
    vector<vector<vector<char> > >().swap(QuantumImageOne);	
    vector<vector<vector<char> > >().swap(QuantumImageTwo);
    vector<vector<vector<char> > >().swap(QuantumUnitImage);	
    vector<vector<vector<char> > >().swap(QuantumImageReverse);		
    vector<vector<vector<char> > >().swap(QuantumImageComplement);		
    vector<vector<vector<char> > >().swap(QuantumSubtracterCarry);								
	vector<vector<vector<char> > >().swap(QuantumSubtracterResults);
}	
