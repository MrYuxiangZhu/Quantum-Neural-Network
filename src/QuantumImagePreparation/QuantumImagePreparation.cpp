/********************************************************************************
****文件名：量子图像构建过程
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/12/8
*********************************************************************************/

#include "QuantumImagePreparation.hpp"

/******************************************************************************
****函数名：HardWare()
****说  明：HardWare构造函数
****参  数：无
****返回值：无
*******************************************************************************/
HardWare::HardWare()
{

}

/******************************************************************************
****函数名：~HardWare()
****说  明：HardWare析构函数
****参  数：无
****返回值：无
*******************************************************************************/
HardWare::~HardWare()
{

}

/******************************************************************************
****函数名：Obtain_Thread_Num()
****说  明：获取CPU线程数
****参  数：无
****返回值：hardware_threads
*******************************************************************************/
int HardWare::Obtain_Thread_Num(void) 
{
    hardware_threads = std::thread::hardware_concurrency();//获取PC的CPU core数目，C++库可能无法访问该信息，所以可能返回0
    for (int p = 0;; ++p) 
	{
        if (pow(2, p) == hardware_threads) 
		{
            break;
        }
        if (pow(2, p) > hardware_threads) 
		{
            hardware_threads = pow(2, p - 1);
            break;
        }
    }
	return hardware_threads / THREAD_TIME;
}

/******************************************************************************
****函数名：NEQR_Image_Constructor_accumulate_block::operator()
****说  明：NEQR量子图像构建子单元
****参数：
****const cv::Mat& ClassicImage : 原图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****vector<vector<vector<char> > >& result：结果值
****返回值：无
*******************************************************************************/
template<typename Iterator>
void NEQR_Image_Constructor_accumulate_block::operator()(const cv::Mat& ClassicImage, Iterator first, Iterator last, Iterator colums, vector<vector<vector<char>>>& result) 
{
    char pixeltemp = 0;
    //经典图像转换成量子图像
	for (int m = *first; m <= *(last - 1); ++m) 
	{
		const unsigned char* indata = ClassicImage.ptr<unsigned char>(m);
		for (int n = 0; n <= *(colums - 1); ++n) 
		{
			pixeltemp = indata[n];                         		//暂存（Y，X）位置像素
			for (int k = 0; k < 8; ++k) 
			{
				if ((0x80 & pixeltemp) == 0x80) 
				{
					result[m][n][k] = QuantumState[1];          //赋值量子基态1
				}
				else 
				{
					result[m][n][k] = QuantumState[0];          //赋值量子基态0
				}
				pixeltemp = pixeltemp << 1;
			}
		}
	}
}

/******************************************************************************
****函数名：NEQR()
****说  明：NEQR构造函数
****参  数：无
****返回值：无
*******************************************************************************/
NEQR::NEQR()
{
	hardware_threads = Obtain_Thread_Num();								//获取CPU线程数
	//std::cout << dec << "NEQR_hardware_threads= " << hardware_threads << std::endl;
}

/******************************************************************************
****函数名：~NEQR()
****说  明：NEQR析构函数
****参  数：无
****返回值：无
*******************************************************************************/
NEQR::~NEQR()
{
	Delete_QuantumImages();								//删除容器内存
}

/******************************************************************************
****函数名：Resize_Classic_Image()
****说  明：重设图像大小
****参  数：
****cv::Mat &ClassicImage：原图像
****int code：一个颜色空间转换到另一个颜色空间，CV_RGB2GRAY灰度化
****bool Debug：是否调试打印
****返回值：重设后的灰度图像
*******************************************************************************/
cv::Mat NEQR::Resize_Classic_Image(cv::Mat& ClassicImage, int code, bool Debug) 
{
	cv::Mat GrayImage;									//经典灰度图像
	cv::cvtColor(ClassicImage, GrayImage, code);	//灰度化
    cv::Mat ResizeImage(dsize, GrayImage.type());
    cv::resize(GrayImage, ResizeImage, ResizeImage.size(), 0, 0, CV_INTER_LINEAR);
	if (Debug) 
	{
		cv::imshow("ResizeImage",ResizeImage);				//显示重设后的图像
	}
	return ResizeImage;
}

/******************************************************************************
****函数名：Resize_Classic_Image()
****说  明：重设图像大小
****参  数：
****cv::Mat &ClassicImage：原图像
****int Height：预设图像高度
****int Width：预设图像宽度
****int code：一个颜色空间转换到另一个颜色空间，CV_RGB2GRAY灰度化
****bool Debug：是否调试打印
****返回值：重设后的灰度图像
*******************************************************************************/
cv::Mat NEQR::Resize_Classic_Image(cv::Mat& ClassicImage, int Height, int Width, int code, bool Debug) 
{
	cv::Mat GrayImage;								//经典灰度图像
	cv::cvtColor(ClassicImage, GrayImage, code);	//灰度化
	cv::Size dsize = Size(Height, Width);			//重设图像大小
    cv::Mat ResizeImage(dsize, GrayImage.type());
    cv::resize(GrayImage, ResizeImage, ResizeImage.size(), 0, 0, CV_INTER_LINEAR);
	if (Debug) 
	{
		cv::imshow("ResizeImage",ResizeImage);		//显示重设后的图像
	}
	return ResizeImage;
}

/******************************************************************************
****函数名：Image_Measure()
****说  明：测量经典图像
****参  数：
****const cv::Mat &ClassicImage：原图像
****返回值：无
*******************************************************************************/
void NEQR::Image_Measure(const cv::Mat& ClassicImage) 
{
	Height = ClassicImage.rows;               			//图片的高度
    Width = ClassicImage.cols;             				//图片的宽度
	//std::cout << "NEQR(Height, Width)= " << Height << "," << Width << std::endl;
    for(int i = 0; i < Width; ++i) 
	{
        ClassicImageXaxis.emplace_back(i);
    }
    for(int j = 0; j < Height; ++j) 
	{
        ClassicImageYaxis.emplace_back(j);
    }
	//计算X轴量子数量
    for (int i = 0;; ++i) 
	{                
        if (pow(2, i) == Width) 
		{
            QuantumNumX = i;
            break;
        }
    }
	//计算Y轴量子数量
    for (int j = 0;; ++j) 
	{
        if (pow(2, j) == Height) 
		{
            QuantumNumY = j;
            break;
        }
    }
}

