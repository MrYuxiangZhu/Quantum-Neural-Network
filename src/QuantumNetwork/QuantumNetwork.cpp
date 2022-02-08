/********************************************************************************
****文件名：量子BP神经网络
****作  者：朱宇祥
****版  本：1.0
****日  期：2019/3/3
*********************************************************************************/

#include "QuantumNetwork.hpp"

/******************************************************************************
********数据输入构造
*******************************************************************************/
QuantumData::QuantumData(int VideoType, int VideoNum, int Frames, int datas):VideoTypeVectorSize(VideoType), VideoNumVectorSize(VideoNum), FrameNums(Frames), EachNums(datas)
{

}
/******************************************************************************
********创建数据空间
*******************************************************************************/
void QuantumData::Create_Data(vector<float_cell>& input, vector<float_cell>& output) 
{
    vector<float> output_neuron(VideoTypeVectorSize, 0);//一维输出神经元数组
    vector<iovector> action_frames(VideoNumVectorSize * FrameNums, output_neuron);//二维数组，动作帧数
    vector<float_cell> Output_data(VideoTypeVectorSize, action_frames);//三维，动作种类
    for (int i = 0; i < VideoTypeVectorSize; ++i) 
    {
        for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) 
        {
            for (int k = 0; k < VideoTypeVectorSize; ++k) 
            {
                Output_data[i][j][k] = (i == k ? 1 : 0);
            }
        }
    }
    output = Output_data;
}

/********************************************************************************
*******转换成训练数据，乘以PI / 2
********************************************************************************/
/*vector<float> QuantumData::ChangeToInput(vector<float>& input) 
{
    vector<float> output;
    for (int i = 0; i < input.size(); ++i) 
    {
        if(input[i] > 0) 
        {
            output.emplace_back(input[i] + 0.5);
        }
        else 
        {
            output.emplace_back(input[i] + 0.0);
        }
    }
    return output;
}*/

/********************************************************************************
*******转换成训练数据，乘以PI / 2
********************************************************************************/
vector<float> QuantumData::ChangeToInput(vector<vector<float> >& input) 
{
    vector<float> output;
    //std::cout << "input=" << input.size() << std::endl;
    //std::cout << "input[0]=" << input[0].size() << std::endl;
    for (int i = 0; i < input.size(); ++i) {

        for (int j = 0; j < input[0].size(); ++j) 
        {
            //output.push_back(2 * PI / (1 + exp((-1) * input[i][j])));
            //output.push_back(input[i][j]);
            if (input[i][j] > 0.0) 
            {
                output.emplace_back(atan2(input[i][j] + 0.3, 1));
            }
            else 
            {
                output.emplace_back(atan2(input[i][j] + 0.0, 1));
            }
        }
    }
    return output;
}

