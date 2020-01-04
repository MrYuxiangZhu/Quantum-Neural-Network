CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(QUANTUM_HUMAN_ACTION_RECOGNITION_PROGECT)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread" )
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

FIND_PACKAGE(OpenCV REQUIRED)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(./src)

##################################################################
#####生成链接库
##################################################################
# Camera，测试摄像头
SET(LIB_CAMERA_SOURCE
	./src/Camera/camera.cpp
	./src/Camera/camera.hpp)
ADD_LIBRARY(CAMERA SHARED 
			${LIB_CAMERA_SOURCE})
TARGET_LINK_LIBRARIES(CAMERA 
			${OpenCV_LIBS})

# NEQR，构建量子图像
SET(LIB_QUANTUM_IMAGE_SOURCE
	./src/QuantumImagePreparation/QuantumImagePreparation.cpp
	./src/QuantumImagePreparation/QuantumImagePreparation.hpp)
ADD_LIBRARY(QUANTUM_IMAGE SHARED 
			${LIB_QUANTUM_IMAGE_SOURCE} 
			${LIB_CAMERA_SOURCE}
			)
TARGET_LINK_LIBRARIES(QUANTUM_IMAGE 
			${OpenCV_LIBS}
			)

# 量子电路
SET(LIB_QUANTUM_CIRCUIT_SOURCE
	./src/QuantumCircuit/QuantumCircuit.cpp
	./src/QuantumCircuit/QuantumCircuit.hpp)
ADD_LIBRARY(QUANTUM_CIRCUIT SHARED 
			${LIB_QUANTUM_CIRCUIT_SOURCE} 
			${LIB_QUANTUM_IMAGE_SOURCE}
			)
TARGET_LINK_LIBRARIES(QUANTUM_CIRCUIT 
			${OpenCV_LIBS}
			)
			
# 量子ViBe动态链接库生成
SET(LIB_QUANTUM_VIBE_SOURCE
	./src/QuantumViBe/QuantumViBe.cpp
	./src/QuantumViBe/QuantumViBe.hpp)
ADD_LIBRARY(QUANTUM_VIBE SHARED 
			${LIB_QUANTUM_VIBE_SOURCE} 
			${LIB_QUANTUM_CIRCUIT_SOURCE} 
			${LIB_QUANTUM_IMAGE_SOURCE} 
			${LIB_CAMERA_SOURCE}
			)
TARGET_LINK_LIBRARIES(QUANTUM_VIBE 
			${OpenCV_LIBS}
			)
		
# 量子BGDifference，背景差分法动态链接库生成
SET(LIB_QBGDIFF_SOURCE
	./src/QuantumBGDifference/QuantumBGDifference.hpp
	./src/QuantumBGDifference/QuantumBGDifference.cpp)
ADD_LIBRARY(QBGDiff SHARED 
			${LIB_QBGDIFF_SOURCE}
			)
TARGET_LINK_LIBRARIES(QBGDiff
			${OpenCV_LIBS}
			)		
			
# 量子边缘检测
SET(LIB_QUANTUM_EDGE_EXTRACT_SOURCE
	./src/QuantumImageEdgeExtract/QuantumImageEdgeExtract.cpp
	./src/QuantumImageEdgeExtract/QuantumImageEdgeExtract.hpp)
ADD_LIBRARY(QUANTUM_EDGE_EXTRACT SHARED 
			${LIB_QUANTUM_EDGE_EXTRACT_SOURCE}
			${LIB_QUANTUM_VIBE_SOURCE} 			
			${LIB_QUANTUM_IMAGE_SOURCE}
			${LIB_CAMERA_SOURCE}
			)
TARGET_LINK_LIBRARIES(QUANTUM_EDGE_EXTRACT 
			${OpenCV_LIBS}
			)
			
# 量子图像特征提取
SET(LIB_QUANTUM_FEATURE_EXTRACT_SOURCE
	./src/QuantumImageFeatureExtract/QuantumImageFeatureExtract.cpp
	./src/QuantumImageFeatureExtract/QuantumImageFeatureExtract.hpp)
ADD_LIBRARY(QUANTUM_FEATURE_EXTRACT SHARED 
			${LIB_QUANTUM_FEATURE_EXTRACT_SOURCE}
			${LIB_QUANTUM_EDGE_EXTRACT_SOURCE}
			${LIB_QUANTUM_VIBE_SOURCE} 			
			${LIB_QUANTUM_IMAGE_SOURCE}
			${LIB_CAMERA_SOURCE}
			)
