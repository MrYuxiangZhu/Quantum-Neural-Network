/********************************************************************************
****文件名：VIBE
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/

#include "QuantumViBe.hpp"

/*===================================================================
 * 并行子函数：Process_Frames_accumulate_block
 * 说明：独立线程计算；
 * 参数：
 *   Iterator first:  像素行初始位置
 *   Iterator last :  像素行结束位置
 *   Iterator colums: 像素列数
 *   vector<vector<vector<T> > >& result：像素值
 *------------------------------------------------------------------
 * Constructed Function: ViBe
 *
 * Summary:
 *   Init several arguments of ViBe Algorithm.
 *
 * Arguments:
 *   int num_sam - Number of pixel's samples
 *   int min_match - Match Number of make pixel as Background
 *   int r - Radius of pixel value
 *   int rand_sam - the probability of random sample
=====================================================================
*/

Process_Frames_accumulate_block::Process_Frames_accumulate_block() 
{
    num_samples = DEFAULT_NUM_SAMPLES;
    num_min_matches = DEFAULT_MIN_MATCHES;
    radius = DEFAULT_RADIUS;
    random_sample = DEFAULT_RANDOM_SAMPLE;
    fgtimes = DEFAULT_FOREGROUND_TIME;
}

/*===================================================================
 * 并行子函数：Process_Frames_accumulate_block::operator()
 * 说明：独立线程计算；
 * 参数：
 *   const Mat& img:  原图像
 *   Iterator first:  像素行初始位置
 *   Iterator last :  像素行结束位置
 *   Iterator colums: 像素列数
 *   vector<vector<unsigned char> >& FGModel: 二值化图像
 *   vector<vector<vector<T> > >& result：像素值
 *------------------------------------------------------------------
=====================================================================
*/
template<typename Iterator>
void Process_Frames_accumulate_block::operator()(const Mat& img, Iterator first, Iterator last, Iterator colums, vector<vector<unsigned char> >& FGModel, vector<vector<vector<unsigned char>>>& result) 
{   //每个子序列累加，不能通过线程的返回值返回累加结果，而是通过一个result引用将结果返回给主线程
    RNG rng;
    int k = 0, dist = 0, matches = 0;
    for (int i = *first; i <= *(last-1); ++i)
	{
        for (int j = 0; j <= *(colums - 1); ++j)
        {
            //========================================
            //        前景提取   |   Extract Foreground Areas
            //========================================
            /*===================================================================
             * 说明：计算当前像素值与样本库的匹配情况；
             * 参数：
             *   int matches: 当前像素值与样本库中值之差小于阈值范围RADIUS的个数；
             *   int count: 遍历样本库的缓存变量；
             *------------------------------------------------------------------
             * Summary:
             *   Count how many samples in library can match to current pixel value.
             *
             * Argumen:
             *   int matches - the Number of samples whose value subtract current pixel's value
             *          is less than RADIUS.
             *   int count - the temp variance for going through sample library.
            =====================================================================
            */
            for(k = 0, matches = 0; matches < num_min_matches && k < num_samples; ++k)
            {
                dist = abs(result[i][j][k] - img.at<unsigned char>(i, j));
                if (dist < radius) 
                {
                    matches++;
                }
            }
            /*===================================================================
             * 说明：
             *      当前像素值与样本库中值匹配次数较高，则认为是背景像素点；
             *      此时更新前景统计次数、更新前景模型、更新该像素模型样本值、更新该像素点邻域像素点的模型样本值
             *------------------------------------------------------------------
             * Summary:
             *   the Match Times of current pixel value and samples in library is large enough to
             * regard current pixel as a Background pixel.
             *   Then it needs to be done:
             *   - Run the times of Foreground Statistic
             *   - Run Foreground Model
             *   - Run model sample library of this pixel probably
             *   - Run model sample library of this pixel's neighborhood pixel probably
            =====================================================================
            */
           if (matches >= num_min_matches)
            {
                //std::cout << "matches,num_min_matches = " << matches << " , " << num_min_matches << std::endl;
                // 已经认为是背景像素，故该像素的前景统计次数置0
                // This pixel has regard as a background pixel, so the count of this pixel's foreground statistic set as 0
                result[i][j][num_samples]=0;
                //std::cout << "i,j = " << i << " , " << j << std::endl;
                // 该像素点被的前景模型像素值置0
                // Set Foreground Model's pixel as 0
                FGModel[i][j] = 0;
                //FGModel.at<unsigned char>(i, j) = 0;
            }
            /*===================================================================
             * 说明：
             *      当前像素值与样本库中值匹配次数较低，则认为是前景像素点；
             *      此时需要更新前景统计次数、判断更新前景模型；
             *------------------------------------------------------------------
             * Summary:
             *   the Match Times of current pixel value and samples in library is small enough to
             * regard current pixel as a Foreground pixel.
             *   Then it needs to be done:
             *   - Run the times of Foreground Statistic
             *   - Judge and Run Foreground Model
            =====================================================================
            */
            else
            {
                //std::cout << "matches,num_min_matches = " << matches << " , " << num_min_matches << std::endl;
                // 已经认为是前景像素，故该像素的前景统计次数+1
                // This pixel has regard as a foreground pixel, so the count of this pixel's foreground statistic plus 1
                result[i][j][num_samples]++;

                // 该像素点被的前景模型像素值置255
                // Set Foreground Model's pixel as 255
                FGModel[i][j] = 255;

                // 如果某个像素点连续50次被检测为前景，则认为一块静止区域被误判为运动，将其更新为背景点
                // if this pixel is regarded as foreground for more than 50 times, then we regard this static area as dynamic area by mistake, and Run this pixel as background one.
                if(result[i][j][num_samples] > fgtimes)
                {
                    int random = rng.uniform(0, num_samples);
                    result[i][j][random]=img.at<unsigned char>(i, j);
                }
            }

            //================================================================
            //        更新模型样本库    |    Update Background Model Sample Library
            //================================================================
            if (matches >= num_min_matches)
            {
                // 已经认为该像素是背景像素，那么它有 1 / φ 的概率去更新自己的模型样本值
                // This pixel is already regarded as Background Pixel, then it has possibility of 1/φ to Run its model sample's value.
                int random = rng.uniform(0, random_sample);
                if (random == 0)
                {
                    random = rng.uniform(0, num_samples);
                    result[i][j][random] = img.at<unsigned char>(i, j);
                }

                // 同时也有 1 / φ 的概率去更新它的邻居点的模型样本值
                // At the same time, it has possibility of 1/φ to Run its neighborhood point's sample value.
                random = rng.uniform(0, random_sample);
                if (random == 0)
                {
                    int row, col;
                    random = rng.uniform(0, 9); row = i + c_yoff[random];
                    random = rng.uniform(0, 9); col = j + c_xoff[random];

                    // 防止选取的像素点越界
                    // Protect Pixel from Crossing the border
                    if (row < 0) row = 0;
                    if (row >= img.rows)  row = img.rows - 1;
                    if (col < 0) col = 0;
                    if (col >= img.cols) col = img.cols - 1;

                    // 为样本库赋随机值
                    // Set random pixel's Value for Sample Library
                    random = rng.uniform(0, num_samples);
                    result[row][col][random]=img.at<unsigned char>(i, j);
                }
            }
        }
    }
}

