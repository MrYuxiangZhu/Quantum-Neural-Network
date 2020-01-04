/*=================================================================
 * Extract Background & Foreground Model by ViBe Algorithm using OpenCV Library.
 *
 * Copyright (C) 2017 Chandler Geng. All rights reserved.
 *
 *     This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *     You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA
===================================================================
*/

#include "../QuantumViBe/QuantumViBe.hpp"

int main(int argc, char* argv[])
{
    VideoCapture capture;
    capture = VideoCapture("./Video/Camera Road 01.avi");
    if(!capture.isOpened())
    {
        capture = VideoCapture("../Video/Camera Road 01.avi");
        if(!capture.isOpened())
        {
            capture = VideoCapture("../../Video/Camera Road 01.avi");
            if(!capture.isOpened())
            {
                cout<<"ERROR: Did't find this video!"<<endl;
                return 0;
            }
        }
    }

    //capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    //capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    //Camera vedio(1);               //设置摄像头对象
    //VideoCapture capture = vedio.Get_Capture();
    //cvNamedWindow("SrcImage",1);
    /*if (!capture.isOpened())
    {
        cout<<"No camera or video input!"<<endl;
        return -1;
    }*/

    // 程序运行时间统计变量
    // the Time Statistical Variable of Program Running Time
    double time;
    double start;
    cv::Mat First, frame, gray, FGModel;
    QViBe qvibe;
    NEQR QuantumImage;
    Quantum_Measure measure;
    bool count = true;
    int frameToStart = 0;
    capture.set(CAP_PROP_POS_FRAMES, frameToStart);//从此时的帧数开始获取帧
    if (!capture.read(First))
    {
        std::cout << "Read vedio failed!" << std::endl;
    }
    cv::Mat FirstResizeImage = QuantumImage.Resize_Classic_Image(First, 256, 256, CV_RGB2GRAY, false);
    QuantumImage.Image_Measure(FirstResizeImage);
    QuantumImage.Quantum_Image_Init(FirstResizeImage, false);
    measure.Init(QuantumImage);
    qvibe.Init(QuantumImage);

    //创建 grad_x 和 grad_y 矩阵  
    Mat grad_x, grad_y;  
    Mat abs_grad_x, abs_grad_y,dst;  

    while (1)
    {
        capture >> frame;
        if (frame.empty())
            continue;
       /* frame = vedio.GetFrameImage();
        if(frame.empty())
            continue;
        cvtColor(frame, gray, CV_RGB2GRAY);*/

        if (count)
        {
            start = static_cast<double>(getTickCount());
            cv::Mat ResizeImage = QuantumImage.Resize_Classic_Image(frame, 256, 256, CV_RGB2GRAY, false);
            imshow("ResizeImage",ResizeImage);//显示原图像
            QuantumImage.Constructor(ResizeImage, false);
            qvibe.ProcessFirstFrame(QuantumImage);
            time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
            std::cout << "Time of Update ViBe Background = " << time << " ms" << std::endl;
            cout<<"Training ViBe Success."<<endl;
            count = false;
            //vedio.DrawText(frame);
            //imshow("SrcImage",frame);//显示原图像
        }
        else
        {
            start = static_cast<double>(getTickCount());
            cv::Mat ResizeImage = QuantumImage.Resize_Classic_Image(frame, 256, 256, CV_RGB2GRAY, false);
            imshow("ResizeImage",ResizeImage);//显示原图像
            QuantumImage.Constructor(ResizeImage, false);
            qvibe.Run(QuantumImage, false);
            FGModel = qvibe.Get_FGModel();
            imshow("FGModel", FGModel);
            GaussianBlur(FGModel, FGModel, Size(7,7), 1.5, 1.5);
		    Canny(FGModel, FGModel, 0, 30, 3);
            //求 X方向梯度  
            //Sobel(FGModel, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);  
            //convertScaleAbs( grad_x, abs_grad_x );  
  
            //求Y方向梯度  
            //Sobel(FGModel, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);  
            //convertScaleAbs( grad_y, abs_grad_y );  
  
            //合并梯度(近似)  
            //addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);  
			//morphologyEx(FGModel, FGModel, MORPH_OPEN, Mat());
            time = ((double)getTickCount() - start) / getTickFrequency() * 1000;
            std::cout << "Time of Update ViBe Background = " << time << " ms" << std::endl;
            //vedio.DrawText(frame);
            imshow("edge", FGModel);
        }

        //imshow("input", frame);
        if(waitKey(10) >= 0)
            break;
    }

    return 0;
}
