/********************************************************************************
****文件名：BP神经网络
****作  者：朱宇祥
****版  本：1.0
****日  期：2018/8/13
*********************************************************************************/

#include "NeuralNet.hpp"

/******************************************************************************
********数据输入构造
*******************************************************************************/
Data::Data(int VideoType, int VideoNum, int Frames, int datas):VideoTypeVectorSize(VideoType), VideoNumVectorSize(VideoNum), FrameNums(Frames), EachNums(datas)
{

}
/******************************************************************************
********创建数据空间
*******************************************************************************/
void Data::Create_Data(vector<float_cell >& input, vector<float_cell >& output) {
    vector<float> output_neuron(VideoTypeVectorSize, 0);//一维输出神经元数组
    vector<iovector > action_frames(VideoNumVectorSize * FrameNums, output_neuron);//二维数组，动作帧数
    vector<float_cell > Output_data(VideoTypeVectorSize, action_frames);//三维，动作种类
    for(int i = 0; i < VideoTypeVectorSize; ++i) {
        for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) {
            for (int k = 0; k < VideoTypeVectorSize; ++k) {
                if (i == k) {
                    Output_data[i][j][k] = 1; 
                }
                else {
                    Output_data[i][j][k] = 0; 
                }
            }
        }
    }
    output = Output_data;
}
/********************************************************************************
*******数据写入硬盘
********************************************************************************/
void Data::Write_Data(string filename) {
    ofstream WriteData(filename);
    string output;
    output = to_string(this->EachNums);
    WriteData << output << endl;
    output = to_string(this->FrameNums);
    WriteData << output << endl;
    output = to_string(this->VideoNumVectorSize);
    WriteData << output << endl;
    output = to_string(this->VideoTypeVectorSize);
    WriteData << output << endl;
    for (int i = 0; i < VideoTypeVectorSize; ++i) {
        for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) {
            for (int k = 0; k < EachNums; ++k) {
                output = to_string(this->SetIn[i][j][k]);
                WriteData << output << endl;
            }
        }
    }
    for (int i = 0; i < VideoTypeVectorSize; ++i) {
        for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) {
            for (int k = 0; k < VideoTypeVectorSize; ++k) {
                output = to_string(this->SetOut[i][j][k]);
                WriteData << output << endl;
            }
        }
    }
    WriteData.clear(); //为了代码具有移植性和复用性, 这句最好带上,清除标志位.有些系统若不清理可能会出现问题.
	WriteData.close();
}
/********************************************************************************
*******从硬盘读数据
********************************************************************************/
void Data::Read_Data(string filename) {
    char buffer[100];
    ifstream ReadData(filename);
    if (ReadData.is_open())
	{
        ReadData.getline(buffer, 100);
        this->EachNums = stringToNum<int>(buffer);
        ReadData.getline(buffer, 100);
        this->FrameNums = stringToNum<int>(buffer);
        ReadData.getline(buffer, 100);
        this->VideoNumVectorSize = stringToNum<int>(buffer);
        ReadData.getline(buffer, 100);
        this->VideoTypeVectorSize = stringToNum<int>(buffer);
        for (int i = 0; i < VideoTypeVectorSize; ++i) {
            for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) {
                for (int k = 0; k < EachNums; ++k) {
                    ReadData.getline(buffer, 100);
                    this->SetIn[i][j][k] = stringToNum<float>(buffer);
                }
            }
        }
        for (int i = 0; i < VideoTypeVectorSize; ++i) {
            for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) {
                for (int k = 0; k < VideoTypeVectorSize; ++k) {
                    ReadData.getline(buffer, 100);
                    this->SetOut[i][j][k] = stringToNum<float>(buffer);
                }
            }
        }
    }
    ReadData.clear(); //为了代码具有移植性和复用性, 这句最好带上,清除标志位.有些系统若不清理可能会出现问题.
	ReadData.close();
}
/******************************************************************************
********数据输入析构
*******************************************************************************/
Data::~Data()
{
	
}