/******************************************************************************
****函数名：Quantum_Image_Init()
****说  明：初始化NEQR量子图像
****参  数：
****const cv::Mat &ClassicImage：原图像
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
void NEQR::Quantum_Image_Init(const cv::Mat& ClassicImage, bool Debug) 
{
	vector<char> temp;
    int count;
	//double time;
	//double start;
	//start = static_cast<double>(getTickCount());
	
    //保存X轴量子坐标
	for (int j = 0; j < pow(2, QuantumNumX); ++j) 
	{
		count = j;
		for (int k = 0; k < QuantumNumX; ++k) 
		{
			int xaxis = pow(2, QuantumNumX - 1);
			if ((xaxis & count) == xaxis) 
			{
				temp.emplace_back(QuantumState[1]);            //赋值量子基态1
			}
			else 
			{
				temp.emplace_back(QuantumState[0]);             //赋值量子基态0
			}
			count = count << 1;
		}
		if (Debug) 
		{
			std::cout << "QuantumX[" << j << "]=|";
			for (int n = 0; n < QuantumNumX; ++n) 
			{
				std::cout << temp[n];
			}
			std::cout << ">" << std::endl;
		}
		QuantumX.emplace_back(temp);
		temp.clear();
	}

    temp.clear();
    //保存Y轴量子坐标
	for (int j = 0; j < pow(2, QuantumNumY); ++j) 
	{
		count = j;
		for (int k = 0; k < QuantumNumY; ++k) 
		{
			int yaxis = pow(2, QuantumNumY - 1);
			if ((yaxis & count) == yaxis) 
			{
				temp.emplace_back(QuantumState[1]);            //赋值量子基态1
			}
			else 
			{
				temp.emplace_back(QuantumState[0]);             //赋值量子基态0
			}
			count = count << 1;
		}
		if (Debug) 
		{
			std::cout << "QuantumY[" << j << "]=|";
			for (int n = 0; n < QuantumNumY; ++n) 
			{
				std::cout << temp[n];
			}
			std::cout << ">" << std::endl;
		}
		QuantumY.emplace_back(temp);
		temp.clear();
	}
	//初始化空白像素
	vector<char> pixel_temp;
	vector<vector<char>> quantum_temp;
	//初始化量子图像
	for (int i = 0; i < pow(2, QuantumNumX); ++i) 
	{
		for (int j = 0; j < pow(2, QuantumNumY); ++j) 
		{
			for (int k = 0; k < 8; ++k) 
			{
				pixel_temp.emplace_back(QuantumState[0]);            			//赋值量子基态0
			}
			quantum_temp.emplace_back(pixel_temp);
			pixel_temp.clear();
		}
		QuantumImage.emplace_back(quantum_temp);
		quantum_temp.clear();
	}
	if (Debug) 
	{
		Show_QuantumImage();
	}
	vector<char>().swap(temp);
	vector<char>().swap(pixel_temp);
	vector<vector<char>>().swap(quantum_temp);
	//time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
	//std::cout << "Time of NEQR Image Init = " << time << " ms" << std::endl;
}

/******************************************************************************
****函数名：Process_First_Frame()
****说  明：第一帧判断
****参  数：无
****bool First：是否第一帧
****返回值：无
*******************************************************************************/
bool NEQR::Process_First_Frame(bool First) 
{
	return First;
}	

/******************************************************************************
****函数名：NEQR_Image_Constructor_parallel_accumulate()
****说  明：NEQR量子图像构建多线程并行计算
****参数：
****const cv::Mat& ClassicImage : 原图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug	   ：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void NEQR::NEQR_Image_Constructor_parallel_accumulate(const cv::Mat& ClassicImage, Iterator first, Iterator last, Iterator colums, bool Debug) 
{//创建NEQR图像
	unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) 
	{//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (NEQR_Image_Constructor_accumulate_block(), std::ref(ClassicImage), block_start, block_end, colums, std::ref(QuantumImage));//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    NEQR_Image_Constructor_accumulate_block()(ClassicImage, block_start, last, colums, QuantumImage);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
	if (Debug) 
	{
		Show_QuantumImage();												//显示NEQR量子图像
		Show_ClassicImage(ClassicImage);
	}
}

/******************************************************************************
****函数名：Constructor()
****说  明：构建NEQR量子图像
****参  数：
****const cv::Mat &ClassicImage：经典图像
****bool Debug：是否显示构建时间
****返回值：无
*******************************************************************************/
void NEQR::Constructor(const cv::Mat& ClassicImage, bool Debug) 
{
	if (Debug) 
	{
		double time;
    	double start;
		std::cout << "NEQR Quantum Image Constructor!" << std::endl;
		start = static_cast<double>(getTickCount());
		NEQR_Image_Constructor_parallel_accumulate(ClassicImage, ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);
		time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Create Threads = " << time << " ms" << std::endl;
	}
	else 
	{
		NEQR_Image_Constructor_parallel_accumulate(ClassicImage, ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);
	}
}

