// ʹ������ͷ���м��
// ʹ��OpenCV��ʵ��
// ������OpenCV�Ļ���
// 1. ��װOpencv
// 2. ����opencv��ͷ�ļ�����Ŀ¼
// 3. ���opencv�Ŀ��ļ�Ŀ¼
// 4. ����VS��ʹ����Щ���ļ�
/*
	opencv_core249d.lib
	opencv_highgui249d.lib
	opencv_imgproc249d.lib

	opencv_core249d.dll
	opencv_highgui249d.dll
	opencv_imgproc249d.dll
*/
#include "network_tools.h"
 
#include <opencv.hpp>
#include <opencv2/highgui.hpp>
#include <openservice.h>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <highgui.hpp>
#include <opencv2/opencv.hpp>




using namespace cv;

SOCKET serverSocket;
sockaddr_in sockAddr;  //�������˵�ַ

#define SERVER_PORT  2021

VideoCapture cap; //����ͷ



class Face_detection_demo
{
public:

	void face_detection_demo();
};



void Face_detection_demo::face_detection_demo() {
	std::string root_dir = ".//face_detector";
	std::string root_dir1 = ".//face_detector//opencv_face_detector_uint8.pb";
	std::string root_dir2 = ".//face_detector//opencv_face_detector.pbtxt";

	dnn::Net net = dnn::readNetFromTensorflow(root_dir1, root_dir2);


	////printf("\033[0;%d;40m%s\033[0m\n",color++,msg);
	//const char buff[1024] = "";

	//const char msg[] = "please input the video position \n>";
	//printf("\033[0;31;40m%s\033[0m", msg);
	//scanf("%s", &buff);

	VideoCapture capture;//F:/WorkCpp/�������/images/example_dsh.mp4
	capture.open(0);
	const char empty[] = "frame empty";
	const char meg_succeed[] = "imagine writed succeed";
	Mat frame;
	int count = 0;
	int count_succeed = 0;
	while (true)
	{
		capture.read(frame);
		if (frame.empty()) {
			printf("\033[0;31;40m%s\033[0m", empty);
			break;
		}
		Mat blob = dnn::blobFromImage(frame, 1.0, Size(300, 300), Scalar(104, 177, 123), false, false);
		net.setInput(blob);// NCHW
		Mat probs = net.forward(); // 
		Mat detectionMat(probs.size[2], probs.size[3], CV_32F, probs.ptr<float>());
		// �������
		for (int i = 0; i < detectionMat.rows; i++)
		{
			float confidence = detectionMat.at<float>(i, 2);
			if (confidence > 0.5)
			{
				int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
				int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
				int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
				int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);
				Rect box(x1, y1, x2 - x1, y2 - y1);
				rectangle(frame, box, Scalar(0, 0, 255), 2, 8, 0);
			}


		}
		namedWindow("�������", WINDOW_FREERATIO);
		imshow("�������", frame);
		int c = waitKey(1);
		if (c == 27) { // �˳�
			break;
		}

		count++;
	}
}




void touPai(const char* fileName) {
	
	

	Mat frame; //opencv�б�ʾͼƬ����
	cap >> frame;
	imwrite(fileName, frame);
	//IplImage outImage =  frame; //=IplImageҲ��opencv����������ͼƬ���ݵ�
	//cvSaveImage(fileName, &outImage, 0); // ����ͼƬ
	
}

int main(void) {
	// ��1����������ʾ�����ĸ�IP��ַ���Լ�أ���˭���Խ��룩
	// �����0�� ��ʾ�κοͻ��ˣ������Խ���
	serverSocket = webInitTCP(0, &sockAddr, SERVER_PORT);
	// ���׽���
	bind(serverSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	// ������������
	listen(serverSocket, 1);

	// ����ߵĵ�ַ
	SOCKADDR client;
	int nSize = sizeof(client);
	SOCKET clientSock;

	

	while (1) {
		// �ȴ����˷��ͼ��ָ��
		printf("�ȴ����ƶ˷�������...\n");
		clientSock = accept(serverSocket, &client, &nSize);
		printf("���ƶ��Ѿ����룡\n");

		cap.open(0);

		while (1) {
			// ���ܡ����ָ�
			char buff[4096];

			printf("�ȴ�ָ��...\n");
			int ret = recv(clientSock, buff, sizeof(buff), 0);
			if (ret <= 0) break;

			// �ж��յ����ǲ��ǡ���ء�ָ��
			if (strcmp(buff, "TOU_PAI") == 0) {
				//  ��ʽ���
				touPai("tou_pai.jpg");
				printf("�Ѿ����㣡\n");

				FILE* file = fopen("tou_pai.jpg", "rb");
				fseek(file, 0, SEEK_END);
				int len = ftell(file);
				send(clientSock,(char*)&len, 4, NULL);
				
				fseek(file, 0, SEEK_SET);
				while (1) {
					int ret = fread(buff, 1, 4096, file);
					if (ret <= 0) break;
					send(clientSock, buff, ret, NULL);
				}
				fclose(file);
				printf("�Ѿ��������\n");

				/*char pack[] = { 0x01, 0x02, 0x03, 0x04};
				send(clientSock, pack, 4, NULL);
				printf("��������Ѿ��������\n");*/
			}
		}

		
		closesocket(clientSock);
		cap.release(); //�ر�����ͷ
	}

	closesocket(serverSocket);

	
	return 0;
}
