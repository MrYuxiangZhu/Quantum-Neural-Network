/********************************************************************************
****文件名：摄像头头文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/11/11
*********************************************************************************/

#ifndef __CAMERA_HPP
#define __CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <vector>
#include <iostream>
using namespace cv;
using namespace std;


/********************************************************************************
*******摄像头数据
********************************************************************************/
class Camera {
private:
    int device_num;             //设备号
    string device_name;         //设备名称
    Mat frame;                  //每一帧图像
    VideoCapture capture;       //摄像头
    tm* LocalTime;              //本地时间
    string BeijingTime;         //北京时间字符串           
public:
	Camera();
    Camera(int num);
    Camera(string name);
    ~Camera();
	void device(int num);			   //设备号
	void device(string name);		   //设备号
    Mat GetFrameImage(void);       //获取每一帧图像
    void ReleaseCamera(void);      //释放摄像头
    string IntToString(int num);   //整形转字符串
    string GetLocalTime(void);     //获取时间字符串
    void DrawText(Mat& image);     //增加水印
	VideoCapture Get_Capture(void){ return capture ;};
};

#endif