/*===================================================================
 * 多线程构造函数：Process_Frames_parallel_accumulate
 * 说明：构造多个线程；
 * 参数：
 *   int num_sam:  每个像素点的样本个数
 *   int min_match:  #min指数
 *   int r:  Sqthere半径
 *   int rand_sam:  子采样概率
 *------------------------------------------------------------------
 * Constructed Function: Process_Frames_parallel_accumulate
 *
 * Summary:
 *   Init several arguments of ViBe Algorithm.
 *
 * Arguments:
 *   int num_sam - Number of pixel's samples
 *   int min_match - Match Number of make pixel as Background
 *   int r - Radius of pixel value
 *   int rand_sam - the probability of random sample
=====================================================================
*/
template<typename Iterator>
void QViBe::Process_Frames_parallel_accumulate(const Mat& img, Iterator first,Iterator last, Iterator colums, bool Debug) 
{
    unsigned long const length = std::distance(first, last);//计算序列的长度
    unsigned long const block_size = length / hardware_threads;//重新计算每个线程需要执行的序列大小
    vector<thread> threads(hardware_threads - 1);//开启线程池，只用开启num_threads-1个子线程，因为主线程也可以计算一个序列
    Iterator block_start = first;//序列开始位置
    for (int i  = 0; i < (hardware_threads - 1); ++i) 
    {//这里只分配子线程的任务序列
        Iterator block_end = block_start;
        std::advance(block_end, block_size);//迭代器block_end向前移动block_size
        threads[i] = std::thread (Process_Frames_accumulate_block(), img, block_start, block_end, colums, std::ref(fgmodel), std::ref(samples));//每个子线程的子序列分配
        block_start = block_end;//更新序列位置
    }
    Process_Frames_accumulate_block()(img, block_start, last, colums, fgmodel, samples);//主线程的任务，注意是last
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//等待子线程完成
}

