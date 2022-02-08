/********************************************************************************
****文件名：摄像头源文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/11/11
*********************************************************************************/

#include "camera.hpp"

/******************************************************************************
********摄像头构造函数
*******************************************************************************/
Camera::Camera() 
{
	std::cout << "Built with OpenCV " << CV_VERSION << std::endl;
	std::cout << "Open Camera " << std::endl;
}

/******************************************************************************
********摄像头构造函数,默认num=0
*******************************************************************************/
Camera::Camera(int num): device_num(num) 
{
	std::cout << "Built with OpenCV " << CV_VERSION << std::endl;
	std::cout << "Open Camera " << std::endl;
	capture.open(device_num);
}

/******************************************************************************
********摄像头构造函数
*******************************************************************************/
Camera::Camera(string name): device_name(name) 
 {
    std::cout << "Built with OpenCV " << CV_VERSION << std::endl;
	std::cout << "Open Camera " << std::endl;
	capture.open(name);
}

/******************************************************************************
********摄像头设备号
*******************************************************************************/
void Camera::device(int num) 
{
	capture.open(num);
}

/******************************************************************************
********摄像头设备号
*******************************************************************************/
void Camera::device(string name) 
{
	capture.open(name);
}

/******************************************************************************
********摄像头析构函数
*******************************************************************************/
Camera::~Camera() 
{

}

/******************************************************************************
********获取每一帧图像
*******************************************************************************/
Mat Camera::GetFrameImage(void) 
{
	if (!capture.isOpened()) 
    {
		std::cout << "create camera capture error" << std::endl;
		system("pause");
		exit(-1);
	}

	capture >> frame;			//读入视频解码
	return frame;
}

/******************************************************************************
********释放摄像头
*******************************************************************************/
void Camera::ReleaseCamera(void) 
{
	cvDestroyAllWindows();
}

/******************************************************************************
********整形转字符串
*******************************************************************************/
string Camera::IntToString(int num)
{
    char str[256];
    sprintf(str, "%d", num);
    string result = str;
    return result;
}

/******************************************************************************
********获取时间字符串
*******************************************************************************/
string Camera::GetLocalTime(void) 
{
	time_t Local = time(NULL);
    LocalTime = localtime(&Local);

    BeijingTime = IntToString(LocalTime->tm_year + 1900);
    BeijingTime += "/";

    if (LocalTime->tm_mon < 10)
    {
        BeijingTime += "0";
        BeijingTime += IntToString(LocalTime->tm_mon + 1);
    }
    else 
    {
        BeijingTime += IntToString(LocalTime->tm_mon + 1);
    }

    BeijingTime += "/";

    if (LocalTime->tm_mday < 10) 
    {
        BeijingTime += "0";
        BeijingTime += IntToString(LocalTime->tm_mday);
    }
    else 
    {
        BeijingTime += IntToString(LocalTime->tm_mday);
    }

    BeijingTime += " ";

    if (LocalTime->tm_hour < 10) 
    {
        BeijingTime += "0";
        BeijingTime += IntToString(LocalTime->tm_hour);
    }
    else 
    {
        BeijingTime += IntToString(LocalTime->tm_hour);
    }

    BeijingTime += ":";

    if (LocalTime->tm_min < 10) 
    {
        BeijingTime += "0";
        BeijingTime += IntToString(LocalTime->tm_min);
    }
    else 
    {
        BeijingTime += IntToString(LocalTime->tm_min);
    }

    BeijingTime += ":";

    if (LocalTime->tm_sec < 10) 
    {
        BeijingTime += "0";
        BeijingTime += IntToString(LocalTime->tm_sec);
    }
    else 
    {
        BeijingTime += IntToString(LocalTime->tm_sec);
    }

	return BeijingTime;
}

/******************************************************************************
********加水印
*******************************************************************************/
void Camera::DrawText(Mat & image)
{
	string TImeString = GetLocalTime();
    putText(image, TImeString,
            Point(20, 50),
            FONT_HERSHEY_COMPLEX, 1, // font face and scale
            Scalar(0, 0, 255), // red
            1, LINE_AA); // line thickness and type
}