/******************************************************************************
****函数名：Show_QuantumImage()
****说  明：显示NEQR量子图像
****参  数：无
****返回值：无
*******************************************************************************/
void NEQR::Show_QuantumImage(void) 
{
	std::cout << "Show NEQR Quantum Image Constructor!" << std::endl;
	//显示量子图像
	for (int i = 0; i < pow(2, QuantumNumX); ++i) 
	{
		for (int j = 0; j < pow(2, QuantumNumY); ++j) 
		{
			//显示像素值
			std::cout << "NEQR_QuantumImage[" << i << "][" << j << "]=|";
			for (int k = 0; k < 8; ++k) 
			{
				std::cout << QuantumImage[i][j][k];
			}
			std::cout << ">";
			//显示X轴坐标
			std::cout << "|";
			for (int cnt = 0; cnt < QuantumNumX; ++cnt) 
			{
				std::cout << QuantumX[i][cnt];
			}
			std::cout << ">";
			//显示X轴坐标
			std::cout << "|";
			for (int cnt = 0; cnt < QuantumNumY; ++cnt) 
			{
				std::cout << QuantumY[j][cnt];
			}
			std::cout << ">" << std::endl;
		}	
	}
}

/******************************************************************************
****函数名：Show_QuantumImage()
****说  明：显示NEQR量子图像
****参  数：
****string name：名字
****返回值：无
*******************************************************************************/
void NEQR::Show_QuantumImage(string name) 
{
	std::cout << "Show NEQR Quantum Image Constructor!" << std::endl;
	//显示量子图像
	for (int i = 0; i < pow(2, QuantumNumX); ++i) 
	{
		for (int j = 0; j < pow(2, QuantumNumY); ++j) 
		{
			//显示像素值
			std::cout << name <<"[" << i << "][" << j << "]=|";
			for (int k = 0; k < 8; ++k) 
			{
				std::cout << QuantumImage[i][j][k];
			}
			std::cout << ">";
			//显示X轴坐标
			std::cout << "|";
			for (int cnt = 0; cnt < QuantumNumX; ++cnt)
			{
				std::cout << QuantumX[i][cnt];
			}
			std::cout << ">";
			//显示X轴坐标
			std::cout << "|";
			for (int cnt = 0; cnt < QuantumNumY; ++cnt) 
			{
				std::cout << QuantumY[j][cnt];
			}
			std::cout << ">" << std::endl;
		}	
	}
}

/******************************************************************************
****函数名：Show_ClassicImage()
****说  明：显示经典图像
****参  数：const cv::Mat& ClassicImage
****返回值：无
*******************************************************************************/
void NEQR::Show_ClassicImage(const cv::Mat& ClassicImage) 
{
	for (int m = 0; m < Height; ++m) 
	{
		const unsigned char* indata = ClassicImage.ptr<unsigned char>(m);
		for (int n = 0; n < Width; ++n) 
		{
			std::cout << "ClassicImage[" << m << "][" << n << "]= " << (int)indata[n] << std::endl;          //暂存（Y，X）位置像素
		}
	}
}

/******************************************************************************
****函数名：Show_ClassicImage()
****说  明：显示经典图像
****参  数：
****const cv::Mat& ClassicImage：经典图像
****string name：名字
****返回值：无
*******************************************************************************/
void NEQR::Show_ClassicImage(const cv::Mat& ClassicImage, string name) 
{
	for (int m = 0; m < Height; ++m) 
	{
		const unsigned char* indata = ClassicImage.ptr<unsigned char>(m);
		for (int n = 0; n < Width; ++n) 
		{
			std::cout << name <<"[" << m << "][" << n << "]= " << (int)indata[n] << std::endl;  //暂存（Y，X）位置像素
		}
	}
}

/******************************************************************************
****函数名：size()
****说  明：获取NEQR量子图像大小
****参  数：无
****返回值：vector<int> 图像长和宽
*******************************************************************************/
vector<int> NEQR::size(void) 
{
	vector<int> temp;
	temp.emplace_back(Height);
	temp.emplace_back(Width);
	return temp;
}

/******************************************************************************
****函数名：Clone()
****说  明：NEQR量子图像克隆
****参  数：无
****返回值：NEQR量子图像
*******************************************************************************/
NEQR NEQR::Clone(void) 
{
	NEQR temp;
	temp.QuantumNumTotal = QuantumNumTotal;				//总量子数量
	temp.QuantumNumX = QuantumNumX;						//X轴量子数量
	temp.QuantumNumY = QuantumNumY;						//Y轴量子数量
	temp.Height = Height;								//图像高度
	temp.Width = Width;									//图像宽度
	temp.hardware_threads = hardware_threads;			//CPU线程数
	temp.QuantumImage = QuantumImage;					//存储量子图像
	temp.QuantumX = QuantumX;							//量子图像X轴
	temp.QuantumY = QuantumY;							//量子图像Y轴
	temp.ClassicImageXaxis = ClassicImageXaxis;			//经典图像X轴
	temp.ClassicImageYaxis = ClassicImageYaxis;			//经典图像Y轴
	return temp;
}

/******************************************************************************
****函数名：Zeros()
****说  明：空白的NEQR量子图像
****参  数：无
****返回值：NEQR量子图像
*******************************************************************************/
NEQR NEQR::Zeros(void) 
{
	NEQR temp;
	vector<vector<vector<char>>>BlankQuantumImage;				
	vector<char> pixel_temp;
	vector<vector<char>> quantum_temp;
	//初始化量子图像
	for (int i = 0; i < pow(2, QuantumNumX); ++i) 
	{
		for (int j = 0; j < pow(2, QuantumNumY); ++j) 
		{
			for (int k = 0; k < 8; ++k) 
			{
				pixel_temp.emplace_back(QuantumState[0]);  //赋值量子基态0
			}
			quantum_temp.emplace_back(pixel_temp);
			pixel_temp.clear();
		}
		BlankQuantumImage.emplace_back(quantum_temp);
		quantum_temp.clear();
	}
	temp.QuantumNumTotal = QuantumNumTotal;				//总量子数量
	temp.QuantumNumX = QuantumNumX;						//X轴量子数量
	temp.QuantumNumY = QuantumNumY;						//Y轴量子数量
	temp.Height = Height;								//图像高度
	temp.Width = Width;									//图像宽度
	temp.hardware_threads = hardware_threads;			//CPU线程数
	temp.QuantumImage = BlankQuantumImage;				//存储空白量子图像
	temp.QuantumX = QuantumX;							//量子图像X轴
	temp.QuantumY = QuantumY;							//量子图像Y轴
	temp.ClassicImageXaxis = ClassicImageXaxis;			//经典图像X轴
	temp.ClassicImageYaxis = ClassicImageYaxis;			//经典图像Y轴

	vector<char>().swap(pixel_temp);
	vector<vector<char>>().swap(quantum_temp);
	vector<vector<vector<char>>>().swap(BlankQuantumImage);
	return temp;
}