/*===================================================================
 * 构造函数：QViBe
 * 说明：初始化QViBe算法部分参数；
 * 参数：
 *   int num_sam:  每个像素点的样本个数
 *   int min_match:  #min指数
 *   int r:  Sqthere半径
 *   int rand_sam:  子采样概率
 *   int times:检测前景的次数
 *------------------------------------------------------------------
 * Constructed Function: QViBe
 *
 * Summary:
 *   Init several arguments of QViBe Algorithm.
 *
 * Arguments:
 *   int num_sam - Number of pixel's samples
 *   int min_match - Match Number of make pixel as Background
 *   int r - Radius of pixel value
 *   int rand_sam - the probability of random sample
 *   int times - foreground times
=====================================================================
*/
QViBe::QViBe(int num_sam, int min_match, int r, int rand_sam, int times)
{
    num_samples = num_sam;
    num_min_matches = min_match;
    radius = r;
    random_sample = rand_sam;
    fgtimes = times;
    int c_off[9] = {-1, 0, 1, -1, 1, -1, 0, 1, 0};
    for(int i = 0; i < 9; i++)
    {
        c_xoff[i] = c_yoff[i] = c_off[i];
	}
    hardware_threads = Obtain_Thread_Num();
}

/*===================================================================
 * 析构函数：~QViBe
 * 说明：释放样本库内存；
 *------------------------------------------------------------------
 * Destructor Function: ~QViBe
 *
 * Summary:
 *   Release the memory of Sample Library.
=====================================================================
*/
QViBe::~QViBe(void)
{
    DeleteSamples();
}

