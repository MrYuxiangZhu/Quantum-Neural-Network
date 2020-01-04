/********************************************************************************
****文件名：量子BP神经网络识别主程序
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/
#include "../Camera/camera.hpp"
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
#define hidden_neurons 40
#define hidden_layersnum 1
#define learning_Rate 0.01
#define error_thresh 0.01
#define train_epochs 10000
#define error_sum 999
#define input_layer (ImageHeight / (windows * convolutions)) * (ImageWidth / (windows * convolutions))
#define output_layer 10
/********************************************************************
 * 主函数
*********************************************************************/

int main()
{
    int VideoTypeNumber = output_layer;        
    int VideoNum = 9;
    int FrameNum = 20;

    string Root = "./Video/";
    string dataname = "./data/action_data.txt";
    string networkname = "./data/qunetwork_data.txt";
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

    vector<float> FeatureImage;                    //图像特征
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
    QuantumNeuralNet QBpNeuralNet(networkname);

    vector<float> actions(VideoTypeNumber, 0);
    Qudata.AddData(Read_data, Output_data);//存入训练数据库
    Qudata.Read_Data(dataname);
    Read_data = Qudata.GetInputSet();       //输入数据集
    Output_data = Qudata.GetOutputSet();    //输出数据
    if (Read_data[1][0] == Read_data[4][10]) {
        std::cout << "error!" << std::endl;
    }
    std::cout << "Begin to read quantum bp network!" << std::endl;
    /*for(int i = 0; i < Read_data.size(); ++i) {
        for (int j = 0; j < Read_data[0].size(); ++j) {
            std::cout << "action= " << QBpNeuralNet.Recongnition(Read_data[i][j], actions, false) << std::endl;
        }
    }*/
    for (int j = 0; j < Read_data[0][1].size(); ++j) {
        std::cout << "Read_data= " << Read_data[8][10][j] << std::endl;
    }
    std::cout << "action= " << QBpNeuralNet.Recongnition(Read_data[0][1], actions, false) << std::endl;
//    for (int i = 0; i < actions.size(); ++i) {
//        std::cout << actions[i] << std::endl;
//    }
    return 0;
}