/******************************************************************************
****函数名：Threshold()
****说  明：特定值的NEQR量子图像
****参  数：无
****char value：设定值
****返回值：NEQR量子图像
*******************************************************************************/		
NEQR NEQR::Threshold(char value) 
{
	NEQR temp;
	vector<char> pixel_temp;
	vector<vector<char>> quantum_temp;
	vector<vector<vector<char>>>ThresholdQuantumImage;
	for (int k = 0; k < 8; ++k) 
	{
		if ((0x80 & value) == 0x80) 
		{
			pixel_temp.emplace_back(QuantumState[1]);          //赋值量子基态1
		}
		else 
		{
			pixel_temp.emplace_back(QuantumState[0]);          //赋值量子基态0
		}
		value = value << 1;
	}
    //经典图像转换成量子图像
	for (int m = 0; m < Height; ++m) 
	{
		for (int n = 0; n < Width; ++n) 
		{
			quantum_temp.emplace_back(pixel_temp);
		}
		ThresholdQuantumImage.emplace_back(quantum_temp);
		quantum_temp.clear();
	}
	temp.QuantumNumTotal = QuantumNumTotal;				//总量子数量
	temp.QuantumNumX = QuantumNumX;						//X轴量子数量
	temp.QuantumNumY = QuantumNumY;						//Y轴量子数量
	temp.Height = Height;								//图像高度
	temp.Width = Width;									//图像宽度
	temp.hardware_threads = hardware_threads;			//CPU线程数
	temp.QuantumImage = ThresholdQuantumImage;			//存储特定值量子图像
	temp.QuantumX = QuantumX;							//量子图像X轴
	temp.QuantumY = QuantumY;							//量子图像Y轴
	temp.ClassicImageXaxis = ClassicImageXaxis;			//经典图像X轴
	temp.ClassicImageYaxis = ClassicImageYaxis;			//经典图像Y轴
	vector<char>().swap(pixel_temp);
	vector<vector<char>>().swap(quantum_temp);
	vector<vector<vector<char>>>().swap(ThresholdQuantumImage);
	return temp;
}

/******************************************************************************
****函数名：Delete_QuantumImages()
****说  明：删除NEQR量子图像内存
****参  数：无
****返回值：无
*******************************************************************************/
void NEQR::Delete_QuantumImages() 
{
	vector<int>().swap(ClassicImageXaxis);
	vector<int>().swap(ClassicImageYaxis);
	vector<vector<char>>().swap(QuantumX);
	vector<vector<char>>().swap(QuantumY);
	vector<vector<vector<char>>>().swap(QuantumImage);
}

/******************************************************************************
****函数名：Quantum_Measure_accumulate_block::operator()
****说  明：NEQR量子图像测量子单元
****参数：
****const vector<vector<vector<char> > >& QuantumImage: NEQR量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****vector<vector<unsigned char> >& ClassicImageArray：经典图像数组
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Quantum_Measure_accumulate_block::operator()(const vector<vector<vector<char>>>& QuantumImage, Iterator first, Iterator last, Iterator colums, vector<vector<unsigned char>>& ClassicImageArray) {
	unsigned char pixelvalue = 0;
	for (int m = *first; m <= *(last - 1); ++m) 
	{
		for (int n = 0; n <= *(colums - 1); ++n) 
		{
			for (int k = 0; k < 8; ++k) 
			{
				pixelvalue = pixelvalue << 1;
				if (QuantumImage[m][n][k] == QuantumState[1]) 
				{ //判断是否为|1>
					pixelvalue = pixelvalue + 1;
				}
			}
			ClassicImageArray[m][n] = pixelvalue;
			pixelvalue = 0;
		}
	}
}

/********************************************************************************
****函数名：Quantum_Measure()
****说  明：量子测量构造函数
****参  数：无
****返回值：无
********************************************************************************/
Quantum_Measure::Quantum_Measure() 
{
	hardware_threads = Obtain_Thread_Num();
	//std::cout << dec << "Quantum_Measure_hardware_threads= " << hardware_threads << std::endl;
}

/********************************************************************************
****函数名：~Quantum_Measure()
****说  明：量子测量析构函数
****参  数：无
****返回值：无
********************************************************************************/
Quantum_Measure::~Quantum_Measure() 
{
	Delete_QuantumImages();
}

