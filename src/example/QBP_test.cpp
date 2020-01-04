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

#define hidden_neurons 30
#define hidden_layersnum 1
#define learning_Rate 0.05
#define error_thresh 0.5
#define train_epochs 10000
#define error_sum 999
#define input_layer 10
#define output_layer 5

/********************************************************************
 * 主函数
*********************************************************************/
int main(int argc, char* argv[]) 
{
    string networkname = "./data/qunetwork_test.txt";
    vector<iovector > input_temp;
    vector<iovector > output_temp;
    vector<float_cell > input_data;
    //vector<float_cell > output_data;

    vector<float> cell(5, 0);
    vector<iovector> float_cells(5, cell);
    vector<float_cell > output_data(1, float_cells);

    for (int i = 0; i < 5; ++i)
    {
        output_data[0][i][i] = 1;
    }
    for (int i = 0; i < 5; ++i)
    {
        for(int j = 0; j < 5; ++j) {
            std::cout << output_data[0][i][j] << " ";
        }
        std::cout << std::endl;
    }
       //训练数据
    vector<float> input1;
    input1.push_back(0.125);
    input1.push_back(0.245);
    input1.push_back(0.0);
    input1.push_back(0.0);
    input1.push_back(0.0);
    input1.push_back(0.0);
    input1.push_back(0.0);
    input1.push_back(0.0);
    input1.push_back(0.0);
    input1.push_back(0.0);
    //vector<float> output1;
    //output1.push_back(0.0);
    
    input_temp.push_back(input1);
    //output_temp.push_back(output1);

    vector<float> input2;
    input2.push_back(1.123);
    input2.push_back(0.0);
    input2.push_back(0.553);
    input2.push_back(0.0);
    input2.push_back(0.0);
    input2.push_back(0.0);
    input2.push_back(0.0);
    input2.push_back(0.0);
    input2.push_back(0.0);
    input2.push_back(0.0);
    //vector<float> output2;
    //output2.push_back(0.0);

    input_temp.push_back(input2);
    //output_temp.push_back(output2);

    vector<float> input3;
    input3.push_back(0.0);
    input3.push_back(0.0);
    input3.push_back(1.0);
    input3.push_back(0.0);
    input3.push_back(0.0);
    input3.push_back(0.0);
    input3.push_back(0.556);
    input3.push_back(0.0);
    input3.push_back(0.0);
    input3.push_back(1.345);
    //vector<float> output3;
    //output3.push_back(1.0);

    input_temp.push_back(input3);
    //output_temp.push_back(output3);

    vector<float> input4;
    input4.push_back(0.0);
    input4.push_back(0.0);
    input4.push_back(0.5);
    input4.push_back(0.0);
    input4.push_back(0.0);
    input4.push_back(0.0);
    input4.push_back(1.0);
    input4.push_back(0.0);
    input4.push_back(0.0);
    input4.push_back(0.0);
    //vector<float> output4;
    //output4.push_back(0.0);

    input_temp.push_back(input4);
    //output_temp.push_back(output4);

    vector<float> input5;
    input5.push_back(0.0);
    input5.push_back(1.0);
    input5.push_back(0.0);
    input5.push_back(0.0);
    input5.push_back(0.0);
    input5.push_back(0.5);
    input5.push_back(0.0);
    input5.push_back(0.0);
    input5.push_back(0.0);
    input5.push_back(0.0);
    //vector<float> output5;
    //output5.push_back(0.0);

    input_temp.push_back(input5);
    //output_temp.push_back(output5);

    input_data.push_back(input_temp);
    //output_data.push_back(output_temp);

    std::cout << input_data.size() <<std::endl;
    std::cout << input_data[0].size() <<std::endl;
    std::cout << input_data[0][0].size() <<std::endl;

    std::cout << output_data.size() <<std::endl;
    std::cout << output_data[0].size() <<std::endl;
    std::cout << output_data[0][0].size() <<std::endl;

    //建立一个数据类
    QuantumData MyData(1,1,5,10);//2个输入，一个输出
    MyData.AddData(input_data,output_data);//添加数据

    QuantumNeuralNet QBpNeuralNet(input_layer, output_layer, hidden_neurons, hidden_layersnum, learning_Rate,\
                                 error_thresh, train_epochs, QuantumNeuralNet::COUNT, true, error_sum, false, 0);
    
    QBpNeuralNet.Train(MyData, false);//通过数据，开始训练
    //std::cout << QBpNeuralNet.Update(input_data[0][0])[0] << std::endl;//通过输入得到输出
    //std::cout << QBpNeuralNet.Update(input_data[0][1])[0] << std::endl;
    //std::cout << QBpNeuralNet.Update(input_data[0][2])[0] << std::endl;
    //std::cout << QBpNeuralNet.Update(input_data[0][3])[0] << std::endl;
    //std::cout << QBpNeuralNet.Update(input_data[0][4])[0] << std::endl;
    vector<float> output_temps;
    output_temps = QBpNeuralNet.Update(input_data[0][0]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;
    output_temps = QBpNeuralNet.Update(input_data[0][1]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;
    output_temps = QBpNeuralNet.Update(input_data[0][2]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;
    output_temps = QBpNeuralNet.Update(input_data[0][3]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;

    output_temps = QBpNeuralNet.Update(input_data[0][4]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;
    
    QBpNeuralNet.SaveNet(networkname);//保存网络
    
    QuantumNeuralNet QBpNeuralNet2(networkname);//通过文件读取网络
    //std::cout << QBpNeuralNet2.Update(input_data[0][0])[0] << std::endl;//通过输入得到输出
    //std::cout << QBpNeuralNet2.Update(input_data[0][1])[0] << std::endl;
    //std::cout << QBpNeuralNet2.Update(input_data[0][2])[0] << std::endl;
    //std::cout << QBpNeuralNet2.Update(input_data[0][3])[0] << std::endl;
    //std::cout << QBpNeuralNet2.Update(input_data[0][4])[0] << std::endl; 

    output_temps = QBpNeuralNet2.Update(input_data[0][0]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;

    output_temps = QBpNeuralNet2.Update(input_data[0][1]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;

    output_temps = QBpNeuralNet2.Update(input_data[0][2]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;

    output_temps = QBpNeuralNet2.Update(input_data[0][3]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;

    output_temps = QBpNeuralNet2.Update(input_data[0][4]);
    for(int j = 0; j < 5; ++j) {
        std::cout << output_temps[j] << " ";
    }
    std::cout << std::endl;

    return 0;
}
