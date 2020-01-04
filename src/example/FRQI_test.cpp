/********************************************************************************
****文件名：FRQI量子图像构建过程测试
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/12/7
*********************************************************************************/

#include "../QuantumImagePreparation/QuantumImagePreparation.hpp"

int main(int argc, char *argv[]) 
{
    vector<float> temp;
    vector<vector<float> >QuantumImage;		//存储量子图像
    for (int i = 0; i < 128; ++i) {
        for(int j = 0; j < 128; ++j) {
            temp.push_back((float)j / 10);
        }
        QuantumImage.push_back(temp);
        temp.clear();
    }
    FRQI frqi;
    frqi.Image_Measure(QuantumImage, false);
    frqi.Constructor(QuantumImage, true);
    return 0;
}