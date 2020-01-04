/********************************************************************************
****文件名：摄像头测试程序
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/11/11
*********************************************************************************/

#include "../Camera/camera.hpp"

int main (int argv, char *argc[]) 
{
    Mat frame;
    Mat edges;
    Camera video(0);               //设置摄像头对象
    cvNamedWindow("SrcImage",1);
	//进入循环前，等待一段时间摄像头初始化，否则初始化未完成，会直接闪退出程序
	//cvWaitKey(2000);
    while(1)
	{
		
        frame = video.GetFrameImage();
        if(frame.empty())
            break;
        video.DrawText(frame);
        imshow("SrcImage",frame);//显示原图像
		cvWaitKey(3);
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
        video.DrawText(edges);
		imshow("当前视频",edges);
        if(waitKey(10) >= 0)
            break;
	}
    video.ReleaseCamera();          //释放摄像头
    return 0;
}