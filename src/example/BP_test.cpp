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

typedef vector<float> iovector;
typedef vector<iovector > float_cell;

#define hidden_neurons 4
#define hidden_layersnum 1
#define learning_Rate 0.1
#define error_thresh 0.01
#define train_epochs 100000
#define error_sum 0
#define input_layer 2
#define output_layer 1

/********************************************************************
 * 主函数
*********************************************************************/
int main(int argc, char* argv[]) 
{
    string networkname = "./data/network_test.txt";
    vector<iovector > input_temp;
    vector<iovector > output_temp;
    vector<float_cell > input_data;
    vector<float_cell > output_data;
       //训练数据
    vector<float> input1;
    //input1.push_back(1.0);
    input1.push_back(2.0);
    input1.push_back(1.0);
    vector<float> output1;
    output1.push_back(0.0);

    input_temp.push_back(input1);
    output_temp.push_back(output1);

    vector<float> input2;
    input2.push_back(1.0);
    input2.push_back(-1.0);
    vector<float> output2;
    output2.push_back(1.0);

    input_temp.push_back(input2);
    output_temp.push_back(output2);

    vector<float> input3;
    input3.push_back(-1.0);
    input3.push_back(-1.0);
    vector<float> output3;
    output3.push_back(0.0);

    input_temp.push_back(input3);
    output_temp.push_back(output3);

    vector<float> input4;
    input4.push_back(-1.0);
    input4.push_back(1.0);
    vector<float> output4;
    output4.push_back(1.0);

    input_temp.push_back(input4);
    output_temp.push_back(output4);

    input_data.push_back(input_temp);
    output_data.push_back(output_temp);

    std::cout << input_data.size() <<std::endl;
    std::cout << input_data[0].size() <<std::endl;
    std::cout << input_data[0][0].size() <<std::endl;

    std::cout << output_data.size() <<std::endl;
    std::cout << output_data[0].size() <<std::endl;
    std::cout << output_data[0][0].size() <<std::endl;

    //建立一个数据类
    Data MyData(1,1,8,2);//2个输入，一个输出
    MyData.AddData(input_data,output_data);//添加数据

    NeuralNet Brain(input_layer, output_layer, hidden_neurons, hidden_layersnum, learning_Rate,\
                                 error_thresh, train_epochs, NeuralNet::COUNT, true, error_sum, false, 0);//新建一个神经网络，输入神经元个数，输出神经元个数，隐藏层神经元个数，学习率，停止训练方法（次数或误差最小），是否输出误差值（用于观察是否收敛）
    Brain.Train(MyData);//通过数据，开始训练
    vector<float> outputs;
    outputs = Brain.Update(input_data[0][0]);
    std::cout << outputs.size() << std::endl;
    std::cout << Brain.Update(input_data[0][0])[0] << std::endl;//通过输入得到输出
    std::cout << Brain.Update(input_data[0][1])[0] << std::endl;
    std::cout << Brain.Update(input_data[0][2])[0] << std::endl;
    std::cout << Brain.Update(input_data[0][3])[0] << std::endl;
    
    Brain.saveNet(networkname);//保存网络
    
    NeuralNet Brain2(networkname);//通过文件读取网络
    std::cout << Brain2.Update(input_data[0][0])[0] << std::endl;//通过输入得到输出
    std::cout << Brain2.Update(input_data[0][1])[0] << std::endl;
    std::cout << Brain2.Update(input_data[0][2])[0] << std::endl;
    std::cout << Brain2.Update(input_data[0][3])[0] << std::endl;
    return 0;
}
