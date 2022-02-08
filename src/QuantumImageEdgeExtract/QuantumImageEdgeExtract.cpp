/********************************************************************************
****文件名：量子图像边缘检测过程
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/12/24
*********************************************************************************/

#include "QuantumImageEdgeExtract.hpp"

/******************************************************************************
****函数名：Landscape_Edge_Extract_accumulate_block::operator()
****说  明：量子图像边缘横向检测并行计算子单元
****参数：
****const vector<vector<unsigned char> >& QuantumImage: 量子原图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****vector<vector<unsigned char> >& result：结果值
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Landscape_Edge_Extract_accumulate_block::operator()(const vector<vector<unsigned char>>& QuantumImage, Iterator first, Iterator last, Iterator colums, vector<vector<unsigned char>>& extract) 
{
    for (int i = *first; i <= *(last - 1); ++i)
	{
        for (int j = 1; j < *(colums - 1); ++j)
        {
            if (QuantumImage[j - 1][i] - QuantumImage[j][i] != 0 && QuantumImage[j][i] - QuantumImage[j + 1][i] == 0)
            {
                if (QuantumImage[j - 1][i] == 0 && QuantumImage[j][i] !=0 && QuantumImage[j + 1][i] !=0)
                {
                    extract[j][i] = 255;
                }
                else if (QuantumImage[j - 1][i] !=0 && QuantumImage[j][i] == 0 && QuantumImage[j + 1][i] == 0)
                {
                    extract[j - 1][i] = 255;
                }
            }
            else if (QuantumImage[j - 1][i] - QuantumImage[j][i] != 0 && QuantumImage[j][i] - QuantumImage[j + 1][i] != 0)
            {
                if (QuantumImage[j - 1][i] == 0 && QuantumImage[j][i] != 0 && QuantumImage[j + 1][i] == 0)
                {
                    extract[j][i] = 255;
                }
                else if (QuantumImage[j - 1][i] !=0 && QuantumImage[j][i] == 0 && QuantumImage[j + 1][i] !=0)
                {
                    extract[j - 1][i] = 255;
                    extract[j + 1][i] = 255;
                }
            }
            else if (QuantumImage[j - 1][i] - QuantumImage[j][i] == 0 && QuantumImage[j][i] - QuantumImage[j + 1][i] != 0)
            {
                if (QuantumImage[j - 1][i] == 0 && QuantumImage[j][i] == 0 && QuantumImage[j + 1][i] != 0)
                {
                    extract[j + 1][i] = 255;
                }
                else if (QuantumImage[j - 1][i] != 0 && QuantumImage[j][i] != 0 && QuantumImage[j + 1][i] == 0)
                {
                    extract[j][i] = 255;
                }
            }
        }
    }
}

/******************************************************************************
****函数名：Portrait_Edge_Extract_accumulate_block::operator()
****说  明：量子图像边缘纵向检测并行计算子单元
****参数：
****const vector<vector<unsigned char> >& QuantumImage: 量子原图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****vector<vector<unsigned char> >& result：结果值
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Portrait_Edge_Extract_accumulate_block::operator()(const vector<vector<unsigned char>>& QuantumImage, Iterator first, Iterator last, Iterator colums, vector<vector<unsigned char>> & extract) 
{
    for (int i = *first; i <= *(last - 1); ++i)
	{
        for (int j = 1; j < *(colums - 1); ++j)
        {
            if (QuantumImage[i][j - 1] - QuantumImage[i][j] != 0 && QuantumImage[i][j] - QuantumImage[i][j + 1] == 0)
            {
                if (QuantumImage[i][j - 1] == 0 && QuantumImage[i][j] !=0 && QuantumImage[i][j + 1] !=0)
                {
                    extract[i][j] = 255;
                }
                else if (QuantumImage[i][j - 1] !=0 && QuantumImage[i][j] == 0 && QuantumImage[i][j + 1] == 0)
                {
                    extract[i][j - 1] = 255;
                }
            }
            else if (QuantumImage[i][j - 1] - QuantumImage[i][j] != 0 && QuantumImage[i][j] - QuantumImage[i][j + 1] != 0)
            {
                if (QuantumImage[i][j - 1] == 0 && QuantumImage[i][j] != 0 && QuantumImage[i][j + 1] == 0)
                {
                    extract[i][j] = 255;
                }
                else if (QuantumImage[i][j - 1] !=0 && QuantumImage[i][j] == 0 && QuantumImage[i][j + 1] !=0)
                {
                    extract[i][j - 1] = 255;
                    extract[i][j + 1] = 255;
                }
            }
            else if (QuantumImage[i][j - 1] - QuantumImage[i][j] == 0 && QuantumImage[i][j] - QuantumImage[i][j + 1] != 0)
            {
                if (QuantumImage[i][j - 1] == 0 && QuantumImage[i][j] == 0 && QuantumImage[i][j + 1] != 0)
                {
                    extract[i][j + 1] = 255;
                }
                else if (QuantumImage[i][j - 1] != 0 && QuantumImage[i][j] != 0 && QuantumImage[i][j + 1] == 0)
                {
                    extract[i][j] = 255;
                }
            }
        }
    }
}

/******************************************************************************
****函数名：Quantum_Image_Edge_Extract()
****说  明：Quantum_Image_Edge_Extract构造函数
****参  数：无
****返回值：无
*******************************************************************************/
Quantum_Image_Edge_Extract::Quantum_Image_Edge_Extract(void) 
{
    hardware_threads = Obtain_Thread_Num();								//获取CPU线程数
	//std::cout << dec << "Quantum_Image_Edge_Extract_hardware_threads= " << hardware_threads << std::endl;
}

