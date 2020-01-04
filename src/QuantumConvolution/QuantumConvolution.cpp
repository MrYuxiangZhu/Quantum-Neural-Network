/********************************************************************************
****文件名：量子图像特征提取
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/11/19
*********************************************************************************/
#include "QuantumConvolution.hpp"

/******************************************************************************
****函数名：Convolution_accumulate_block::operator()
****说  明：卷积提取并行计算子单元
****参数：
****vector<vector<float> >& QuantumImage: 量子原图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****int thread_num：进程数
****int WindowsHeight：窗口高度
****int WindowsWidth：窗口宽度
****vector<vector<float> > & feature：结果值
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Convolution_accumulate_block::operator()(vector<vector<float> >& QuantumImage, Iterator first, Iterator last, Iterator colums, int thread_num, int WindowsHeight, int WindowsWidth, vector<vector<float> >& feature)
{
    int Height, Width;
    int length = std::distance(first, last);                   //计算序列的长度
    //std::cout << "length= " << length << std::endl;
    Height = length / WindowsHeight;                           //特征图像高度
    Width = (*(colums - 1) + 1) / WindowsWidth;                      //特征图像宽度
    //std::cout << "Height= " << Height << std::endl;
    //std::cout << "Width= " << Width << std::endl;
    int sum = 0;
    for (int row = 0; row < Height; ++row) {
        for (int col = 0; col < Width; ++col) {
            for (int i = 0; i < WindowsHeight; ++i) {
                for (int j = 0; j < WindowsWidth; ++j) {
                    sum += QuantumImage[*first + row * WindowsHeight + i][col * WindowsWidth + j];
                }
            }
            feature[thread_num * Height + row][col] = ((float)sum / (WindowsHeight * WindowsWidth));  //保存每一列特征值
            sum = 0;
        }
    }
}

/******************************************************************************
****函数名：Convolution_Operator()
****说  明：Convolution_Operator构造函数
****参  数：无
****返回值：无
*******************************************************************************/
Convolution_Operator::Convolution_Operator()
{
	hardware_threads = Obtain_Thread_Num();								//获取CPU线程数
    //std::cout << "hardware_threads= " << hardware_threads << std::endl;
}
/******************************************************************************
****函数名：~Convolution_Operator()
****说  明：Convolution_Operator析构函数
****参  数：无
****返回值：无
*******************************************************************************/
Convolution_Operator::~Convolution_Operator()
{
	Delete();
}
/******************************************************************************
****函数名：Init()
****说  明：初始化提取过程
****参  数：
****vector<vector<float> >& QuantumImage：特征矩阵
****int winheight：窗口高度
****int winwidth：窗口宽度
****返回值：无
*******************************************************************************/
void Convolution_Operator::Init(vector<vector<float> >& QuantumImage, int winheight, int winwidth)
{
    SrcHeight = QuantumImage.size();            //高度
	SrcWidth = QuantumImage[0].size();          //宽度
    SetWindowsSize(winheight, winwidth);        //设置窗口大小
    for(int i = 0; i < SrcWidth; ++i) {
        ClassicImageXaxis.push_back(i);
    }
    for(int j = 0; j < SrcHeight; ++j) {
        ClassicImageYaxis.push_back(j);
    }
    //初始化空白像素
    vector<float>img_temp;
	//初始化量子图像，测量后的
	for (int i = 0; i < SrcHeight; ++i) {
		for (int j = 0; j < SrcWidth; ++j) {
            img_temp.push_back(0);
		}
        QuantumImageSrc.push_back(img_temp);
        img_temp.clear();
	}
    vector<float> pixel_temp;
    //初始化量子图像特征容器
	for (int i = 0; i < FeatureHeight; ++i) {
		for (int j = 0; j < FeatureWidth; ++j) {
			pixel_temp.push_back(0);
		}
        FeatureImage.push_back(pixel_temp);
		pixel_temp.clear();
	}
    vector<float>().swap(pixel_temp);
    vector<float>().swap(img_temp);
}
/******************************************************************************
****函数名：SetWindowsSize()
****说  明：设置窗口大小
****参  数：
****height：窗口高度，2的倍数
****width：窗口宽度，2的倍数
****返回值：无
*******************************************************************************/
void Convolution_Operator::SetWindowsSize(int height, int width) {
    WindowsHeight = height;
    WindowsWidth = width;
    FeatureHeight = SrcHeight / WindowsHeight;
    FeatureWidth = SrcWidth / WindowsWidth;
}
/******************************************************************************
****函数名：ConvertArray()
****说  明：转换成矩阵
****参  数：
****vector<vector<float> >& QuantumImage：特征
****返回值：无
*******************************************************************************/
void Convolution_Operator::ConvertArray(vector<vector<float> >& QuantumImage) {
    for (int i = 0; i < QuantumImage.size(); ++i)
	{
		for (int j = 0; j < QuantumImage[0].size(); ++j)
		{
            if(QuantumImage[i][j] > 0) {
                QuantumImageSrc[i][j] = 1;//srcimg的第i行第j列传给二维数组数据
            }
            else {
                QuantumImageSrc[i][j] = 0;
            }
		}
	}
}
/******************************************************************************
****函数名：Convolution_accumulate()
****说  明：特征提取多线程并行计算
****参数：
****vector<vector<float> >& QuantumImage : 特征矩阵
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug	   ：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Convolution_Operator::Convolution_accumulate(Iterator first, Iterator last, Iterator colums, bool Debug) {
    unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) {//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Convolution_accumulate_block(), std::ref(QuantumImageSrc), block_start, block_end, colums, i, WindowsHeight, WindowsWidth, std::ref(FeatureImage));//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Convolution_accumulate_block()(QuantumImageSrc, block_start, last, colums, hardware_threads - 1 , WindowsHeight, WindowsWidth, FeatureImage);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
}
/******************************************************************************
****函数名：Run()
****说  明：边缘提取
****参  数：
****vector<vector<float> >& QuantumImage：图像特征矩阵
****vector<vector<float> >& feature：特征图像
****bool Debug：是否显示构建时间
****返回值：无
*******************************************************************************/
void Convolution_Operator::Run(vector<vector<float> >& QuantumImage, vector<vector<float> >& feature, bool Debug) {
    if (Debug) {
		double time;
    	double start;
		std::cout << "Quantum Image Feature Extracted!" << std::endl;
		start = static_cast<double>(getTickCount());
        Reset_Feature();                        //初始化图像容器
        ConvertArray(QuantumImage);
		Convolution_accumulate(ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);//特征提取
        feature = Get_Feature();                //获取特征图像
        time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Feature Extracted = " << time << " ms" << std::endl;
	}
	else {
        Reset_Feature();                         //初始化图像容器
        ConvertArray(QuantumImage);
		Convolution_accumulate(ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);//特征提取
        feature = Get_Feature();                //获取特征图像
    }
}
/******************************************************************************
****函数名：Run()
****说  明：边缘提取
****参  数：
****vector<vector<float> >& QuantumImage：图像特征矩阵
****vector<float>& feature：特征图像
****bool Debug：是否显示构建时间
****返回值：无
*******************************************************************************/
void Convolution_Operator::Run(vector<vector<float> >& QuantumImage, vector<float>& feature, bool Debug) {
    if (Debug) {
		double time;
    	double start;
		std::cout << "Quantum Image Feature Extracted!" << std::endl;
		start = static_cast<double>(getTickCount());
        Reset_Feature();                        //初始化图像容器
        ConvertArray(QuantumImage);
		Convolution_accumulate(ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);//特征提取
        feature = Get_Dimension_Feature();                //获取特征图像
        time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Feature Extracted = " << time << " ms" << std::endl;
	}
	else {
        Reset_Feature();                         //初始化图像容器
        ConvertArray(QuantumImage);
		Convolution_accumulate(ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);//特征提取
        feature = Get_Dimension_Feature();                //获取特征图像
    }
}
/******************************************************************************
****函数名：ShowFeatureImage()
****说  明：显示特征图像
****参  数：无
****返回值：无
*******************************************************************************/
void Convolution_Operator::ShowFeatureImage(void) {
    for (int i = 0; i < FeatureHeight; i++) {
        for (int j = 0; j< FeatureWidth; j++) {
            std::cout << "Convolution[" << i << "][" << j << "]=" << FeatureImage[i][j] << std::endl;
        }
    }
}
/******************************************************************************
****函数名：ShowFeatureImage()
****说  明：显示特征图像
****参  数：name 
****返回值：无
*******************************************************************************/
void Convolution_Operator::ShowFeatureImage(string name) {
    for (int i = 0; i < FeatureHeight; i++) {
        for (int j = 0; j< FeatureWidth; j++) {
            std::cout << name << "[" << i << "][" << j << "]=" << FeatureImage[i][j] << std::endl;
        }
    }
}
/******************************************************************************
****函数名：Reset_Feature()
****说  明：初始化边缘图像
****参  数：无
****返回值：无
*******************************************************************************/
void Convolution_Operator::Reset_Feature(void) {
    for (int i = 0; i < SrcHeight; ++i)
	{
		for (int j = 0; j < SrcWidth; ++j)
		{
            QuantumImageSrc[i][j] = 0;
		}
	}
    for (int i = 0; i < FeatureHeight; ++i)
	{
		for (int j = 0; j < FeatureWidth; ++j)
		{
            FeatureImage[i][j] = 0;
		}
	}
}
/******************************************************************************
****函数名：Get_Dimension_Feature()
****说  明：获取一维特征图像
****参  数：无
****返回值：无
*******************************************************************************/
vector<float> Convolution_Operator::Get_Dimension_Feature(void) {
    vector<float> output;
    for(int i = 0; i < FeatureImage.size(); ++i) {
        for(int j = 0; j < FeatureImage[0].size(); ++j) {
            output.push_back(FeatureImage[i][j]);
        }
    }
    return output;
}
/******************************************************************************
****函数名：Delete()
****说  明：删除量子图像内存
****参  数：无
****返回值：无
*******************************************************************************/
void Convolution_Operator::Delete() {
	vector<int>().swap(ClassicImageXaxis);
	vector<int>().swap(ClassicImageYaxis);
    vector<vector<float> >().swap(FeatureImage);
    vector<vector<float> >().swap(QuantumImageSrc);
}