/********************************************************************************
*******数据写入硬盘
********************************************************************************/
void QuantumData::Write_Data(string filename) 
{
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
    for (int i = 0; i < VideoTypeVectorSize; ++i) 
    {
        for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j)
        {
            for (int k = 0; k < EachNums; ++k) 
            {
                output = to_string(this->SetIn[i][j][k]);
                WriteData << output << endl;
            }
        }
    }
    for (int i = 0; i < VideoTypeVectorSize; ++i) 
    {
        for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) 
        {
            for (int k = 0; k < VideoTypeVectorSize; ++k) 
            {
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
void QuantumData::Read_Data(string filename) 
{
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
        for (int i = 0; i < VideoTypeVectorSize; ++i) 
        {
            for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) 
            {
                for (int k = 0; k < EachNums; ++k) 
                {
                    ReadData.getline(buffer, 100);
                    this->SetIn[i][j][k] = stringToNum<float>(buffer);
                }
            }
        }
        for (int i = 0; i < VideoTypeVectorSize; ++i) 
        {
            for (int j = 0; j < VideoNumVectorSize * FrameNums; ++j) 
            {
                for (int k = 0; k < VideoTypeVectorSize; ++k) 
                {
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
QuantumData::~QuantumData()
{
	
}

/******************************************************************************
********转移函数
*******************************************************************************/
float QuantumNeuralNet::Sigmoid(float activation, float response)
{
    return ( 1 / ( 1 + exp(-activation / response)));
}

/******************************************************************************
********转移函数
*******************************************************************************/
float QuantumNeuralNet::atanderivate(float activation) 
{
    return (1 / (1 + activation * activation));
}

/******************************************************************************
********构造函数
*******************************************************************************/
QuantumNeuralNet::QuantumNeuralNet() 
{

}

/******************************************************************************
********初始化网络
*******************************************************************************/
QuantumNeuralNet::QuantumNeuralNet(int inputlayer, int outputlayer, int hiddenneurons, int hiddenLayersnum, float learningRate, float errorthresh, long int trainepochs, STOPTYPE type, bool debug, float errorsum, bool trained, int numepochs):
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
    NumEpochs(numepochs)
{
    CreateNet();
}

/********************************************************************************
*******读取网络权值
********************************************************************************/
QuantumNeuralNet::QuantumNeuralNet(string filename) 
{
    char buffer[100];
    ifstream NeuralNetData(filename);
    if (NeuralNetData.is_open())
	{
        NeuralNetData.getline(buffer, 100);
        this->NumLayers = stringToNum<int>(buffer);
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
        for(int i = 0; i < this->VecLayers.size(); ++i) 
        {
            NeuralNetData.getline(buffer, 100);
            this->VecLayers[i].NumNeurons = stringToNum<int>(buffer);
            for(int j = 0; j < this->VecLayers[i].VecNeurons.size(); ++j)
            {
                NeuralNetData.getline(buffer, 100);
                this->VecLayers[i].VecNeurons[j].NumInputs = stringToNum<int>(buffer);
                NeuralNetData.getline(buffer, 100);
                this->VecLayers[i].VecNeurons[j].ParaSigmoid = stringToNum<float>(buffer);
                for(int k = 0; k < this->VecLayers[i].VecNeurons[j].VecWeight.size(); ++k) 
                {
                    NeuralNetData.getline(buffer, 100);
                    this->VecLayers[i].VecNeurons[j].VecWeight[k] = stringToNum<float>(buffer);
                }
            }
        }
    }
    NeuralNetData.clear(); //为了代码具有移植性和复用性, 这句最好带上,清除标志位.有些系统若不清理可能会出现问题.
	NeuralNetData.close();
}

/********************************************************************************
*******创建网络
********************************************************************************/
void QuantumNeuralNet::CreateNet()
{
    if(NumHiddenLayers > 0) 
    {
        VecLayers.emplace_back(QuantumNeuronLayer(NeuronsPerHiddenLayer, NumInputs, 1));		//输入层
		for(int i = 0; i < NumHiddenLayers - 1; ++i)
        {
			VecLayers.emplace_back(QuantumNeuronLayer(NeuronsPerHiddenLayer, NeuronsPerHiddenLayer, 2));//隐层
		} 
        VecLayers.emplace_back(QuantumNeuronLayer(NumOutputs, NeuronsPerHiddenLayer, 3));		//输出层
    }
    else 
    {
        VecLayers.emplace_back(QuantumNeuronLayer(NumOutputs, NumInputs, 1));
    }
    NumLayers = NumHiddenLayers + 1;                                    //总层数 = 隐层数  + 输出层数，这里不包括输入层
}

/********************************************************************************
*******初始化神经网络权值
********************************************************************************/
void QuantumNeuralNet::InitializeNetwork()
{
    for (int i = 0; i < NumLayers; ++i) 
    {								    //权值维数比隐层数多1
		for (int n = 0; n < VecLayers[i].NumNeurons; ++n) 
        {
			for (int k = 0; k< VecLayers[i].VecNeurons[n].NumInputs; ++k) 
            {
				VecLayers[i].VecNeurons[n].VecWeight[k] = RandomClamped(i + 1);//随机生成权重
                VecLayers[i].VecNeurons[n].VecTij[k] = 0;                       //Tij参数初始化为0
			}
            VecLayers[i].VecNeurons[n].ParaSigmoid = RandomClamped(2);          //随机生成Sigmoid值
            VecLayers[i].ErrorLayer[n] = 0;     
		}            
	} 
    //ErrorSum=9999;
    //NumEpochs=0;
}

/********************************************************************************
*******实际输出
********************************************************************************/
vector<float> QuantumNeuralNet::Update(vector<float> inputs)
{
    vector<float> outputs;     //输出
    float arctanS = 0;         //反正切
    float cos_temp = 0;
    float sin_temp = 0;
    float Sj_temp = 0;
    float Hj_temp = 0;
    if (inputs.size() != NumInputs)
    {
        return outputs;
    }
    outputs.clear();
    //计算第一层隐层输出
    //std::cout << "VecLayers[0].NumNeurons= " << VecLayers[0].NumNeurons << std::endl;
    //std::cout << "VecLayers[1].NumNeurons= " << VecLayers[1].NumNeurons << std::endl;
    //std::cout << "NumInputs= " << NumInputs << std::endl;
    for(int j = 0; j < VecLayers[0].NumNeurons; ++j) 
    {
        float sumsin = 0;          //正弦之和
        float sumcos = 0;          //余弦之和
        for (int i = 0; i < NumInputs; ++i) 
        {
            sumsin += sin(VecLayers[0].VecNeurons[j].VecWeight[i] + inputs[i]);
            sumcos += cos(VecLayers[0].VecNeurons[j].VecWeight[i] + inputs[i]);
        }
        VecLayers[0].VecNeurons[j].ParaAj = sumsin;
        VecLayers[0].VecNeurons[j].ParaBj = sumcos;
        VecLayers[0].VecNeurons[j].Arctg = (float)atan2(sumsin, sumcos);
        for (int i = 0; i < NumInputs; ++i) 
        {
            cos_temp = cos(VecLayers[0].VecNeurons[j].VecWeight[i] + inputs[i]) * VecLayers[0].VecNeurons[j].ParaBj;
            sin_temp = sin(VecLayers[0].VecNeurons[j].VecWeight[i] + inputs[i]) * sin(VecLayers[0].VecNeurons[j].VecWeight[i] + inputs[i]);
            Sj_temp = VecLayers[0].VecNeurons[j].ParaAj * VecLayers[0].VecNeurons[j].ParaAj + VecLayers[0].VecNeurons[j].ParaBj * VecLayers[0].VecNeurons[j].ParaBj;
            VecLayers[0].VecNeurons[j].VecTij[i] = (cos_temp + sin_temp) / Sj_temp;
        }
        Hj_temp = sin(Sigmoid(VecLayers[0].VecNeurons[j].ParaSigmoid, ACTIVATION_RESPONSE) * PI / 2 - VecLayers[0].VecNeurons[j].Arctg);
        VecLayers[0].VecNeurons[j].Activation = Hj_temp;
        outputs.emplace_back(Hj_temp);//隐层输出
    }
    //计算隐层到输出层
    inputs = outputs;
    outputs.clear();
    for(int m = 0; m < VecLayers[1].NumNeurons; ++m) 
    {
        float netinput = 0;
        int numInputs = VecLayers[1].VecNeurons[m].NumInputs;
        for (int n = 0; n < numInputs; ++n) 
        {
            netinput += VecLayers[1].VecNeurons[m].VecWeight[n] * inputs[n];   
        }
        VecLayers[1].VecNeurons[m].Activation = netinput;
        outputs.emplace_back(atan2(VecLayers[1].VecNeurons[m].Activation, 1));//即输出
    }
    /*std::cout << "outputs= " << std::endl;
    for (int i = 0; i < outputs.size(); ++i) 
    {
         std::cout << outputs[i] << " ";
    }
    std::cout << std::endl;*/
    return outputs;
}

/********************************************************************************
*******训练网络子函数，三层神经网络
********************************************************************************/
bool QuantumNeuralNet::NetworkTrainingEpoch(vector<iovector >& SetIn, vector<iovector >& SetOut) 
{
    vector<float>::iterator curWeight;//指向某个权重
    vector<float>::iterator curError;//指向某个误差
    vector<QuantumNeuron>::iterator curNrnOut, curNrnHid, curSigmoid, curAngle;//指向输出神经元、某个隐藏神经元和指向某个Sigmoid
    ErrorSum = 0;//置零
    //对每一个输入集合调整权值
    for(unsigned int vec = 0; vec < SetIn.size(); ++vec) 
    {
        vector<float> outputs = Update(SetIn[vec]);//通过神经网络获得输出
        //计算误差E
        for(int hidden = 0; hidden < VecLayers[0].NumNeurons; ++hidden) 
        {
            VecLayers[0].ErrorLayer[hidden] = 0;
            for(int out = 0; out < NumOutputs; ++out) 
            {
                VecLayers[0].ErrorLayer[hidden] +=  VecLayers[1].VecNeurons[out].VecWeight[hidden] * (SetOut[vec][out] - outputs[out]) * \
                                                atanderivate(VecLayers[1].VecNeurons[out].Activation);
            }
        }
        //隐层到输出层
        for(int op = 0; op < NumOutputs; ++op) 
        {
            float err = SetOut[vec][op] - outputs[op];//误差
            //std::cout << "outputs[op] " << outputs[op] << std::endl;
            ErrorSum += (SetOut[vec][op] - outputs[op]) * (SetOut[vec][op] - outputs[op]);    //误差的平方之和
            curWeight = VecLayers[1].VecNeurons[op].VecWeight.begin();//标记第一个权重
            curNrnHid = VecLayers[0].VecNeurons.begin();//标记隐藏层第一个神经元
            curNrnOut = VecLayers[1].VecNeurons.begin();//标记输出层第一个神经元
            //unsigned long const length = std::distance(curNrnHid, VecLayers[0].VecNeurons.end());//计算序列的长度
            //对该神经元的每一个权重进行调整
            while(curWeight != VecLayers[1].VecNeurons[op].VecWeight.end()) 
            {
                *curWeight += err * LearningRate * atanderivate(curNrnOut->Activation) * sin(Sigmoid(curNrnHid->ParaSigmoid, ACTIVATION_RESPONSE) * \
                PI / 2 - curNrnHid->Arctg);//根据误差和学习率和阈值调整权重
                //std::cout << "*curWeight " << *curWeight << std::endl;
                curWeight++;//指向下一个权重
                curNrnHid++;//指向下一个隐藏层神经元
            }
            curNrnOut++;
        }
        //std::cout << "ErrorSum= " << ErrorSum << std::endl;
        //更新第一隐层角度参数
        curAngle = VecLayers[0].VecNeurons.begin();//标记隐藏层第一个神经元
        curError = VecLayers[0].ErrorLayer.begin(); //标记隐藏层第一个误差
        while(curAngle != VecLayers[0].VecNeurons.end()) 
        {
            int inputnum = curAngle->NumInputs;
            for(int w = 0; w < inputnum; ++w) 
            {
                curAngle->VecWeight[w] -= (*curError * LearningRate * cos(Sigmoid(curAngle->ParaSigmoid, ACTIVATION_RESPONSE) * PI / 2 - curAngle->Arctg) * curAngle->VecTij[w]);
            }      
            curAngle++;
            curError++;
        }
        //更新角度和Sigmoid参数
        curSigmoid = VecLayers[0].VecNeurons.begin();//标记隐藏层第一个神经元
        curError = VecLayers[0].ErrorLayer.begin(); //标记隐藏层第一个误差
        //对该神经元的每一个Sigmoid进行调整
        while(curSigmoid != VecLayers[0].VecNeurons.end()) 
        { 
            curSigmoid->ParaSigmoid += (*curError * LearningRate * cos(Sigmoid(curSigmoid->ParaSigmoid, ACTIVATION_RESPONSE) * PI / 2 \
            - curSigmoid->Arctg) * Sigmoid(curSigmoid->ParaSigmoid, ACTIVATION_RESPONSE) * (1 - Sigmoid(curSigmoid->ParaSigmoid, ACTIVATION_RESPONSE))) * PI / 2;
            //std::cout << "curSigmoid->ParaSigmoid= " << curSigmoid->ParaSigmoid << std::endl;
            curSigmoid++;//指向下一个隐藏层神经元
            curError++;//指向下一个误差
        }
    }
    return 1;
}

/********************************************************************************
*******训练网络
********************************************************************************/
bool QuantumNeuralNet::Train(QuantumData data, bool Debug)
{
    std::cout << "Training..." << std::endl;
    if (Trained == true) 
    {
        return false;		
	}
    vector<vector<vector<float>>> SetIn = data.GetInputSet();
    vector<vector<vector<float>>> SetOut = data.GetOutputSet();
    InitializeNetwork();
	long int TrainedCount = 0;
    //训练值差作为停止标志
    /*while (TrainedCount < TrainCount) 
    {
        for (int i = 0; i < SetIn.size(); ++i) 
        {
            for (int j = 0; j < 20; ++j) 
            {
                std::cout << "#";
            }
            NetworkTrainingEpoch(SetIn[i], SetOut[i]);
            std::cout << std::endl;
            std::cout << "ErrorSum= " << ErrorSum << std::endl;
        }
        TrainedCount++;
        std::cout << "TrainedCount= " << TrainedCount << std::endl;
    }*/
    if (StopType == COUNT) 
    {
        long int i = TrainCount;
        while(i--)
        {
            if(Debug)
                cout<<"ErrorSum:"<<ErrorSum<<endl;
            //NetworkTrainingEpoch(SetIn,SetOut);
            for (int i = 0; i < SetIn.size(); ++i) 
            {
                NetworkTrainingEpoch(SetIn[i], SetOut[i]);
            }
        }
    }
	//最大误差值作为停止标志
    else 
    {
        while (ErrorSum > ERROR_THRESHOLD) 
        {
            if(Debug)
                cout<<"ErrorSum:"<<ErrorSum<<endl;
            for (int i = 0; i < SetIn.size(); ++i) 
            {
                NetworkTrainingEpoch(SetIn[i], SetOut[i]);
            }
        }
    }
    std::cout << "ErrorSum= " << ErrorSum << std::endl;
    Trained = true;
    std::cout << "Done!!!" << std::endl;
    return true;
}

/********************************************************************************
*******识别图像
********************************************************************************/
int QuantumNeuralNet::Recongnition(vector<float>& inputs, vector<float>& output, bool Debug) 
{
    vector<float> action;     //输出
    //std::cout << "Recongnition..." << std::endl;
    //std::cout << inputs.size() << std::endl;
    output = Update(inputs);
    //std::cout << output.size() << std::endl;
    action = ActualArray(output);
    for (int i = 0; i < action.size(); ++i) 
    {
        if (action[i] == 1) 
        {
            return i + 1;
        }
    }
    return 0;
}

/********************************************************************************
*******识别图像准确率
********************************************************************************/
float QuantumNeuralNet::RecognitionAccuracy(vector<float_cell >& SetIn, vector<float_cell >& SetOut, int Total , bool Debug) 
{
    vector<float> action;     //动作
    vector<float> outputs;     //输出
    float accuracy;
    int accuracy_cout = 0;
    std::cout << "Recongnition..." << std::endl;
    //std::cout << "SetIn.size= " << SetIn.size() << std::endl;
    //std::cout << "SetIn[0].size= " << SetIn[0].size() << std::endl;
    for (int i = 0; i < SetIn.size(); ++i) 
    {
        for (int j = 0; j < SetIn[0].size(); ++j) 
        {
            outputs = Update(SetIn[i][j]);
            action = ActualArray(outputs);
            for (int i = 0; i < action.size(); ++i) 
            {
                if (action[i] == 1) 
                {
                    std::cout << "action[" << i << "]=" << action[i] << std::endl;
                }
            }
            if (action == SetOut[i][j]) 
            {
                accuracy_cout++;
            }
        }
    }
    accuracy = accuracy_cout / Total;
    return accuracy;
}
/********************************************************************************
*******保存网络权值
********************************************************************************/
void QuantumNeuralNet::SaveNet(string filename) 
{
    ofstream SaveNetData(filename);
    string output;
    output = to_string(this->NumLayers);
    SaveNetData << output << endl;
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
    for(int i = 0; i < this->VecLayers.size(); ++i) 
    {
        output = to_string(this->VecLayers[i].NumNeurons);
        SaveNetData << output << endl;
        for(int j = 0; j < this->VecLayers[i].VecNeurons.size(); ++j) 
        {
            output = to_string(this->VecLayers[i].VecNeurons[j].NumInputs);
            SaveNetData << output << endl;
            output = to_string(this->VecLayers[i].VecNeurons[j].ParaSigmoid);
            SaveNetData << output << endl;
            for(int k = 0; k < this->VecLayers[i].VecNeurons[j].VecWeight.size(); ++k) 
            {
                output = to_string(this->VecLayers[i].VecNeurons[j].VecWeight[k]);
                SaveNetData << output << endl;
            }      
        }
    }
    SaveNetData.clear(); //为了代码具有移植性和复用性, 这句最好带上,清除标志位.有些系统若不清理可能会出现问题.
	SaveNetData.close();
}

/********************************************************************************
*******小数转换成整数，便于与期望比对
********************************************************************************/
vector<float> QuantumNeuralNet::ActualArray(vector<float>& input) 
{
    vector<float> output;
    float temp = input[0];
    int label = 0;
    for (int i = 1; i < input.size(); ++i) 
    {
        if (temp < input[i]) 
        {
            temp = input[i];
            label = i;
        }
    }
    for (int j = 0; j < input.size(); ++j) 
    {
        if(label == j) 
        {
            output.emplace_back(1);
        }
        else 
        {
            output.emplace_back(0);
        }
    }
    return output;
}

/********************************************************************************
*******训练网络析构
********************************************************************************/
QuantumNeuralNet::~QuantumNeuralNet()
{
	Delete();
}

/********************************************************************************
*******删除容器
********************************************************************************/
void QuantumNeuralNet::Delete()
{
    vector<QuantumNeuronLayer>().swap(VecLayers);
}
