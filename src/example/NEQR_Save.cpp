/********************************************************************************
****文件名：NEQR量子图像构建过程测试
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/12/7
*********************************************************************************/

#include "../QuantumImagePreparation/QuantumImagePreparation.hpp"

int main(int argc, char* argv[])
{
    string root = "../../Video/";
    string dataroot = "./data/";
    string videotype = ".avi";
    string filenum, direction;
    filenum = to_string(1);
    direction = root + filenum + videotype;
    std::cout << direction << std::endl;
    NEQR_Save(dataroot + "test.txt", "./Video/Camera Road 01.avi");
    return 0;
}