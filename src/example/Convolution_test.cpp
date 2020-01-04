/********************************************************************************
****文件名：量子图像特征提取
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/11/19
*********************************************************************************/
#include "../QuantumImageFeatureExtract/QuantumImageFeatureExtract.hpp"
#include "../QuantumConvolution/QuantumConvolution.hpp"
#include "../QuantumImageEdgeExtract/QuantumImageEdgeExtract.hpp"
#define windows 16

int main(int argc, char* argv[])
{
    int count = 0;
    vector<float> temp(64, 0);
    vector<vector<float> > input(64, temp);
    vector<vector<float> > output;

    Convolution_Operator convolution;

    for(int i = 0; i < 64; ++i) {
        for(int j = 0; j < 64;) {
            input[i][j] = 1;
            j = j + 4;
        }
    }
    for(int i = 0; i < 64; ++i) {
        for(int j = 0; j < 64; ++j) {
            std::cout << input[i][j] << " ";
        }
        std::cout << std::endl;
    }
    convolution.Init(input, windows, windows);
    convolution.Run(input, output, false);
    for(int i = 0; i < output.size(); ++i) {
        for(int j = 0; j < output[0].size(); ++j) {
            std::cout << output[i][j] << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}