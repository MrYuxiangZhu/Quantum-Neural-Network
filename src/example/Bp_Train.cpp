/********************************************************************************
****文件名：量子BP神经网络主程序
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/
#include "../Camera/camera.hpp"
#include "../QuantumViBe/QuantumViBe.hpp"
#include "../BpNetwork/NeuralNet.hpp"
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
#define learning_Rate 0.5
#define error_thresh 0.03
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
    int FrameNum = 20;                                             //采集视频帧数

    string dataname = "./data/action_data.txt";
    string networkname = "./data/network_data.txt";
 

    vector<float> output_neuron(VideoTypeNumber, 0);//一维输出神经元数组
    vector<cell_float > action_frames(VideoNum * FrameNum, output_neuron);//二维数组，动作帧数
    vector<array_float > Output_data(VideoTypeNumber, action_frames);//三维，动作种类

    vector<cell_float > action_data;//二维数组，动作图像数据
    vector<array_float > Input_data;//三维数组，输入数据

    //读取数据缓存器
    vector<float> read_neuron(input_layer, 0);//一维输出神经元数组
    vector<cell_float > read_frames(VideoNum * FrameNum, read_neuron);//二维数组，动作帧数
    vector<array_float > Read_data(VideoTypeNumber, read_frames);//三维，动作种类
    
    
    Data data(VideoTypeNumber, VideoNum, FrameNum, input_layer);            //数据类
    NeuralNet BpNeuralNet(input_layer, output_layer, hidden_neurons, hidden_layersnum, learning_Rate,\
                                 error_thresh, train_epochs, NeuralNet::ERRORSUM, true, error_sum, false, 0);
    
    //动作种类与输出神经元关系
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
    data.AddData(Read_data, Output_data);//初始化缓存空间
    data.Read_Data(dataname);            //从硬盘读数据
    Read_data = data.GetInputSet();       //输入数据集
    Output_data = data.GetOutputSet();    //输出数据
    std::cout << "Begin to train quantum bp network!" << std::endl;
    BpNeuralNet.Train(data);       //开始训练
    std::cout << "Train end!" << std::endl;    
    BpNeuralNet.saveNet(networkname);//保存网络
    return 0;
}
