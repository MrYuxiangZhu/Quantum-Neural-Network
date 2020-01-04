/********************************************************************************
****文件名：量子BP神经网络主程序
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
#define hidden_neurons 55
#define hidden_layersnum 1
#define learning_Rate 0.5
#define error_thresh 0.01
#define train_epochs 20000
#define error_sum 999
#define input_layer (ImageHeight / (windows * convolutions)) * (ImageWidth / (windows * convolutions))
#define output_layer 10
/********************************************************************
 * 主函数
*********************************************************************/
int main(int argc, char* argv[]) 
{
    int VideoTypeNumber = output_layer;        
    int VideoNum = 9;
    int FrameNum = 20;
    
    string dataname = "./data/action_data.txt";
    string networkname = "./data/qunetwork_data.txt";
   

    vector<float> output_neuron(VideoTypeNumber, 0);//一维输出神经元数组
    vector<cell_float > action_frames(VideoNum * FrameNum, output_neuron);//二维数组，动作帧数
    vector<array_float > Output_data(VideoTypeNumber, action_frames);//三维，动作种类
    vector<array_float > Input_data;//三维数组，输入数据

    //读取数据缓存器
    vector<float> read_neuron(input_layer, 0);//一维输出神经元数组
    vector<cell_float > read_frames(VideoNum * FrameNum, read_neuron);//二维数组，动作帧数
    vector<array_float > Read_data(VideoTypeNumber, read_frames);//三维，动作种类
    
    QuantumData Qudata(VideoTypeNumber, VideoNum, FrameNum, input_layer);            //数据类
    QuantumNeuralNet QBpNeuralNet(input_layer, output_layer, hidden_neurons, hidden_layersnum, learning_Rate,\
                                 error_thresh, train_epochs, QuantumNeuralNet::COUNT, true, error_sum, false, 0);

    Qudata.AddData(Read_data, Output_data);//初始化缓存空间
    Qudata.Read_Data(dataname);            //从硬盘读数据
    Read_data = Qudata.GetInputSet();       //输入数据集
    Output_data = Qudata.GetOutputSet();    //输出数据
    /*for (int i = 0; i < Output_data.size(); ++i) {
        std::cout << "Output_data[" << i << "]= " << std::endl;
        for (int j = 0; j < Output_data[0].size(); ++j) {
            for(int k = 0; k < Output_data[0][0].size(); ++k) {
                std::cout << Output_data[i][j][k] << " " ;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl; 
    }*/
    std::cout << "Begin to train quantum bp network!" << std::endl;
    QBpNeuralNet.Train(Qudata, true);       //开始训练
    std::cout << "Train end!" << std::endl;    
    QBpNeuralNet.SaveNet(networkname);//保存网络
    
    return 0;
}