TARGET_LINK_LIBRARIES(QUANTUM_FEATURE_EXTRACT 
			${OpenCV_LIBS}
			)
# 卷积特征提取
SET(LIB_CONVOLUTION_EXTRACT_SOURCE
	./src/QuantumConvolution/QuantumConvolution.cpp
	./src/QuantumConvolution/QuantumConvolution.hpp)
ADD_LIBRARY(CONVOLUTION_EXTRACT SHARED
			${LIB_QUANTUM_FEATURE_EXTRACT_SOURCE} 
			${LIB_CONVOLUTION_EXTRACT_SOURCE}
			${LIB_QUANTUM_EDGE_EXTRACT_SOURCE}
			${LIB_QUANTUM_VIBE_SOURCE} 			
			${LIB_QUANTUM_IMAGE_SOURCE}
			${LIB_CAMERA_SOURCE}
			)
TARGET_LINK_LIBRARIES(CONVOLUTION_EXTRACT 
			${OpenCV_LIBS}
			)

# 量子神经网络
SET(LIB_QUANTUM_NETWORK_SOURCE
	./src/QuantumNetwork/QuantumNetwork.cpp
	./src/QuantumNetwork/QuantumNetwork.hpp)
ADD_LIBRARY(QUANTUM_NETWORK SHARED 
			${LIB_QUANTUM_FEATURE_EXTRACT_SOURCE}
			${LIB_CONVOLUTION_EXTRACT_SOURCE}
			${LIB_QUANTUM_EDGE_EXTRACT_SOURCE}
			${LIB_QUANTUM_VIBE_SOURCE} 			
			${LIB_QUANTUM_IMAGE_SOURCE}
			${LIB_CAMERA_SOURCE}
			${LIB_QUANTUM_NETWORK_SOURCE}
			)
TARGET_LINK_LIBRARIES(QUANTUM_NETWORK 
			${OpenCV_LIBS}
			)

# BP神经网络
SET(LIB_BP_NETWORK_SOURCE
	./src/BpNetwork/NeuralNet.cpp
	./src/BpNetwork/NeuralNet.hpp)
ADD_LIBRARY(BP_NETWORK SHARED 
			${LIB_CONVOLUTION_EXTRACT_SOURCE}
			${LIB_QUANTUM_FEATURE_EXTRACT_SOURCE}
			${LIB_QUANTUM_EDGE_EXTRACT_SOURCE}
			${LIB_QUANTUM_VIBE_SOURCE} 			
			${LIB_QUANTUM_IMAGE_SOURCE}
			${LIB_CAMERA_SOURCE}
			${LIB_BP_NETWORK_SOURCE}
			${LIB_QUANTUM_NETWORK_SOURCE}
			)
TARGET_LINK_LIBRARIES(BP_NETWORK 
			${OpenCV_LIBS}
			)
	
##################################################################
#####生成运行程序
##################################################################

# 生成Camera测试程序
SET(LIB_Camera CAMERA)
ADD_EXECUTABLE(Camera_test ./src/example/camera_test.cpp)
TARGET_LINK_LIBRARIES(Camera_test 
			${LIB_Camera}
			)

# 生成NEQR测试程序
SET(LIB_QUANTUM_IMAGE QUANTUM_IMAGE)
ADD_EXECUTABLE(NEQR_test ./src/example/NEQR_test.cpp)
TARGET_LINK_LIBRARIES(NEQR_test 
			${LIB_QUANTUM_IMAGE}
			)

# 生成NEQR存储程序
ADD_EXECUTABLE(NEQR_save ./src/example/NEQR_Save.cpp)
TARGET_LINK_LIBRARIES(NEQR_save 
			${LIB_QUANTUM_IMAGE}
			)

# 生成FRQI测试程序
SET(LIB_QUANTUM_IMAGE QUANTUM_IMAGE)
ADD_EXECUTABLE(FRQI_test ./src/example/FRQI_test.cpp)
TARGET_LINK_LIBRARIES(FRQI_test 
			${LIB_QUANTUM_IMAGE}
			)

# 生成量子电路测试程序
SET(LIB_QUANTUM_CIRCUIT QUANTUM_CIRCUIT)
ADD_EXECUTABLE(Quantum_Circuit_test ./src/example/QuantumCircuit_test.cpp)
TARGET_LINK_LIBRARIES(Quantum_Circuit_test 
			${LIB_QUANTUM_CIRCUIT}
			)

