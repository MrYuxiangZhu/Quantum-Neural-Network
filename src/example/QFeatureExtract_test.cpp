/********************************************************************************
****文件名：量子图像特征提取
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/11/19
*********************************************************************************/
#include "../QuantumImageFeatureExtract/QuantumImageFeatureExtract.hpp"
#include "../QuantumViBe/QuantumViBe.hpp"
#include "../QuantumImageEdgeExtract/QuantumImageEdgeExtract.hpp"

#define windows 16

int main(int argc, char* argv[])
{
    VideoCapture capture;
    capture = VideoCapture("./Video/Camera Road 01.avi");
    //capture = VideoCapture("./Video/ira_wave2.avi");
    if(!capture.isOpened())
    {
        capture = VideoCapture("../Video/Camera Road 01.avi");
        //capture = VideoCapture("../Video/ira_wave2.avi");
        if(!capture.isOpened())
        {
            capture = VideoCapture("../../Video/Camera Road 01.avi");
            //capture = VideoCapture("../../Video/ira_wave2.avi");
            if(!capture.isOpened())
            {
                cout<<"ERROR: Did't find this video!"<<endl;
                return 0;
            }
        }
    }
     // 程序运行时间统计变量
    // the Time Statistical Variable of Program Running Time
    double time;
    double start;
    cv::Mat First, frame, gray, FGModel, Edge;
    QViBe qvibe;
    NEQR QuantumImage;
    Quantum_Measure measure;
    Quantum_Image_Edge_Extract Edge_Extract;
    Quantum_Image_Feature_Extract Feature;
    vector<vector<float> > FeatureImage;

    int count = 0;
    int frameToStart = 0;
    capture.set(CAP_PROP_POS_FRAMES, frameToStart);//从此时的帧数开始获取帧
    if (!capture.read(First))
    {
        std::cout << "Read vedio failed!" << std::endl;
    }
    cv::Mat FirstResizeImage = QuantumImage.Resize_Classic_Image(First, 256, 256, CV_RGB2GRAY, false);
    //cv::Mat FirstResizeImage = QuantumImage.Resize_Classic_Image(First, CV_RGB2GRAY, false);
    QuantumImage.Image_Measure(FirstResizeImage);
    QuantumImage.Quantum_Image_Init(FirstResizeImage, false);
    measure.Init(QuantumImage);
    qvibe.Init(QuantumImage);
    while (1)
    {
        capture >> frame;
        if (frame.empty())
            continue;
        if (count == 0)
        {
            cv::Mat ResizeImage = QuantumImage.Resize_Classic_Image(frame, 256, 256, CV_RGB2GRAY, false);
            //cv::Mat ResizeImage = QuantumImage.Resize_Classic_Image(frame, CV_RGB2GRAY, false);
            //GaussianBlur(ResizeImage, ResizeImage, Size(7,7), 1.5, 1.5);
            QuantumImage.Constructor(ResizeImage, false);
            qvibe.ProcessFirstFrame(QuantumImage);
            Edge_Extract.Quantum_Image_Edge_Extract_Init(ResizeImage);
            Feature.Init(ResizeImage, 4, 4);
            std::cout << "Time of Update ViBe Background = " << time << " ms" << std::endl;
            cout<<"Training ViBe Success."<<endl;
            //count = false;
        }
        else
        {
            start = static_cast<double>(getTickCount());
            cv::Mat ResizeImage = QuantumImage.Resize_Classic_Image(frame, 256, 256, CV_RGB2GRAY, false);
            //cv::Mat ResizeImage = QuantumImage.Resize_Classic_Image(frame, CV_RGB2GRAY, false);
            //GaussianBlur(ResizeImage, ResizeImage, Size(7,7), 1.5, 1.5);
            imshow("ResizeImage",ResizeImage);//显示原图像
            QuantumImage.Constructor(ResizeImage, false);               //构建NEQR
            qvibe.Run(QuantumImage, false);                             //运行QViBe
            FGModel = qvibe.Get_FGModel();                              //获取前景二值化图像
			//morphologyEx(FGModel, FGModel, MORPH_OPEN, Mat());
            Edge_Extract.Image_Edge_Extract(FGModel, Edge, false);      //提取边缘
            Feature.Run(Edge, FeatureImage, false);                     //提取特征值
            //Edge_Extract.Show_Edge("edge");
            //Feature.ShowFeatureImage("feature");
            time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
            std::cout << "Time of Update ViBe Background = " << time << " ms" << std::endl;
            imshow("FGModel", FGModel);
            imshow("Edge", Edge);
        }
        count++;
        if(waitKey(10) >= 0)
            break;
    }
    return 0;
}