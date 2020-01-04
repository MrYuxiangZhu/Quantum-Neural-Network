/********************************************************************************
****文件名：量子BP神经网络识别主程序
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/
#include "../Camera/camera.hpp"
#include "../QuantumConvolution/QuantumConvolution.hpp"
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
#define hidden_neurons 100
#define hidden_layersnum 1
#define learning_Rate 0.1
#define error_thresh 0.003
#define train_epochs 10000
#define error_sum 999
//#define input_layer (ImageHeight / (windows * convolutions)) * (ImageWidth / (windows * convolutions))
#define input_layer (ImageHeight / windows) * (ImageWidth / windows)
#define output_layer 10
/********************************************************************
 * 主函数
*********************************************************************/
int main(int argc, char* argv[]) 
{
    int VideoTypeNumber = output_layer;                             //动作种类
    int VideoNum = 9;                                               //每种动作视频个数
    int FramePass = 3;                                            //跳过视频帧数
    int FrameNum = 10;                                             //采集视频帧数
    int qucount = 0;

    string Root = "./Video/";
    string dataname = "./data/action_data.txt";
    string networkname = "./data/network_data.txt";
    string connect = "/";
    string VideoType = ".avi";
    string VideoTypeName, VideoName, Driection;
    VideoCapture capture;
   
     // 程序运行时间统计变量
    // the Time Statistical Variable of Program Running Time
    double time;
    double start;

    cv::Mat First, frame, gray, FGModel, Edge;

    QViBe qvibe[VideoTypeNumber * VideoNum];
    NEQR QuantumImage[VideoTypeNumber * VideoNum];
    Quantum_Measure measure[VideoTypeNumber * VideoNum];
    Quantum_Image_Edge_Extract Edge_Extract[VideoTypeNumber * VideoNum];
    Quantum_Image_Feature_Extract Feature[VideoTypeNumber * VideoNum];
    Convolution_Operator convolution[VideoTypeNumber * VideoNum];

    vector<cell_float > FeatureImage;               //图像特征
    vector<cell_float > ConvolutionImage;          //卷积图像
    vector<float> QuantunData;                     //神经元输入样本

    vector<float> output_neuron(VideoTypeNumber, 0);//一维输出神经元数组
    vector<cell_float > action_frames(VideoNum * FrameNum, output_neuron);//二维数组，动作帧数
    vector<array_float > Output_data(VideoTypeNumber, action_frames);//三维，动作种类

    vector<cell_float > action_data;//二维数组，动作图像数据
    vector<array_float > Input_data;//三维数组，输入数据

    //读取数据缓存器
    vector<float> read_neuron(input_layer, 0);//一维输出神经元数组
    vector<cell_float > read_frames(VideoNum * FrameNum, read_neuron);//二维数组，动作帧数
    vector<array_float > Read_data(VideoTypeNumber, read_frames);//三维，动作种类
    
    QuantumData Qudata(VideoTypeNumber, VideoNum, FrameNum, input_layer);            //数据类
    NeuralNet BpNeuralNet(networkname);
    

    bool count = true;
    for(int i = 0; i < VideoTypeNumber; ++i) {
        for (int j = 0; j < VideoNum * FrameNum; ++j) {
            for (int k = 0; k < VideoTypeNumber; ++k) {
                if (i == k) {
                    Output_data[i][j][k] = 1; 
                }
                else {
                    Output_data[i][j][k] = 0; 
                }
            }
        }
    }
    //capture = VideoCapture("./Video/3/3.avi");
    //long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
    //std::cout << "total frames: " << totalFrameNumber << std::endl;
    std::cout << "Read video data!" << std::endl;
    start = static_cast<float>(getTickCount());
    qucount = 0;
    for (int i = 1; i <= VideoTypeNumber; ++i) {
        VideoTypeName = to_string(i);
        action_data.clear();                    //每种动作二维数组先清空
        for (int j = 1; j <= VideoNum; ++j) {
            VideoName = to_string(j);
            Driection = Root + VideoTypeName + connect + VideoName + VideoType;
            capture = VideoCapture(Driection);
            //long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
            //std::cout << "total frames: " << totalFrameNumber << std::endl;
            //std::cout << Driection << std::endl;
            if(!capture.isOpened())
            {
                std::cout << "ERROR: Did't find this video!" << std::endl;
                return 0;
            }
            count = true;
            int frames = 0;
            while(1) {
                capture >> frame;
                if (frame.empty()) {
                    continue;
                }
                else {
                    if (count)
                    {
                        FeatureImage.clear();
                        QuantunData.clear();
                        ConvolutionImage.clear();
                        cv::Mat ResizeImage = QuantumImage[qucount].Resize_Classic_Image(frame, ImageHeight, ImageWidth, CV_RGB2GRAY, false);
                        QuantumImage[qucount].Image_Measure(ResizeImage);
                        QuantumImage[qucount].Quantum_Image_Init(ResizeImage, false);
                        measure[qucount].Init(QuantumImage[qucount]);
                        qvibe[qucount].Init(QuantumImage[qucount]);
                        QuantumImage[qucount].Constructor(ResizeImage, false);
                        qvibe[qucount].ProcessFirstFrame(QuantumImage[qucount]);
                        Edge_Extract[qucount].Quantum_Image_Edge_Extract_Init(ResizeImage);
                        Feature[qucount].Init(ResizeImage, windows, windows);
                        Edge_Extract[qucount].Image_Edge_Extract(ResizeImage, Edge, false);
                        Feature[qucount].Run(Edge, FeatureImage, false);                     //提取特征值
                        //convolution[qucount].Init(FeatureImage, convolutions, convolutions);
                        //convolution[qucount].Run(FeatureImage, ConvolutionImage, false);
                        //imshow("Edge", Edge);
                        //Feature[qucount].ShowFeatureImage("Feature");
                        //convolution[qucount].ShowFeatureImage("Convolution");
                        //QuantunData = Qudata.ChangeToInput(ConvolutionImage);       //转换成训练数据
                        //action_data.push_back(QuantunData);                                 //特征点集                    
                        count = false;
                    }
                    else
                    {
                        FeatureImage.clear();
                        QuantunData.clear();
                        ConvolutionImage.clear();
                        cv::Mat ResizeImage = QuantumImage[qucount].Resize_Classic_Image(frame, ImageHeight, ImageWidth, CV_RGB2GRAY, false);
                        QuantumImage[qucount].Constructor(ResizeImage, false);
                        qvibe[qucount].Run(QuantumImage[qucount], false);
                        FGModel = qvibe[qucount].Get_FGModel();
                        Edge_Extract[qucount].Image_Edge_Extract(FGModel, Edge, false);
                        Feature[qucount].Run(Edge, FeatureImage, false);                     //提取特征值
                        //convolution[qucount].Run(FeatureImage, ConvolutionImage, false);
                        //Feature[qucount].ShowFeatureImage("Feature");
                        //convolution[qucount].ShowFeatureImage("Convolution");
                        if(frames > FramePass)
                        {
                            imshow("ResizeImage",ResizeImage);//显示原图像
                            imshow("FGModel", FGModel);
                            imshow("Edge", Edge);
                            QuantunData = Qudata.ChangeToInput(FeatureImage);           //转换成训练数据
                            action_data.push_back(QuantunData);                         //特征点集
                        }
                    }
                    if(waitKey(10) >= 0)
                        break;
                    frames++;
                    if (frames > FrameNum + FramePass) {
                        break;
                    }
                }
            }
            qucount++;
        }
        Input_data.push_back(action_data);  //动作数据集               
    }
    time = ((float)getTickCount() - start) / getTickFrequency() * 1000;

       
    vector<float> actions(VideoTypeNumber, 0);

    //Qudata.AddData(Input_data, Output_data);//存入训练数据库
    //Read_data = Qudata.GetInputSet();       //输入数据集
    //Output_data = Qudata.GetOutputSet();    //输出数据

    std::cout << "Begin to read bp network!" << std::endl;
    std::cout << "accuracy= " << BpNeuralNet.RecognitionAccuracy(Input_data, Output_data, VideoTypeNumber * VideoNum * FrameNum, false) * 100 << "%" << std::endl;
    std::cout << "Recognition End!" << std::endl;
    return 0;
}