# 生成ViBe测试程序
SET(LIB_QUANTUM_VIBE QUANTUM_VIBE)
ADD_EXECUTABLE(Quantum_Vibe_test ./src/example/QuantumViBe_test.cpp)
TARGET_LINK_LIBRARIES(Quantum_Vibe_test 
			${LIB_QUANTUM_VIBE}
			)

# 生成BGDifference测试程序
SET(LIB_QBGDIFF QBGDiff)
ADD_EXECUTABLE(QBGDiff_test ./src/example/QBGDifference_test.cpp)
TARGET_LINK_LIBRARIES(QBGDiff_test
			${LIB_QBGDIFF}
			)
			
# 生成量子图像边缘检测测试程序
SET(LIB_QUANTUM_EDGE_EXTRACT QUANTUM_EDGE_EXTRACT)
ADD_EXECUTABLE(EdgeExtract_test ./src/example/QEdgeExtract_test.cpp)
TARGET_LINK_LIBRARIES(EdgeExtract_test 
			${LIB_QUANTUM_EDGE_EXTRACT}
			)

# 生成量子图像特征提取测试程序
SET(LIB_QUANTUM_FEATURE_EXTRACT QUANTUM_FEATURE_EXTRACT)
ADD_EXECUTABLE(FeatureExtract_test ./src/example/QFeatureExtract_test.cpp)
TARGET_LINK_LIBRARIES(FeatureExtract_test 
			${LIB_QUANTUM_FEATURE_EXTRACT}
			)
# 生成卷积测试程序
SET(LIB_CONVOLUTION CONVOLUTION_EXTRACT)
ADD_EXECUTABLE(convolution_test ./src/example/Convolution_test.cpp)
TARGET_LINK_LIBRARIES(convolution_test 
			${LIB_CONVOLUTION}
			)
# 生成视频数据存储程序
SET(LIB_QUANTUM_BP_NETWORK QUANTUM_NETWORK)
ADD_EXECUTABLE(videodata_Save ./src/example/VideoData_Save.cpp)
TARGET_LINK_LIBRARIES(videodata_Save 
			${LIB_QUANTUM_BP_NETWORK}
			)		
# 生成量子BP训练程序
ADD_EXECUTABLE(action_train ./src/example/ActionTrain.cpp)
TARGET_LINK_LIBRARIES(action_train 
			${LIB_QUANTUM_BP_NETWORK}
			)
# 生成量子BP识别程序
ADD_EXECUTABLE(action_recognition ./src/example/ActionRecognition.cpp)
TARGET_LINK_LIBRARIES(action_recognition 
			${LIB_QUANTUM_BP_NETWORK}
			)
# 生成量子BP识别率程序
ADD_EXECUTABLE(Recognition_accuracy ./src/example/RecognitionAccuracy.cpp)
TARGET_LINK_LIBRARIES(Recognition_accuracy 
			${LIB_QUANTUM_BP_NETWORK}
			)

# 生成BP训练程序
SET(LIB_BP_NETWORK BP_NETWORK)
ADD_EXECUTABLE(bp_train ./src/example/Bp_Train.cpp)
TARGET_LINK_LIBRARIES(bp_train 
			${LIB_BP_NETWORK}
			)
# 生成BP识别程序
ADD_EXECUTABLE(bp_accuracy ./src/example/BP_RecognitionAccuracy.cpp)
TARGET_LINK_LIBRARIES(bp_accuracy 
			${LIB_BP_NETWORK}
			)
# 生成BP识别程序
ADD_EXECUTABLE(bp_recognition ./src/example/BP_Recognition.cpp)
TARGET_LINK_LIBRARIES(bp_recognition 
			${LIB_BP_NETWORK}
			)
# 生成BP测试程序
ADD_EXECUTABLE(bp_test ./src/example/BP_test.cpp)
TARGET_LINK_LIBRARIES(bp_test 
			${LIB_BP_NETWORK}
			)
# 生成BP测试程序
ADD_EXECUTABLE(qbp_test ./src/example/QBP_test.cpp)
TARGET_LINK_LIBRARIES(qbp_test 
			${LIB_QUANTUM_BP_NETWORK}
			)
# 生成动作识别程序
ADD_EXECUTABLE(video_recognition ./src/example/VideoActionRecognition.cpp)
TARGET_LINK_LIBRARIES(video_recognition 
			${LIB_BP_NETWORK}
			)
