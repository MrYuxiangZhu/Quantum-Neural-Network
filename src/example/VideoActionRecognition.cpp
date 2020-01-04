/********************************************************************************
****文件名：量子BP神经网络识别主程序
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/
#include "../Camera/camera.hpp"
#include "../BpNetwork/NeuralNet.hpp"
#include "../QuantumViBe/QuantumViBe.hpp"
#include "../QuantumNetwork/QuantumNetwork.hpp"
#include "../QuantumImagePreparation/QuantumImagePreparation.hpp"
#include "../QuantumImageFeatureExtract/QuantumImageFeatureExtract.hpp"
#include "../QuantumImageEdgeExtract/QuantumImageEdgeExtract.hpp"

typedef vector<float> cell_float;
typedef vector<cell_float > array_float;
#define ImageHeight 128
#define ImageWidth  128
#define windows 4
#define convolutions 4
#define hidden_neurons 30
#define hidden_layersnum 1
#define learning_Rate 0.5
#define error_thresh 0.003
#define train_epochs 10000
#define error_sum 999
#define input_layer (ImageHeight / windows) * (ImageWidth / windows)
#define output_layer 10

/********************************************************************
 * 记录动作函数
*********************************************************************/

/********************************************************************
 * 寻找最大值函数
*********************************************************************/
int search(vector<float>& actions_record) {
    float temp = actions_record[0];
    int max = 0;
    for (int j = 1; j < actions_record.size(); ++j) {
        if (temp < actions_record[j]) {
            temp = actions_record[j];
            max = j;
        }
    }
    return max;
}
/********************************************************************
 * 输出动作函数
*********************************************************************/
void body_action(int num) {
    switch (num) {
    case 0:
        std::cout << "动作：弯腰" << std::endl;
        break;
    case 1:
        std::cout << "动作：打拳" << std::endl;
        break;
    case 2:
        std::cout << "动作：快跑" << std::endl;
        break;
    case 3:
        std::cout << "动作：开合跳" << std::endl;
        break;
    case 4:
        std::cout << "动作：跳跃" << std::endl;
        break;
    case 5:
        std::cout << "动作：单手挥舞" << std::endl;
        break;
    case 6:
        std::cout << "动作：原地跳跃" << std::endl;
        break;
    case 7:
        std::cout << "动作：侧面行走" << std::endl;
        break;
    case 8:
        std::cout << "动作：单腿跳" << std::endl;
        break;
    case 9:
        std::cout << "动作：慢跑" << std::endl;
        break;
    case 10:
        std::cout << "动作：双手挥舞" << std::endl;
        break;
    case 11:
        std::cout << "动作：行走" << std::endl;
        break;
    }
}

/********************************************************************
 * 主函数
*********************************************************************/
int main(int argv, char *argc[])
{
    VideoCapture capture;                                          //摄像头
    if (argv > 1) {
        if (argc[1] == "0") {
            capture.open(0);                                     //设置摄像头对象
        }
        else if (argc[1] == "1") {
            capture.open(1);                                     //设置摄像头对象
        }
        else {
            capture.open(argc[1]);                               //设置摄像头对象
        }
    }
    else {
        capture.open(0);                                           //设置摄像头对象
    }
    if (!capture.isOpened()) {
		std::cout << "create camera capture error" << std::endl;
		system("pause");
		exit(-1);
	}
    capture.set(CV_CAP_PROP_FRAME_WIDTH,480);                   //摄像头像素宽度
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,640);                  //摄像头像素高度
 
    int VideoTypeNumber = output_layer;                           //动作种类
    int VideoNum = 6;                                             //每种动作视频个数
    int FramePass = 10;                                           //跳过视频帧数
    int FrameNum = 40;                                            //采集视频帧数

    string Root = "./Video/";
    string dataname = "./data/action_data.txt";
    string networkname = "./data/network_data.txt";
    string connect = "/";
    string VideoType = ".avi";
    string VideoTypeName, VideoName, Driection;

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

    vector<cell_float > FeatureImage;           //图像特征
    vector<float> QuantunData;            //神经元输入样本

    vector<float> output_neuron(VideoTypeNumber, 0);//一维输出神经元数组
    vector<float> actions_record(VideoTypeNumber, 0);//一维输出神经元数组
    vector<float> actions;
    //vector<float > action_data;                     //一维数组，动作图像数据

    QuantumData Qudata(VideoTypeNumber, VideoNum, FrameNum, input_layer);            //数据类
    NeuralNet BpNeuralNet(networkname);             //创建训练好的神经网络
    bool count = true;
    int background = 0;
    std::cout << "Read video data!" << std::endl;
    while (1) 
    {
        //count = true;
	int frames = 0;
        for (int m = 0; m < actions_record.size(); ++m) {
            actions_record[m] = 0;
        }
        background = 0;
        for (int i = 0; i < FrameNum; ++i) {
            capture >> frame;			//读入视频解码
            if(frame.empty())
                continue;
            if (count)
            {
                cv::Mat ResizeImage = QuantumImage.Resize_Classic_Image(frame, ImageHeight, ImageWidth, CV_RGB2GRAY, false);
                QuantumImage.Image_Measure(ResizeImage);
                QuantumImage.Quantum_Image_Init(ResizeImage, false);
                measure.Init(QuantumImage);
                qvibe.Init(QuantumImage);
                QuantumImage.Constructor(ResizeImage, false);
                qvibe.ProcessFirstFrame(QuantumImage);
                Edge_Extract.Quantum_Image_Edge_Extract_Init(ResizeImage);
                Feature.Init(ResizeImage, windows, windows);      
                count = false;
            }
            else {
                FeatureImage.clear();
                QuantunData.clear();
                cv::Mat ResizeImage = QuantumImage.Resize_Classic_Image(frame, ImageHeight, ImageWidth, CV_RGB2GRAY, false);
                QuantumImage.Constructor(ResizeImage, false);
                qvibe.Run(QuantumImage, false);
                FGModel = qvibe.Get_FGModel();
                Edge_Extract.Image_Edge_Extract(FGModel, Edge, false);
                Feature.Run(Edge, FeatureImage, false);                 //提取特征值
                QuantunData = Qudata.ChangeToInput(FeatureImage);              //转换成训练数据
                if(frames > FramePass)
                {
                    //std::cout << "Begin to Recongnition!" << std::endl;
                    if (BpNeuralNet.Recongnition(QuantunData, actions, false) > 0) {
                        actions_record[BpNeuralNet.Recongnition(QuantunData, actions, false) - 1] += 1; 
                        background = 1;
                    }
                }
                imshow("ResizeImage",ResizeImage);//显示原图像
                imshow("FGModel", FGModel);
                imshow("Edge", Edge);
        	    if(waitKey(10) >= 0)
            	    goto end;
                frames++;
                if (frames > FrameNum) {
                    //break;
                }
            }
        }
        if (background) {
	        body_action(search(actions_record));                                        //输出动作	  
	    }
    }
end:
    return 0;
}