/********************************************************************************
****函数名：Init()
****说  明：量子测量初始化
****参  数：
****NEQR &QuantumImage:量子图像
****返回值：无
********************************************************************************/
void Quantum_Measure::Init(NEQR& QuantumImage) 
{
	size(QuantumImage);
	vector<unsigned char> temp;
	//初始化经典图像二维数组
	//std::cout << "Quantum_Measure(Height, Width)= " << Height << "," << Width << std::endl;
	for (int row = 0; row < Height; ++row) 
	{
		for (int col = 0; col < Width; ++col) 
		{
			temp.emplace_back(0);
		}
		ClassicImageArray.emplace_back(temp);
		temp.clear();
	}
	//初始化经典图像坐标
	for(int i = 0; i < Width; ++i) 
	{
        ClassicImageXaxis.emplace_back(i);
    }
    for(int j = 0; j < Height; ++j) 
	{
        ClassicImageYaxis.emplace_back(j);
    }
	vector<char> pixel_temp;
	vector<vector<char>> quantum_temp;
	//初始化量子图像
	for (int i = 0; i < Height; ++i) 
	{
		for (int j = 0; j < Width; ++j) 
		{
			for (int k = 0; k < 8; ++k) 
			{
				pixel_temp.emplace_back(QuantumState[0]);        
			}
			quantum_temp.emplace_back(pixel_temp);
			pixel_temp.clear();
		}
		QuantumImages.emplace_back(quantum_temp);
		quantum_temp.clear();
	}
	//初始化溢出位
	vector<char> flow;
	for (int row = 0; row < Height; ++row) 
	{
		for (int col = 0; col < Width; ++col) 
		{
			flow.emplace_back(QuantumState[0]);
		}
		QuantumOverFlow.emplace_back(flow);
		flow.clear();
	}
	vector<char>().swap(flow);
	vector<char>().swap(pixel_temp);
	vector<vector<char>>().swap(quantum_temp);
	vector<unsigned char>().swap(temp);
	CIReconstructor = cv::Mat::zeros(Height, Width, CV_8UC1); 			//重建图像初始化
}

/********************************************************************************
****函数名：size()
****说  明：获取NEQR量子图像大小
****参  数：
****NEQR &QuantumImage:量子图像
****返回值：无
********************************************************************************/
void Quantum_Measure::size(NEQR &QuantumImage) 
{
	Height = QuantumImage.Get_Image_Height();
	Width = QuantumImage.Get_Image_Width();
	QuantumNumX = QuantumImage.Get_QuantumNumX();
	QuantumNumY = QuantumImage.Get_QuantumNumY();
	QuantumNumTotal = QuantumImage.Get_QuantumNumTotal();
	//std::cout << "Height,Width= " << Height << "," << Width << std::endl;
	//std::cout << "QuantumNumX,QuantumNumY,QuantumNumTotal= " << QuantumNumX << "," << QuantumNumY << "," << QuantumNumTotal << std::endl;
}

/******************************************************************************
****函数名：Quantum_Measure_parallel_accumulate()
****说  明：NEQR量子图像测量多线程并行计算
****参数：
****const NEQR &neqr: NEQR量子图像
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug	   ：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void Quantum_Measure::Quantum_Measure_parallel_accumulate(NEQR& neqr, Iterator first, Iterator last, Iterator colums, bool Debug) 
{
	QuantumImages = neqr.Get_Quantum_Image_Pixel();
	unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) 
	{//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Quantum_Measure_accumulate_block(), std::ref(QuantumImages), block_start, block_end, colums, std::ref(ClassicImageArray));//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Quantum_Measure_accumulate_block()(QuantumImages, block_start, last, colums, ClassicImageArray);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成

	if (Debug) 
	{
		
	}
}

/********************************************************************************
****函数名：Measure()
****说  明：测量量子图像
****参  数：
****const NEQR &neqr: NEQR量子图像
****bool Debug	   ：是否调试打印
****返回值：无
********************************************************************************/
void Quantum_Measure::Measure(NEQR &neqr, bool Debug) 
{
	if (Debug) 
	{
		double time;
    	double start;
		std::cout << "NEQR Quantum Image Constructor!" << std::endl;
		start = static_cast<double>(getTickCount());
		Quantum_Measure_parallel_accumulate(neqr, ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);
		time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Create Threads = " << time << " ms" << std::endl;
	}
	else 
	{
		Quantum_Measure_parallel_accumulate(neqr, ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);
	}
}

/******************************************************************************
****函数名：Show_ClassicImage()
****说  明：显示经典图像
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Measure::Show_ClassicImage(void) 
{
	for (int m = 0; m < Height; ++m) 
	{
		for (int n = 0; n < Width; ++n) 
		{
			std::cout << "MeasureClassicImage[" << m << "][" << n << "]= " << (int)ClassicImageArray[m][n] << std::endl;                         		//暂存（Y，X）位置像素
		}
	}
}

/******************************************************************************
****函数名：Show_ClassicImage()
****说  明：显示经典图像
****参  数：
****string name：名字
****返回值：无
*******************************************************************************/
void Quantum_Measure::Show_ClassicImage(string name) 
{
	for (int m = 0; m < Height; ++m) 
	{
		for (int n = 0; n < Width; ++n) 
		{
			std::cout << name << "[" << m << "][" << n << "]= " << (int)ClassicImageArray[m][n] << std::endl; //暂存（Y，X）位置像素
		}
	}
}

/******************************************************************************
****函数名：Get_Classic_Image_Reconstructor()
****说  明：获得重建后的经典图像
****参  数：无
****返回值：无
*******************************************************************************/
cv::Mat Quantum_Measure::Get_Classic_Image_Reconstructor(void) 
{
	for (int i = 0; i < ClassicImageArray.size(); ++i)
	{
		uchar* ptmp = CIReconstructor.ptr<uchar>(i);//指针指向CIReconstructor的第i行
		for (int j = 0; j < ClassicImageArray[0].size(); ++j)
		{
			ptmp[j] = ClassicImageArray[i][j];//二维数组数据传给CIReconstructor的第i行第j列
		}
	}
    return CIReconstructor;
}

/******************************************************************************
****函数名：Delete_QuantumImages()
****说  明：删除容器
****参  数：无
****返回值：无
*******************************************************************************/
void Quantum_Measure::Delete_QuantumImages() 
{
	vector<int>().swap(ClassicImageXaxis);								
	vector<int>().swap(ClassicImageYaxis);								
	vector<vector<unsigned char>>().swap(ClassicImageArray);
	vector<vector<char>>().swap(QuantumOverFlow);
	vector<vector<vector<char>>>().swap(QuantumImages);
}