/******************************************************************************
****函数名：~Quantum_Image_Edge_Extract()
****说  明：Quantum_Image_Edge_Extract析构函数
****参  数：无
****返回值：无
*******************************************************************************/
Quantum_Image_Edge_Extract::~Quantum_Image_Edge_Extract(void)
{
    Delete();
}

/******************************************************************************
****函数名：Quantum_Image_Edge_Extract_Init()
****说  明：初始化提取过程
****参  数：
****cv::Mat& srcimg：原图像
****返回值：无
*******************************************************************************/
void Quantum_Image_Edge_Extract::Quantum_Image_Edge_Extract_Init(cv::Mat& srcimg) 
{
    Height = srcimg.rows;
	Width = srcimg.cols;
    for(int i = 0; i < Width; ++i) 
    {
        ClassicImageXaxis.emplace_back(i);
    }
    for(int j = 0; j < Height; ++j) 
    {
        ClassicImageYaxis.emplace_back(j);
    }
    //初始化空白像素
	vector<unsigned char> pixel_temp;
	//初始化量子图像，测量后的
	for (int i = 0; i < Height; ++i) 
    {
		for (int j = 0; j < Width; ++j) 
        {
			pixel_temp.emplace_back(0);
		}
        QuantumImageSrc.emplace_back(pixel_temp);
        ExtractedImage.emplace_back(pixel_temp);
		pixel_temp.clear();
	}
    vector<unsigned char>().swap(pixel_temp);
    EdgeImage = cv::Mat::zeros(srcimg.size(),CV_8UC1);           //初始化二值化图像 
}

/******************************************************************************
****函数名：ConvertArray()
****说  明：转换成矩阵
****参  数：
****const Mat& srcimg：二值化后图像
****返回值：无
*******************************************************************************/
void Quantum_Image_Edge_Extract::ConvertArray(cv::Mat& srcimg) 
{
    for (int i = 0; i < srcimg.rows; ++i)
	{
		unsigned char* ptmp = srcimg.ptr<unsigned char>(i);//指针指向srcimg的第i行
		for (int j = 0; j < srcimg.cols; ++j)
		{
			 QuantumImageSrc[i][j] = ptmp[j];//srcimg的第i行第j列传给二维数组数据
		}
	}
}