/******************************************************************************
********转移函数
*******************************************************************************/
float NeuralNet::Sigmoid(float activation, float response){
    return ( 1 / ( 1 + exp(-activation / response)));
}
/******************************************************************************
********转移函数
*******************************************************************************/
float NeuralNet::atanderivate(float activation) {
    return (1 / (1 + activation * activation));
}
/******************************************************************************
********初始化网络
*******************************************************************************/
NeuralNet::NeuralNet(int inputlayer,int outputlayer,int hiddenneurons,int hiddenLayersnum,float learningRate,float errorthresh,long int trainepochs,STOPTYPE type,bool debug,float errorsum,bool trained,int numepochs):
	NumInputs(inputlayer),
    NumOutputs(outputlayer),
    NeuronsPerHiddenLayer(hiddenneurons),
	NumHiddenLayers(hiddenLayersnum),
    LearningRate(learningRate),
    ERROR_THRESHOLD(errorthresh),
    TrainCount(trainepochs),
    StopType(type),
    Debug(debug),
    ErrorSum(errorsum),
    Trained(trained),
    NumEpochs(numepochs){
    CreateNet();
}
/********************************************************************************
*******创建网络
********************************************************************************/
void NeuralNet::CreateNet(){
    if(NumHiddenLayers>0){
        vecLayers.push_back(NeuronLayer(NeuronsPerHiddenLayer,NumInputs));		//输入层
		for(int i = 0;i < NumHiddenLayers - 1; ++i){
			vecLayers.push_back(NeuronLayer(NeuronsPerHiddenLayer,NeuronsPerHiddenLayer));//隐层
		} 
        vecLayers.push_back(NeuronLayer(NumOutputs,NeuronsPerHiddenLayer));		//输出层
    }
    else{
        vecLayers.push_back(NeuronLayer(NumOutputs,NumInputs));
    }
}
/********************************************************************************
*******初始化神经网络权值
********************************************************************************/
void NeuralNet::InitializeNetwork(){
    for(int i = 0;i < NumHiddenLayers + 1; ++i){								//权值维数比隐层数多1
		for(int n = 0; n < vecLayers[i].NumNeurons; ++n){
			for(int k = 0;k < vecLayers[i].vecNeurons[n].NumInputs; ++k){
				vecLayers[i].vecNeurons[n].vecWeight[k] = RandomClamped();//随机生成权重
			}                
		}            
	}        
    //ErrorSum=9999;
    //NumEpochs=0;
}
/********************************************************************************
*******更新网络权值
********************************************************************************/
vector<float> NeuralNet::Update(vector<float> inputs){
    vector<float> outputs;
    int cWeight = 0;
    if (inputs.size() != NumInputs)
        return outputs;
    for(int i = 0;i < NumHiddenLayers + 1; ++i){
        if(i>0)
            inputs=outputs;
        outputs.clear();
        cWeight = 0;
        for(int n = 0; n < vecLayers[i].NumNeurons; ++n){
            float netinput=0;
            int	numInputs = vecLayers[i].vecNeurons[n].NumInputs;
            for (int k = 0; k < numInputs-1; ++k) {
                netinput+=vecLayers[i].vecNeurons[n].vecWeight[k]*inputs[cWeight++];
            }
            netinput += vecLayers[i].vecNeurons[n].vecWeight[numInputs - 1] * (-1);
            vecLayers[i].vecNeurons[n].Activation = Sigmoid(netinput, ACTIVATION_RESPONSE);
            outputs.push_back(vecLayers[i].vecNeurons[n].Activation);//即输出
            cWeight = 0;
        }
    }
    return outputs;
}