/******************************************************************************
****函数名：FRQI_Image_Constructor_accumulate_block::operator()
****说  明：FRQI量子图像构建子单元
****参数：
****参  数：vector<vector<float> >& Parameter：图像系数
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****vector<vector<char> >& result：结果值
****返回值：无
*******************************************************************************/
template<typename Iterator>
void FRQI_Image_Constructor_accumulate_block::operator()(const vector<vector<float>>& Parameter, Iterator first, Iterator last, Iterator colums, vector<vector<float>>& result) {
	//保存FRQI图像
	for (int row = *first; row <= *(last - 1); ++row) 
	{
		for (int col = 0; col <= *(colums - 1); ++col) 
		{
			result[row][col] = Parameter[row][col] * PI;
		}
	}
}

/******************************************************************************
****函数名：FRQI()
****说  明：FRQI构造函数
****参  数：无
****返回值：无
*******************************************************************************/
FRQI::FRQI()
{
	hardware_threads = Obtain_Thread_Num();								//获取CPU线程数
	//std::cout << dec << "FRQI_hardware_threads= " << hardware_threads << std::endl;
}

/******************************************************************************
****函数名：FRQI()
****说  明：FRQI析构函数
****参  数：无
****返回值：无
*******************************************************************************/
FRQI::~FRQI()
{
	Delete_QuantumImages();								//删除容器内存
}

/******************************************************************************
****函数名：Process_First_Frame()
****说  明：第一帧判断
****参  数：无
****bool First：是否第一帧
****返回值：无
*******************************************************************************/
bool FRQI::Process_First_Frame(bool First) 
{
	return First;
}	

/******************************************************************************
****函数名：Image_Measure()
****说  明：测量经典图像
****参  数：vector<vector<float> >& Parameter：图像系数
****bool Debug：是否调试打印
****返回值：无
*******************************************************************************/
void FRQI::Image_Measure(vector<vector<float> >& Parameter, bool Debug) 
{
	Height = Parameter.size();               			//图片的高度
    Width = Parameter[0].size();             			//图片的宽度
	//初始化图片坐标
    for (int i = 0; i < Width; ++i) 
	{
        ClassicImageXaxis.emplace_back(i);
    }
    for (int j = 0; j < Height; ++j) 
	{
        ClassicImageYaxis.emplace_back(j);
    }
	Height = Parameter.size();							//图像高度
	Width = Parameter[0].size();						//图像宽度
	if (Debug) 
	{
		std::cout << "FRQI_Height=" << Height << std::endl;
		std::cout << "FRQI_Width=" << Width << std::endl;
	}
	//计算X轴量子数量
	for (int i = 0;; ++i) 
	{
		if (pow(2, i) == Width) 
		{
			QuantumNumX = i;
			break;
		}
	}
	//计算Y轴量子数量
	for (int j = 0;; ++j) 
	{
		if (pow(2, j) == Height) 
		{
			QuantumNumY = j;
			break;
		}
	}
	vector<char> temp;
	int count;
	//保存X轴量子坐标
	for (int j = 0; j < pow(2, QuantumNumX); ++j) 
	{
		count = j;
		for (int k = 0; k < QuantumNumX; ++k) 
		{
			int xaxis = pow(2, QuantumNumX - 1);
			if ((xaxis & count) == xaxis) 
			{
				temp.emplace_back(QuantumState[1]);            //赋值量子基态1
			}
			else 
			{
				temp.emplace_back(QuantumState[0]);             //赋值量子基态0
			}
			count = count << 1;
		}
		if (Debug) 
		{
			std::cout << "QuantumX[" << j << "]=|";
			for (int n = 0; n < QuantumNumX; ++n) 
			{
				std::cout << temp[n];
			}
			std::cout << ">" << std::endl;
		}
		QuantumX.emplace_back(temp);
		temp.clear();
	}

	temp.clear();
	//保存Y轴量子坐标
	for (int j = 0; j < pow(2, QuantumNumY); ++j) 
	{
		count = j;
		for (int k = 0; k < QuantumNumY; ++k) 
		{
			int yaxis = pow(2, QuantumNumY - 1);
			if ((yaxis & count) == yaxis)
			 {
				temp.emplace_back(QuantumState[1]);            //赋值量子基态1
			}
			else 
			{
				temp.emplace_back(QuantumState[0]);             //赋值量子基态0
			}
			count = count << 1;
		}
		if (Debug) 
		{
			std::cout << "QuantumY[" << j << "]=|";
			for (int n = 0; n < QuantumNumY; ++n) 
			{
				std::cout << temp[n];
			}
			std::cout << ">" << std::endl;
		}
		QuantumY.emplace_back(temp);
		temp.clear();
	}

	vector<float> frqi_temp;
	//初始化FRQI
	for (int i = 0; i < Height; ++i) 
	{
		for (int j = 0; j < Width; ++j) 
		{
			frqi_temp.emplace_back(0);
		}
		QuantumImage.emplace_back(frqi_temp);
		frqi_temp.clear();
	}
	vector<char>().swap(temp);
	vector<float>().swap(frqi_temp);
}

/******************************************************************************
****函数名：FRQI_Image_Constructor_parallel_accumulate()
****说  明：FRQI量子图像构建多线程并行计算
****参  数：
****vector<vector<float> >& Parameter : 图像系数
****Iterator first ：图像行迭代器首地址
****Iterator last  ：图像行迭代器末地址
****Iterator colums：图像列迭代器
****bool Debug	   ：是否调试打印
****返回值：无
*******************************************************************************/
template<typename Iterator>
void FRQI::FRQI_Image_Constructor_parallel_accumulate(const vector<vector<float>>& Parameter, Iterator first, Iterator last,Iterator colums, bool Debug) 
{
	unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) 
	{//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (FRQI_Image_Constructor_accumulate_block(), std::ref(Parameter), block_start, block_end, colums, std::ref(QuantumImage));//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    FRQI_Image_Constructor_accumulate_block()(Parameter, block_start, last, colums, QuantumImage);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
	if (Debug) 
	{
		Show_QuantumImage();
	}
}