/******************************************************************************
****函数名：Landscape_Edge_Extract_parallel_accumulate()
****说  明：量子图像边缘横向检测多线程并行计算
****参数：
****const vector<vector<unsigned char> >& QuantumImageSrc : 原图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug	   ：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Quantum_Image_Edge_Extract::Landscape_Edge_Extract_parallel_accumulate(const vector<vector<unsigned char>>& QuantumImageSrc, Iterator first, Iterator last, Iterator colums, bool Debug) 
{
    unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) {//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Landscape_Edge_Extract_accumulate_block(), std::ref(QuantumImageSrc), block_start, block_end, colums, std::ref(ExtractedImage));//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Landscape_Edge_Extract_accumulate_block()(QuantumImageSrc, block_start, last, colums, ExtractedImage);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
}

/******************************************************************************
****函数名：Portrait_Edge_Extract_parallel_accumulate()
****说  明：量子图像边缘纵向检测多线程并行计算
****参数：
****const vector<vector<unsigned char> >& QuantumImageSrc : 原图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug	   ：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Quantum_Image_Edge_Extract::Portrait_Edge_Extract_parallel_accumulate(const vector<vector<unsigned char>>& QuantumImageSrc, Iterator first, Iterator last, Iterator colums, bool Debug) 
{
    unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) {//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Portrait_Edge_Extract_accumulate_block(), std::ref(QuantumImageSrc), block_start, block_end, colums, std::ref(ExtractedImage));//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Portrait_Edge_Extract_accumulate_block()(QuantumImageSrc, block_start, last, colums, ExtractedImage);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
}

/******************************************************************************
****函数名：Image_Edge_Extract()
****说  明：边缘提取
****参  数：
****cv::Mat& srcimg：原量子图像
****cv::Mat& edge:  边缘图像
****bool Debug：是否显示构建时间
****返回值：无
*******************************************************************************/
void Quantum_Image_Edge_Extract::Image_Edge_Extract(cv::Mat& srcimg, cv::Mat& edge, bool Debug) 
{
	if (Debug) 
    {
		double time;
    	double start;
		std::cout << "Quantum Image Edge Extracted!" << std::endl;
		start = static_cast<double>(getTickCount());
        Reset_Edge();                           //初始化图像容器
        ConvertArray(srcimg);                   //源图像转换成矩阵
		Landscape_Edge_Extract_parallel_accumulate(QuantumImageSrc, ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);//横向扫描
		Portrait_Edge_Extract_parallel_accumulate(QuantumImageSrc, ClassicImageXaxis.begin(), ClassicImageXaxis.end(), ClassicImageYaxis.end(), false);//纵向扫描
        edge = Get_Edges();                     //获取边缘图像
        time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Edge Extracted = " << time << " ms" << std::endl;
	}
	else 
    {
        Reset_Edge();                           //初始化图像容器
        ConvertArray(srcimg);                   //源图像转换成矩阵
		Landscape_Edge_Extract_parallel_accumulate(QuantumImageSrc, ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);//横向扫描
		Portrait_Edge_Extract_parallel_accumulate(QuantumImageSrc, ClassicImageXaxis.begin(), ClassicImageXaxis.end(), ClassicImageYaxis.end(), false);//纵向扫描
        edge = Get_Edges();                     //获取边缘图像
    }
}

/******************************************************************************
****函数名：Reset_Edge()
****说  明：初始化边缘图像
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Image_Edge_Extract::Reset_Edge(void) 
{
    for (int i = 0; i < Height; ++i)
	{
		unsigned char* ptmp = EdgeImage.ptr<unsigned char>(i);//指针指向EdgeImage的第i行
		for (int j = 0; j < Width; ++j)
		{
            QuantumImageSrc[i][j] = 0;
            ExtractedImage[i][j] = 0;
			ptmp[j] = 0;
		}
	}
}

/******************************************************************************
****函数名：Get_Edges()
****说  明：转换成图像
****参  数：
****Mat& srcimg：二值化后图像
****返回值：cv::Mat
*******************************************************************************/
cv::Mat Quantum_Image_Edge_Extract::Get_Edges(void) 
{
    for (int i = 0; i < Height; ++i)
	{
		unsigned char* ptmp = EdgeImage.ptr<unsigned char>(i);//指针指向EdgeImage的第i行
		for (int j = 0; j < Width; ++j)
		{
			ptmp[j] = ExtractedImage[i][j];//二维数组数据传给srcimg的第i行第j列
		}
	}
    return EdgeImage;
}

/******************************************************************************
****函数名：Get_Qedge()
****说  明：获取量子边缘图像
****参  数：无
****返回值：NEQR
*******************************************************************************/
NEQR Quantum_Image_Edge_Extract::Get_Qedge(void) 
{
    NEQR temp;
    cv::Mat edges = Get_Edges();
    temp.Constructor(edges, false);
    return temp;
}

/******************************************************************************
****函数名：ShowFeatureImage()
****说  明：显示边缘图像
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Image_Edge_Extract::Show_Edge(void) 
{
    for (int i = 0; i < Height; i++) 
    {
        for (int j = 0; j< Width; j++) 
        {
            std::cout << "EdgeImage[" << i << "][" << j << "]=" << (int)ExtractedImage[i][j] << std::endl;
        }
    }
}

/******************************************************************************
****函数名：ShowFeatureImage()
****说  明：显示边缘图像
****参  数：name 
****返回值：无
*******************************************************************************/
void Quantum_Image_Edge_Extract::Show_Edge(string name) 
{
    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j< Width; j++) 
        {
            std::cout << name << "[" << i << "][" << j << "]=" << (int)ExtractedImage[i][j] << std::endl;
        }
    }
}

/******************************************************************************
****函数名：Delete()
****说  明：删除量子图像内存
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Image_Edge_Extract::Delete() 
{
	vector<int>().swap(ClassicImageXaxis);
	vector<int>().swap(ClassicImageYaxis);
    vector<vector<unsigned char> >().swap(ExtractedImage);
	vector<vector<unsigned char> >().swap(QuantumImageSrc);
}