/*===================================================================
 * 函数名：Init
 * 说明：背景模型初始化；
 *    为样本库分配空间；
 * 参数：
 *   NEQR& qimg:  源图像
 * 返回值：void
 *------------------------------------------------------------------
 * Function: Init
 *
 * Summary:
 *   Init Background Model.
 *   Assign space for sample library.
 *   Read the first frame of video query as background model, then select pixel's
 * neighbourhood pixels randomly and fill the sample library.
 *
 * Arguments:
 *   NEQR& qimg - source image
 *
 * Returns:
 *   void
=====================================================================
*/
void QViBe::Init(NEQR& qimg)
{
    //测量图片
    size(qimg); 
    //初始化量子测量
    measure.Init(qimg);
    measure.Measure(qimg, false);
    cv::Mat classicimg = measure.Get_Classic_Image_Reconstructor();				//获取重建后的经典图像       
    //初始化经典图像X轴坐标
	for(int i = 0; i < Width; ++i) 
    {
        ClassicImageXaxis.emplace_back(i);
    }
    //初始化经典图像Y轴坐标
	for(int j = 0; j < Height; ++j) 
    {
        ClassicImageYaxis.emplace_back(j);
    }
    // 动态分配三维数组，samples[][][num_samples]存储前景被连续检测的次数
    // Dynamic Assign 3-D Array.
    // sample[img.rows][img.cols][num_samples] is a 3-D Array which includes all pixels' samples.
    vector<unsigned char> sample_temp;
    vector<vector<unsigned char>> samples_temp;
    //std::cout << "classicimg.rows = " << classicimg.rows << std::endl;
    //std::cout << "classicimg.cols = " << classicimg.cols << std::endl;
    for (int i = 0; i < classicimg.rows; ++i)
	{
        for (int j = 0; j < classicimg.cols; ++j)
		{
            // 数组中，在num_samples之外多增的一个值，用于统计该像素点连续成为前景的次数；
            // the '+ 1' in 'num_samples + 1', it's used to count times of this pixel regarded as foreground pixel.
            for (int k = 0; k < num_samples + 1; ++k)
			{
                // 创建样本库时，所有样本全部初始化为0
                // All Samples init as 0 When Creating Sample Library.
                sample_temp.emplace_back(0);
			}
            samples_temp.emplace_back(sample_temp);
            sample_temp.clear();
		}
        samples.emplace_back(samples_temp);
        samples_temp.clear();
	}
    sample_temp.clear();
    for (int i = 0; i < classicimg.rows; ++i)
	{
        for (int j = 0; j < classicimg.cols; ++j)
		{
            // 数组中，在num_samples之外多增的一个值，用于统计该像素点连续成为前景的次数；
            // the '+ 1' in 'num_samples + 1', it's used to count times of this pixel regarded as foreground pixel.
            sample_temp.emplace_back(0);
		}
        fgmodel.emplace_back(sample_temp);
        sample_temp.clear();
	}
    vector<unsigned char>().swap(sample_temp);
    vector<vector<unsigned char>>().swap(samples_temp);
    FGModel = cv::Mat::zeros(classicimg.size(),CV_8UC1);           //初始化二值化图像 
}

/*===================================================================
 * 函数名：ProcessFirstFrame
 * 说明：处理第一帧图像；
 *    读取视频序列第一帧，并随机选取像素点邻域内像素填充样本库，初始化背景模型；
 * 参数：
 *   NEQR& qimg:  源图像
 * 返回值：void
 *------------------------------------------------------------------
 * Function: ProcessFirstFrame
 *
 * Summary:
 *   Process First Frame of Video Query, then select pixel's neighbourhood pixels
 * randomly and fill the sample library, and init Background Model.
 *
 * Arguments:
 *   NEQR& qimg - source image
 *
 * Returns:
 *   void
=====================================================================
*/
void QViBe::ProcessFirstFrame(NEQR& qimg)
{
	RNG rng;
	int row, col;
    measure.Measure(qimg, false);
    cv::Mat img = measure.Get_Classic_Image_Reconstructor();				//获取重建后的经典图像       
    for(int i = 0; i < img.rows; i++)
	{
        for(int j = 0; j < img.cols; j++)
		{
            for(int k = 0 ; k < num_samples; k++)
			{
                // 随机选择num_samples个邻域像素点，构建背景模型
                // Random pick up num_samples pixel in neighbourhood to construct the model
                int random;
                random = rng.uniform(0, 9); row = i + c_yoff[random];
                random = rng.uniform(0, 9); col = j + c_xoff[random];

                // 防止选取的像素点越界
                // Protect Pixel from Crossing the border
                if (row < 0)  row = 0;
                if (row >= img.rows)  row = img.rows - 1;
                if (col < 0)  col = 0;
                if (col >= img.cols)   col = img.cols - 1;

                // 为样本库赋随机值
                // Set random pixel's Value for Sample Library
                samples[i][j][k]=img.at<unsigned char>(row, col);
			}
		}
	}
}

