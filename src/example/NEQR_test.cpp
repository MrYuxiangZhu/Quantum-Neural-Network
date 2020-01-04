/********************************************************************************
****文件名：NEQR量子图像构建过程测试
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/12/7
*********************************************************************************/

#include "../QuantumImagePreparation/QuantumImagePreparation.hpp"

int main(int argc, char* argv[])
{
    Mat frame, gray, FGModel;
    VideoCapture capture;
    capture = VideoCapture("./Video/Camera Road 01.avi");
    if(!capture.isOpened())
    {
        capture = VideoCapture("../Video/Camera Road 01.avi");
        if(!capture.isOpened())
        {
            capture = VideoCapture("../../Video/Camera Road 01.avi");
            if(!capture.isOpened())
            {
                cout<<"ERROR: Did't find this video!"<<endl;
                return 0;
            }
        }
    }

    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    //Camera vedio(1);               //设置摄像头对象
    //cvNamedWindow("SrcImage",1);
    if (!capture.isOpened())
    {
        cout<<"No camera or video input!"<<endl;
        return -1;
    }

    // 程序运行时间统计变量
    // the Time Statistical Variable of Program Running Time
    NEQR neqr;
    Quantum_Measure measure;
    bool count = neqr.Process_First_Frame(true);

    while (1)
    {
        capture >> frame;
        if (frame.empty()) {
            continue;
        }  
        //frame = vedio.GetFrameImage();
        //cvtColor(frame, gray, CV_RGB2GRAY);
        cv::Mat ResizeImage = neqr.Resize_Classic_Image(frame, CV_RGB2GRAY, true);
        if (count) {
            neqr.Image_Measure(ResizeImage);
            neqr.Quantum_Image_Init(ResizeImage, false);
            measure.Init(neqr);
            std::cout << "Training NEQR Success." << std::endl;
            count = neqr.Process_First_Frame(false);
            //vedio.DrawText(frame);
            cv::imshow("SrcImage",frame);//显示原图像
        }
        else {
            neqr.Constructor(ResizeImage, false);
            neqr.Show_QuantumImage();												//显示NEQR量子图像
		    //neqr.Show_ClassicImage(ResizeImage);
            measure.Measure(neqr, false);
            //measure.Show_ClassicImage();
            //vedio.DrawText(frame);
            cv::imshow("SrcImage",frame);//显示原图像
        }
        if(waitKey(10) >= 0)
            break;
    }
    return 0;
}