/******************************************************************************
****函数名：Constructor()
****说  明：构建FRQI量子图像
****vector<vector<float> >& Parameter : 图像系数
****bool Debug：是否显示构建时间
****返回值：无
*******************************************************************************/
void FRQI::Constructor(vector<vector<float> >& Parameter, bool Debug) 
{
	std::cout << "FRQI Quantum Image Constructor!" << std::endl;
	if (Debug) 
	{
		double time;
    	double start;
		start = static_cast<double>(getTickCount());
		FRQI_Image_Constructor_parallel_accumulate(Parameter, ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);
		time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Create Threads = " << time << " ms" << std::endl;
	}
	else 
	{
		FRQI_Image_Constructor_parallel_accumulate(Parameter, ClassicImageYaxis.begin(), ClassicImageYaxis.end(), ClassicImageXaxis.end(), false);
	}
}

/******************************************************************************
****函数名：size()
****说  明：获取FRQI量子图像大小
****参  数：无
****返回值：vector<int> 图像长和宽
*******************************************************************************/
vector<int> FRQI::size(void) 
{
	vector<int> temp;
	temp.push_back(Height);
	temp.push_back(Width);
	return temp;
}

/******************************************************************************
****函数名：Show_QuantumImage()
****说  明：显示FRQI量子图像
****参  数：无
****返回值：无
*******************************************************************************/
void FRQI::Show_QuantumImage(void) 
{
	std::cout << "Show FRQI Quantum Image Constructor!" << std::endl;
	//显示量子图像
	for (int m = 0; m < Height; ++m) 
	{
		for (int n = 0; n < Width; ++n) 
		{
			std::cout << "FRQI_QuantumImage[" << m << "][" << n << "]=1/2^(" << Height/2 + Width/2 << ")*(cos(" 
				<< QuantumImage[m][n] << ")|0>+sin(" << QuantumImage[m][n] << ")|1>)";
			std::cout << "|";
			//显示X轴坐标
			for (int cnt = 0; cnt < QuantumNumX; ++cnt) 
			{
				std::cout << QuantumX[m][cnt];
			}
			std::cout << ">|";
			//显示Y轴坐标
			for (int cnt = 0; cnt < QuantumNumY; ++cnt)
			{
				std::cout << QuantumY[n][cnt];
			}
			std::cout << ">" << std::endl;
		}
	}
}

/******************************************************************************
****函数名：Show_QuantumImage()
****说  明：显示FRQI量子图像
****参  数：
****string name：名字
****返回值：无
*******************************************************************************/
void FRQI::Show_QuantumImage(string name) 
{
	std::cout << "Show FRQI Quantum Image Constructor!" << std::endl;
	//显示量子图像
	for (int m = 0; m < Height; ++m)
	{
		for (int n = 0; n < Width; ++n) 
		{
			std::cout << name <<"[" << m << "][" << n << "]=1/2^(" << Height/2 + Width/2 << ")*(cos(" 
				<< QuantumImage[m][n] << ")|0>+sin(" << QuantumImage[m][n] << ")|1>)";
			std::cout << "|";
			//显示X轴坐标
			for (int cnt = 0; cnt < QuantumNumX; ++cnt) 
			{
				std::cout << QuantumX[m][cnt];
			}
			std::cout << ">|";
			//显示Y轴坐标
			for (int cnt = 0; cnt < QuantumNumY; ++cnt) 
			{
				std::cout << QuantumY[n][cnt];
			}
			std::cout << ">" << std::endl;
		}
	}
}

/******************************************************************************
****函数名：Delete_QuantumImages()
****说  明：删除FRQI量子图像内存
****参  数：无
****返回值：无
*******************************************************************************/
void FRQI::Delete_QuantumImages() 
{
	vector<vector<float>>().swap(QuantumImage);
	vector<vector<char>>().swap(QuantumX);
	vector<vector<char>>().swap(QuantumY);
	vector<int>().swap(ClassicImageXaxis);
	vector<int>().swap(ClassicImageYaxis);
}

