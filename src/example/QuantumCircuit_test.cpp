/********************************************************************************
****文件名：量子电路测试源文件
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/12/21
*********************************************************************************/
#include "../QuantumCircuit/QuantumCircuit.hpp"

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

    //capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    //capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    //Camera vedio(1);               //设置摄像头对象
    //cvNamedWindow("SrcImage",1);
    if (!capture.isOpened())
    {
        cout<<"No camera or video input!"<<endl;
        return -1;
    }
    int frame_width = (int)capture.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = (int)capture.get(CAP_PROP_FRAME_HEIGHT);
    std::cout << "frame_height: " << frame_height << std::endl; 
 	std::cout << "frame_width: " << frame_width << std::endl;   
    long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
	//std::cout << "total frames: " << totalFrameNumber << std::endl;
    int frameToStart = 0;

    Mat First, Last;
    capture.set(CAP_PROP_POS_FRAMES, frameToStart);//从此时的帧数开始获取帧
    if (!capture.read(First))
    {
        std::cout << "Read vedio failed!" << std::endl;
    }
    capture.set(CAP_PROP_POS_FRAMES, frameToStart + 1);//从此时的帧数开始获取帧
    if (!capture.read(Last))
    {
        std::cout << "Read vedio failed!" << std::endl;
    }
    // 程序运行时间统计变量
    // the Time Statistical Variable of Program Running Time
    NEQR first;
    NEQR last;
    NEQR result;
    Quantum_Measure measure;

    //cv::Mat FirstResizeImage = first.Resize_Classic_Image(First, 128, 128, CV_RGB2GRAY, false);
    //cv::Mat LastResizeImage = last.Resize_Classic_Image(Last, 128, 128, CV_RGB2GRAY, false);
    cv::Mat FirstResizeImage = first.Resize_Classic_Image(First, 256, 256, CV_RGB2GRAY, false);
    cv::Mat LastResizeImage = last.Resize_Classic_Image(Last, 256, 256, CV_RGB2GRAY, false);
    //cvNamedWindow("FirstImage",1);
    //cv::imshow("FirstImage",FirstResizeImage);//显示原图像
    //cvNamedWindow("LastImage",1);
    //cv::imshow("LastImage",LastResizeImage);//显示原图像

    first.Image_Measure(FirstResizeImage);
    first.Quantum_Image_Init(FirstResizeImage, false);

    last.Image_Measure(LastResizeImage);
    last.Quantum_Image_Init(LastResizeImage, false);

    //Quantum_Adder quantum_adder;
    //quantum_adder.QuantumImage_Adder_Operator_Init(first);

    Quantum_Subtracter quantum_substracter;
    quantum_substracter.QuantumImage_Subtracter_Operator_Init(first);

    result = first.Zeros();                                                                     //复制一张空白量子图像
    measure.Init(first);
    /*while (frameToStart < totalFrameNumber) {
        capture.set(CAP_PROP_POS_FRAMES, frameToStart);//从此时的帧数开始获取帧
        capture.read(First);
        capture.set(CAP_PROP_POS_FRAMES, frameToStart + 1);//从此时的帧数开始获取帧
        capture.read(Last);
        cv::Mat FirstResizeImage = first.Resize_Classic_Image(First, 128, 128, CV_RGB2GRAY, false);
        cv::Mat LastResizeImage = last.Resize_Classic_Image(Last, 128, 128, CV_RGB2GRAY, false);
        cv::imshow("FirstResizeImage",FirstResizeImage);//显示原图像
        first.Constructor(FirstResizeImage, false);
        last.Constructor(LastResizeImage, false);
        quantum_substracter.QuantumImage_Subtracter_Operator(first, last, result, false);
        frameToStart++;
        if(waitKey(5) >= 0)
            break;
    }*/
    double time;
    double start;
    while(frameToStart < totalFrameNumber - 1) {
        start = static_cast<double>(getTickCount());
        capture >> Last;
        //capture.set(CAP_PROP_POS_FRAMES, frameToStart);//从此时的帧数开始获取帧
        //capture.read(First);
        capture.set(CAP_PROP_POS_FRAMES, frameToStart);//从此时的帧数开始获取帧
        capture.read(Last);
        LastResizeImage = last.Resize_Classic_Image(Last, 256, 256, CV_RGB2GRAY, false);
        //cvNamedWindow("FirstResizeImage",1);
        //cv::imshow("FirstResizeImage",FirstResizeImage);//显示原图像
        //cvNamedWindow("LastResizeImage",1);
        //cv::imshow("LastResizeImage",LastResizeImage);//显示原图像
        last.Constructor(LastResizeImage, false);
        quantum_substracter.QuantumImage_Subtracter_Operator(first, last, result, false);
        //quantum_substracter.Show_Results("Quantum_Substracter");
        time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
    	std::cout << "Time of Run = " << time << " ms" << std::endl;
        frameToStart++;
        if(waitKey(5) >= 0)
            break;
    }
    //first.Constructor(FirstResizeImage, false);
    //first.Show_QuantumImage("FirstQuantumImage");												//显示NEQR量子图像
	//first.Show_ClassicImage(FirstResizeImage, "FirstClassicImage");

    //last.Constructor(LastResizeImage, false);
    //last.Show_QuantumImage("LastQuantumImage");												//显示NEQR量子图像
	//last.Show_ClassicImage(LastResizeImage, "LastClassicImage");

    //quantum_adder.QuantumImage_Adder_Operator(first, last, result, true);
    //quantum_adder.Show_Results("Quantum_Adder");

    //quantum_substracter.QuantumImage_Subtracter_Operator(first, last, result, true);
    //quantum_substracter.Show_Results("Quantum_Substracter");
    //measure.Measure(result, false);
    //cv::Mat result_classic = measure.Get_Classic_Image_Reconstructor();
    //cv::imshow("result_classic",result_classic);//显示原图像
    //result.Show_QuantumImage("Result_Subtracter");
    //measure.Show_ClassicImage("Result_Classic_Subtracter");
    //waitKey();
    //capture.release();//释放对象
    return 0;
}