/*===================================================================
 * 函数名：Run
 * 说明：运行 QViBe 算法，提取前景区域并更新背景模型样本库；
 * 参数：
 *   NEQR& qimg:  源图像
 * 返回值：void
 *------------------------------------------------------------------
 * Function: Run
 *
 * Summary:
 *   Run the QViBe Algorithm: Extract Foreground Areas & Update Background Model Sample Library.
 *
 * Arguments:
 *   NEQR& qimg - source image
 *
 * Returns:
 *   void
=====================================================================
*/
void QViBe::Run(NEQR& qimg, bool Debug)
{
    measure.Measure(qimg, false);
    cv::Mat ClassicImg = measure.Get_Classic_Image_Reconstructor();				//获取重建后的经典图像  
    if (Debug) 
    {
		double time;
    	double start;
		std::cout << "QViBe Run!" << std::endl;
		start = static_cast<double>(getTickCount());
        Process_Frames_parallel_accumulate(ClassicImg, ClassicImageXaxis.begin(), ClassicImageXaxis.end(), ClassicImageYaxis.end(), false);
        time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of QViBe = " << time << " ms" << std::endl;
	}
	else 
    {
        Process_Frames_parallel_accumulate(ClassicImg, ClassicImageXaxis.begin(), ClassicImageXaxis.end(), ClassicImageYaxis.end(), false);
    }
}

/*===================================================================
 * 函数名：Show
 * 说明：显示原图像数据；
 * 返回值：无
 *------------------------------------------------------------------
 * Function: getFGModel
 *
 * Summary:
 *   get Foreground Model Binary Image.
 *
 * Returns:
 *   null
=====================================================================
*/
void QViBe::Show() 
{
    std::cout << "samples.size()= " << samples.size() << std::endl;
	std::cout << "samples[0].size()= " << samples[0].size() << std::endl;
	std::cout << "samples[0][0].size()= " << samples[0][0].size() << std::endl;
	for (int i = 0; i < samples.size(); ++i) 
    {
		for (int j = 0; j < samples[0].size(); ++j) 
        {
			for(int k = 0; k < samples[0][0].size(); ++k) 
            {
				std::cout << dec << "samples[" << i << "][" << j << "][" << k << "]=" << (int)samples[i][j][k] << std::endl;
			}
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
void QViBe::size(NEQR& QuantumImage) 
{
	Height = QuantumImage.Get_Image_Height();
	Width = QuantumImage.Get_Image_Width();
}

/*===================================================================
 * 函数名：getFGModel
 * 说明：获取前景模型二值图像；
 * 返回值：Mat
 *------------------------------------------------------------------
 * Function: getFGModel
 *
 * Summary:
 *   get Foreground Model Binary Image.
 *
 * Returns:
 *   Mat
=====================================================================
*/
cv::Mat QViBe::Get_FGModel()
{
    for (int i = 0; i < fgmodel.size(); ++i)
	{
		unsigned char* ptmp = FGModel.ptr<unsigned char>(i);//指针指向FGModel的第i行
		for (int j = 0; j < fgmodel[0].size(); ++j)
		{
			ptmp[j] = fgmodel[i][j];//二维数组数据传给FGModel的第i行第j列
		}
	}
    return FGModel;
}

/*===================================================================
 * 函数名：DeleteSamples
 * 说明：删除样本库；
 * 返回值：void
 *------------------------------------------------------------------
 * Function: DeleteSamples
 *
 * Summary:
 *   Delete Sample Library.
 *
 * Returns:
 *   void
=====================================================================
*/
void QViBe::DeleteSamples()
{
    vector<vector<unsigned char>>().swap(fgmodel);
    vector<vector<vector<unsigned char>>>().swap(samples);
}