/******************************************************************************
****函数名：NEQR_Save()
****说  明：保存NEQR量子图像
****参  数：
****文件名：string filename
****视频名：string video
****返回值：正确 int
*******************************************************************************/
int NEQR_Save(string filename, string video) 
{
	Mat frame;
	int QuantumNumTotal;										//总量子数量
	int QuantumNumX;											//X轴量子数量
	int QuantumNumY;											//Y轴量子数量
	int Height;													//图像高度
	int Width;													//图像宽度
	vector<vector<vector<char>>> QuantumImage;					//存储量子图像
	vector<vector<char>> QuantumX;								//量子图像X轴
	vector<vector<char>> QuantumY;								//量子图像Y轴
	vector<int> ClassicImageXaxis;								//经典图像X轴
	vector<int> ClassicImageYaxis;								//经典图像Y轴

	FILE *sourcefile = fopen(filename.c_str(), "wb");
	VideoCapture capture;
	capture = VideoCapture(video);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
	if (!capture.isOpened())
    {
        cout<<"No camera or video input!"<<endl;
        return -1;
    }
	NEQR neqr;
	//int frametotal = capture.get(CAP_PROP_FRAME_COUNT);//总帧数
	int frametotal = 10;//设定帧数
	capture >> frame;
	cv::Mat ResizeImage = neqr.Resize_Classic_Image(frame, 512, 512, CV_RGB2GRAY, true);
	neqr.Image_Measure(ResizeImage);
    neqr.Quantum_Image_Init(ResizeImage, false);
	for (int total = 0; total < frametotal; ++total) 
	{
		capture >> frame;
        if (frame.empty()) 
		{
            continue;
        }  
        cv::Mat ResizeImage = neqr.Resize_Classic_Image(frame, 512, 512, CV_RGB2GRAY, true);
		neqr.Constructor(ResizeImage, false);
		QuantumNumTotal = neqr.Get_QuantumNumTotal();
		QuantumNumX = neqr.Get_QuantumNumX();
		QuantumNumY = neqr.Get_QuantumNumY();
		Height = neqr.Get_Image_Height();
		Width = neqr.Get_Image_Width();
		QuantumImage = neqr.Get_Quantum_Image_Pixel();
		QuantumX = neqr.Get_Xaxis_Position();
		QuantumY = neqr.Get_Yaxis_Position();
		ClassicImageXaxis = neqr.Get_Classic_Xaxis_Position();
		ClassicImageYaxis = neqr.Get_Classic_Yaxis_Position();

    	fwrite(&QuantumNumTotal, sizeof(int), 1, sourcefile);			//保存总量子数
		fwrite(&QuantumNumX, sizeof(int), 1, sourcefile);				//保存X轴量子数
		fwrite(&QuantumNumY, sizeof(int), 1, sourcefile);				//保存Y轴量子数
		fwrite(&Height, sizeof(int), 1, sourcefile);						//保存图像高度Y轴
		fwrite(&Width, sizeof(int), 1, sourcefile);						//保存图像宽度X轴
		//保存量子图像像素
		for (int i = 0; i < Height; ++i) 
		{
			for (int j = 0; j < Width; ++j) 
			{
				for (int k = 0; k < 8; ++k) 
				{
					fwrite(&QuantumImage[i][j][k], sizeof(char), 1, sourcefile);
				}
			}
		}
		//量子图像Y轴坐标
		for (int j = 0; j < Height; ++j) 
		{
			for (int cnt = 0; cnt < QuantumNumY; ++cnt) 
			{
				fwrite(&QuantumY[j][cnt], sizeof(char), 1, sourcefile);
			}
		}
		//量子图像X轴坐标
		for (int i = 0; i < Width; ++i) 
		{
			for (int cnt = 0; cnt < QuantumNumX; ++cnt) 
			{
				fwrite(&QuantumX[i][cnt], sizeof(char), 1, sourcefile);
			}
		}
		//图像Y轴坐标
		for (int j = 0; j < Height; ++j) 
		{
			fwrite(&ClassicImageYaxis[j], sizeof(int), 1, sourcefile);
		}
		//图像X轴坐标
		for (int j = 0; j < Width; ++j) 
		{
			fwrite(&ClassicImageXaxis[j], sizeof(int), 1, sourcefile);
		}
	}
	fclose(sourcefile);
	return 0;
}

/******************************************************************************
****函数名：NEQR_Read()
****说  明：读取NEQR量子图像
****参  数：
****文件名：string filename
****NEQR:	vector<NEQR>& neqr_data
****返回值：正确 int
*******************************************************************************/
int NEQR_Read(string filename, vector<NEQR>& neqr_data) 
{
	Mat frame;
	int QuantumNumTotal;										//总量子数量
	int QuantumNumX;											//X轴量子数量
	int QuantumNumY;											//Y轴量子数量
	int Height;													//图像高度
	int Width;													//图像宽度
	vector<vector<vector<char>>> QuantumImage;					//存储量子图像
	vector<vector<char>> QuantumX;								//量子图像X轴
	vector<vector<char>> QuantumY;								//量子图像Y轴
	vector<int> ClassicImageXaxis;								//经典图像X轴
	vector<int> ClassicImageYaxis;								//经典图像Y轴
	FILE *sourcefile = fopen(filename.c_str(), "rb");

	for (int frametotal = 0; frametotal < Action_Image * Action_Type; frametotal++) 
	{
    	fread(&QuantumNumTotal, sizeof(int), 1, sourcefile);			//保存总量子数
		fread(&QuantumNumX, sizeof(int), 1, sourcefile);				//保存X轴量子数
		fread(&QuantumNumY, sizeof(int), 1, sourcefile);				//保存Y轴量子数
		fread(&Height, sizeof(int), 1, sourcefile);						//保存图像高度Y轴
		fread(&Width, sizeof(int), 1, sourcefile);						//保存图像宽度X轴
		//保存量子图像像素
		for (int i = 0; i < Height; ++i) 
		{
			for (int j = 0; j < Width; ++j) 
			{
				for (int k = 0; k < 8; ++k) 
				{
					fread(&QuantumImage[i][j][k], sizeof(char), 1, sourcefile);
				}
			}
		}
		//量子图像Y轴坐标
		for (int j = 0; j < Height; ++j) 
		{
			for (int cnt = 0; cnt < QuantumNumY; ++cnt) 
			{
				fread(&QuantumY[j][cnt], sizeof(char), 1, sourcefile);
			}
		}
		//量子图像X轴坐标
		for (int i = 0; i < Width; ++i) 
		{
			for (int cnt = 0; cnt < QuantumNumX; ++cnt) 
			{
				fread(&QuantumX[i][cnt], sizeof(char), 1, sourcefile);
			}
		}
		//图像Y轴坐标
		for (int j = 0; j < Height; ++j) 
		{
			fread(&ClassicImageYaxis[j], sizeof(int), 1, sourcefile);
		}
		//图像X轴坐标
		for (int j = 0; j < Width; ++j) 
		{
			fread(&ClassicImageXaxis[j], sizeof(int), 1, sourcefile);
		}
		NEQR neqr(QuantumNumTotal,
			QuantumNumX,
			QuantumNumY,
			Height,
			Width,
			8,
			QuantumImage,
			QuantumX,
			QuantumY,
			ClassicImageXaxis,
			ClassicImageYaxis);
		neqr_data.push_back(neqr);
	}
	fclose(sourcefile);
	return 0;
}