/********************************************************************************
*******训练网络子函数
********************************************************************************/
bool NeuralNet::NetworkTrainingEpoch(vector<iovector> &SetIn, vector<iovector> &SetOut){
    vector<float>::iterator curWeight;//指向某个权重
    vector<Neuron>::iterator curNrnOut, curNrnHid;//指向输出神经元和某个隐藏神经元
    ErrorSum=0;//置零
    //对每一个输入集合调整权值
    for(unsigned int vec = 0;vec < SetIn.size(); vec++){
        vector<float> outputs=Update(SetIn[vec]);//通过神经网络获得输出
        //根据每一个输出神经元的输出调整权值
        for(int op = 0;op < NumOutputs; op++){
            float err = (SetOut[vec][op]-outputs[op]) * outputs[op] * (1 - outputs[op]);//误差的平方
            ErrorSum += (SetOut[vec][op] - outputs[op]) * (SetOut[vec][op] - outputs[op]);//计算误差总和，用于暂停训练
            vecLayers[1].vecNeurons[op].Error = err;//更新误差（输出层）
            curWeight = vecLayers[1].vecNeurons[op].vecWeight.begin();//标记第一个权重
            curNrnHid = vecLayers[0].vecNeurons.begin();//标记隐藏层第一个神经元
            //对该神经元的每一个权重进行调整
            while(curWeight != vecLayers[1].vecNeurons[op].vecWeight.end() - 1){
                *curWeight += err * LearningRate * curNrnHid->Activation;//根据误差和学习率和阈值调整权重
                curWeight++;//指向下一个权重
                curNrnHid++;//指向下一个隐藏层神经元
            }
            *curWeight += err * LearningRate * (-1);//偏移值
        }
        curNrnHid = vecLayers[0].vecNeurons.begin();//重新指向隐藏层第一个神经元
        int n = 0;
        //对每一个隐藏层神经元
        while(curNrnHid != vecLayers[0].vecNeurons.end() - 1){
            float err = 0;
            curNrnOut = vecLayers[1].vecNeurons.begin();//指向第一个输出神经元
            //对每一个输出神经元的第一个权重
            while(curNrnOut != vecLayers[1].vecNeurons.end()){
                err += curNrnOut->Error * curNrnOut->vecWeight[n];//某种计算误差的方法(BP)
                curNrnOut++;
            }
            err *= curNrnHid->Activation * (1 - curNrnHid->Activation);//某种计算误差的方法(BP)
            for(int w = 0; w < NumInputs; w++)
                curNrnHid->vecWeight[w] += err * LearningRate * SetIn[vec][w];//根据误差更新隐藏层的权重
            curNrnHid->vecWeight[NumInputs] += err * LearningRate * (-1);//偏移值
            curNrnHid++;//下一个隐藏层神经元
            n++;//下一个权重
        }
    }
    return 1;
}
/********************************************************************************
*******训练网络
********************************************************************************/
bool NeuralNet::Train(Data data){
    std::cout << "Training..." << std::endl;
    if(Trained == 1){
        return false;		
	}
    vector<float_cell > SetIn = data.GetInputSet();
    vector<float_cell > SetOut = data.GetOutputSet();
    InitializeNetwork();
	
    long int TrainedCount = 0;
    //训练值差作为停止标志
    /*while (TrainedCount < TrainCount) {
        for (int i = 0; i < SetIn.size(); ++i) {
            for (int j = 0; j < 20; ++j) {
                std::cout << "#";
            }
            NetworkTrainingEpoch(SetIn[i], SetOut[i]);
            std::cout << std::endl;
            std::cout << "ErrorSum= " << ErrorSum << std::endl;
        }
        TrainedCount++;
        std::cout << "TrainedCount= " << TrainedCount << std::endl;
    }*/
	//最大训练值作为停止标志
    if(StopType == COUNT) {
        long int i = TrainCount;
        while(i--) {
            if(Debug)
                cout << "ErrorSum:" << ErrorSum << endl;
            //NetworkTrainingEpoch(SetIn,SetOut);
            for (int i = 0; i < SetIn.size(); ++i) {
                NetworkTrainingEpoch(SetIn[i], SetOut[i]);
            }
        }
    }
	//最大误差值作为停止标志
    else {
        while(ErrorSum > ERROR_THRESHOLD){
            if(Debug)
                std::cout << "ErrorSum:" << ErrorSum << std::endl;
            for (int i = 0; i < SetIn.size(); ++i) {
                NetworkTrainingEpoch(SetIn[i], SetOut[i]);
            }
        }
    }
    Trained = true;
    std::cout << "Done!!!" << std::endl;
    return true;
}
/********************************************************************************
*******读取网络权值
********************************************************************************/
NeuralNet::NeuralNet(string filename) {
    char buffer[100];
    ifstream NeuralNetData(filename);
    if (NeuralNetData.is_open())
	{
        NeuralNetData.getline(buffer, 100);
        this->NumInputs = stringToNum<int>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->NumOutputs = stringToNum<int>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->NumHiddenLayers = stringToNum<int>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->NeuronsPerHiddenLayer = stringToNum<int>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->LearningRate = stringToNum<float>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->ErrorSum = stringToNum<float>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->Trained = stringToNum<bool>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->NumEpochs = stringToNum<int>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->ERROR_THRESHOLD = stringToNum<float>(buffer);
        NeuralNetData.getline(buffer, 100);
        this->TrainCount = stringToNum<long int>(buffer);
        this->CreateNet();
        for(int i = 0; i < this->vecLayers.size(); i++) {
            NeuralNetData.getline(buffer, 100);
            this->vecLayers[i].NumNeurons = stringToNum<int>(buffer);
            for(int j = 0;j < this->vecLayers[i].vecNeurons.size(); j++) {
                NeuralNetData.getline(buffer, 100);
                this->vecLayers[i].vecNeurons[j].NumInputs = stringToNum<int>(buffer);
                NeuralNetData.getline(buffer, 100);
                this->vecLayers[i].vecNeurons[j].Activation = stringToNum<float>(buffer);
                NeuralNetData.getline(buffer, 100);
                this->vecLayers[i].vecNeurons[j].Error = stringToNum<float>(buffer);
                for(int k=0;k<this->vecLayers[i].vecNeurons[j].vecWeight.size();k++) {
                    NeuralNetData.getline(buffer, 100);
                    this->vecLayers[i].vecNeurons[j].vecWeight[k] = stringToNum<float>(buffer);
                }
            }
        }
    }
    NeuralNetData.clear(); //为了代码具有移植性和复用性, 这句最好带上,清除标志位.有些系统若不清理可能会出现问题.
	NeuralNetData.close();
}
/********************************************************************************
*******保存网络权值
********************************************************************************/
void NeuralNet::saveNet(string filename) {
    ofstream SaveNetData(filename);
    string output;
    output = to_string(this->NumInputs);
    SaveNetData << output << endl;
    output = to_string(this->NumOutputs);
    SaveNetData << output << endl;
    output = to_string(this->NumHiddenLayers);
    SaveNetData << output << endl;
    output = to_string(this->NeuronsPerHiddenLayer);
    SaveNetData << output << endl;
    output = to_string(this->LearningRate);
    SaveNetData << output << endl;
    output = to_string(this->ErrorSum);
    SaveNetData << output << endl;
    output = to_string(this->Trained);
    SaveNetData << output << endl;
    output = to_string(this->NumEpochs);
    SaveNetData << output << endl;
    output = to_string(this->ERROR_THRESHOLD);
    SaveNetData << output << endl;
    output = to_string(this->TrainCount);
    SaveNetData << output << endl;
    for(int i = 0; i < this->vecLayers.size(); i++) {
        output = to_string(this->vecLayers[i].NumNeurons);
        SaveNetData << output << endl;
        for(int j = 0; j < this->vecLayers[i].vecNeurons.size(); j++) {
            output = to_string(this->vecLayers[i].vecNeurons[j].NumInputs);
            SaveNetData << output << endl;
            output = to_string(this->vecLayers[i].vecNeurons[j].Activation);
            SaveNetData << output << endl;
            output = to_string(this->vecLayers[i].vecNeurons[j].Error);
            SaveNetData << output << endl;
            for(int k = 0; k < this->vecLayers[i].vecNeurons[j].vecWeight.size(); k++) {
                output = to_string(this->vecLayers[i].vecNeurons[j].vecWeight[k]);
                SaveNetData << output << endl;
            }
        }
    }
    SaveNetData.clear(); //为了代码具有移植性和复用性, 这句最好带上,清除标志位.有些系统若不清理可能会出现问题.
	SaveNetData.close();
}
/********************************************************************************
*******识别图像准确率
********************************************************************************/
float NeuralNet::RecognitionAccuracy(vector<float_cell >& SetIn, vector<float_cell >& SetOut, int Total , bool Debug) {
    vector<float> action;     //动作
    vector<float> outputs;     //输出
    float accuracy;
    int accuracy_cout = 0;
    //std::cout << "Recongnition..." << std::endl;
    //std::cout << "SetIn.size= " << SetIn.size() << std::endl;
    //std::cout << "SetIn[0].size= " << SetIn[0].size() << std::endl;
    //std::cout << "SetIn[0][0].size= " << SetIn[0][0].size() << std::endl;
    for (int i = 0; i < SetIn.size(); ++i) {
        for (int j = 0; j < SetIn[0].size(); ++j) {
            outputs = Update(SetIn[i][j]);
            action = ActualArray(outputs);
            std::cout << "action= ";
            for(int cnt = 0; cnt < action.size(); ++cnt) {
                std::cout << action[cnt] << " ";
            }
            std::cout << std::endl;
            std::cout << "SetOut= "; 
            for(int cnt = 0; cnt < SetOut[i][j].size(); ++cnt) {
                std::cout << SetOut[i][j][cnt] << " ";
            }
            std::cout << std::endl;
            if (action == SetOut[i][j]) {
                accuracy_cout++;
            }
        }
    }
    std::cout << "accuracy_cout= " << accuracy_cout << std::endl;
    accuracy = (accuracy_cout + 0.0) / Total;
    return accuracy;
}
/********************************************************************************
*******小数转换成整数，便于与期望比对
********************************************************************************/
vector<float> NeuralNet::ActualArray(vector<float>& input) {
    vector<float> output;
    float temp = input[0];
    int label = 0;
    for (int i = 1; i < input.size(); ++i) {
        if(temp < input[i]) {
            temp = input[i];
            label = i;
        }
    }
    for (int j = 0; j < input.size(); ++j) {
        if(label == j) {
            output.push_back(1);
        }
        else {
            output.push_back(0);
        }
    }
    return output;
}
/********************************************************************************
*******识别图像
********************************************************************************/
int NeuralNet::Recongnition(vector<float>& inputs, vector<float>& output, bool Debug) {
    vector<float> action;     //输出
    //std::cout << "Recongnition..." << std::endl;
    //std::cout << inputs.size() << std::endl;
    output = Update(inputs);
    //std::cout << output.size() << std::endl;
    action = ActualArray(output);
    for (int i = 0; i < action.size(); ++i) {
        if (action[i] == 1) {
            return i + 1;
        }
    }
    return 0;
}
/********************************************************************************
*******训练网络析构
********************************************************************************/
NeuralNet::~NeuralNet()
{
	
}
