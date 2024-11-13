//#include <opencv2/core.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/opencv.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image_write.h"

#include <iostream>
#include <sstream>
#include <mpi.h>

#include <filesystem> // Para crear carpetas

//using namespace cv;
using namespace std;
namespace fs = std::filesystem;



int main(int argc, char* argv[])
{
	int node, cantidadNodos;
	int nodoPrincipal = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &node);
	MPI_Comm_size(MPI_COMM_WORLD, &cantidadNodos);


	
	if (node == 0) {
		int widthInit = 0;
		int heightInit = 0;
		int channelsInit = 0;
		int widthEnd = 0;
		int heightEnd = 0;
		int channelsEnd = 0;
			

		unsigned char* imgInit = stbi_load("test1_b.jpg", &widthInit, &heightInit, &channelsInit, 0);
		vector<unsigned char*> vecImgs;
		if (imgInit == nullptr) {
			cerr << "Error cargando la imagen\n";
			return -1;
		}
		unsigned char* imgEnd = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);

		if (imgEnd== nullptr) {
			cerr << "Error cargando la imagen\n";
			return -1;
		}

		int imgSize = widthInit * heightInit * channelsInit;
		int fps = 4;
		int duration = 1;
		int frames = fps * duration;
			
		unsigned char* imgSuport = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);
		cout << "inicia proceso de cargas " << endl;
		for (int i = 1; i < cantidadNodos; ++i) {
			MPI_Send(&frames, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			cout << "envió frames: " << i << endl;
			MPI_Send(&widthInit, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			cout << "envió width: " << i << endl;
			MPI_Send(&heightInit, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			cout << "envió height: " << i << endl;
			MPI_Send(&channelsInit, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			cout << "envió channels: " << i << endl;
			MPI_Send(imgInit, imgSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
			cout << "envió imgInit: " << i << endl;
			MPI_Send(imgEnd, imgSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
			cout << "envió imgEnd: " << i << endl;
			cout << "envió todo a proceso: " << i<<endl;
		}

		for (int i = 1; i < cantidadNodos; i++)
		{
			int iterImage = i - 1;
			MPI_Send(&iterImage, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			cout << "envió iteración a proceso: " << i << endl;
		}

		for (int CP = 1; CP < cantidadNodos; CP++)
		{
			cout << "recivira imagen desde " << CP << endl;
			MPI_Recv(imgSuport, imgSize, MPI_UNSIGNED_CHAR, CP, CP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			cout << "recivio imagen desde " << CP << endl;
			vecImgs.push_back(imgSuport);
		}

		if (!fs::exists("images_Output")) {
			fs::create_directory("images_Output");
		}
		cout << "comienza a generar la imagenes" << endl;
		for (int i = 0; i < vecImgs.size(); i++) {
			string name = "images_Output/output_" + std::to_string(i) + ".png";
			vecImgs[i];
			stbi_write_png(name.c_str(), widthInit, heightInit, channelsInit, vecImgs[i], widthInit * channelsInit);

		}
		cout << "crea una copia" << endl;
		stbi_write_png("images_Output/copia.png", widthInit, heightInit, channelsInit, imgInit, widthInit * channelsInit);
		cout << "terminó de generar la imagenes" << endl;

		stbi_image_free(imgInit); // Libera la memoria cuando termines
		stbi_image_free(imgEnd);
		stbi_image_free(imgSuport);

			
	}
	else {
		int widthInit = 0, heightInit = 0, channelsInit = 0, widthEnd = 0, heightEnd = 0, channelsEnd = 0;

		int frames = 0;

		MPI_Recv(&frames, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Recv(&widthInit, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&heightInit, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&channelsInit, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int imgSize = widthInit * heightInit * channelsInit;
		unsigned char* imgInit = new unsigned char[imgSize];
		unsigned char* imgResult = new unsigned char[imgSize];
		unsigned char* imgEnd = new unsigned char[imgSize];

		MPI_Recv(imgInit, imgSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(imgEnd, imgSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			
		int IterImage = 0;
		int P;

		MPI_Recv(&IterImage, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		P = static_cast<float>(IterImage) / frames;
			
		for (int i = 0; i < imgSize; i += channelsInit) {

			imgResult[i] = ((int)imgInit[i] * (1 - P) + (int)imgEnd[i] * P); // Canal rojo
			imgResult[i + 1] = ((int)imgInit[i + 1] * (1 - P) + (int)imgEnd[i + 1] * P); // Canal verde
			imgResult[i + 2] = ((int)imgInit[i + 2] * (1 - P) + (int)imgEnd[i + 2] * P); // Canal azul
		}

		MPI_Send(imgResult, imgSize, MPI_UNSIGNED_CHAR, 0, node, MPI_COMM_WORLD);

		stbi_image_free(imgInit); // Libera la memoria cuando termines
		stbi_image_free(imgEnd);
		stbi_image_free(imgResult);
	}


	MPI_Finalize();
	return 0;
	
}


//int main(int argc, char* argv[])
//{
//	
//	int node, cantidadNodos;
//	int nodoPrincipal = 0;
//	MPI_Init(&argc, &argv);
//	MPI_Comm_rank(MPI_COMM_WORLD, &node);
//	MPI_Comm_size(MPI_COMM_WORLD, &cantidadNodos);
//
//
//	if (cantidadNodos > 1) {
//		
//		if (node == 0) {
//
//			cout << "inicia 0";
//			int widthInit = 0;
//			int heightInit = 0;
//			int channelsInit = 0;
//			int widthEnd = 0;
//			int heightEnd = 0;
//			int channelsEnd = 0;
//			int imgSize = widthInit * heightInit * channelsInit;
//			unsigned char* imgEnd = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);
//			unsigned char* imgInit = stbi_load("test1_b.jpg", &widthInit, &heightInit, &channelsInit, 0);
//
//			int fps = 24;
//			int duration = 4;
//			int frames = fps * duration;
//
//			unsigned char* imgSuport = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);
//			vector<unsigned char*> vecImgs;
//			if (imgInit == nullptr) {
//				cerr << "Error cargando la imagen\n";
//				return -1;
//			}
//
//			
//			for (int i = 1; i < cantidadNodos; ++i) {
//				MPI_Send(&frames, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
//				cout << i << endl;
//				cout << "algo1"<<endl;
//				MPI_Send(&widthInit, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
//				cout << "algo2" << endl;
//				MPI_Send(&heightInit, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
//				cout << "algo3" << endl;
//				MPI_Send(&channelsInit, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
//				cout << "algo4" << endl;
//				MPI_Send(imgInit, imgSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
//				cout << "algo5" << endl;
//				MPI_Send(&widthEnd, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
//				cout << "algo6" << endl;
//				MPI_Send(&heightEnd, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
//				cout << "algo7" << endl;
//				MPI_Send(&channelsEnd, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
//				cout << "algo8" << endl;
//				MPI_Send(imgEnd, imgSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
//				cout << "algo9" << endl;
//			}
//			cout << "termino bucle de envios" <<endl;
//			int contPros = 1;
//			int bucle = 0;
//			char helloMessage[100];
//			for (int i = 0; i < frames; i++) {
//				cout << "quiere enviar bucle" << endl;
//				MPI_Send(&bucle, 1, MPI_INT, contPros, 0, MPI_COMM_WORLD);
//				cout << "envio bucle" << endl;
//				cout << "recivirá bucle desde proceso: " << i << "===> ";
//				MPI_Recv(&bucle, 1, MPI_INT, 0, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//				cout << "recivido bucle: " << bucle;
//				MPI_Send(&i, 1, MPI_FLOAT, contPros, 0, MPI_COMM_WORLD);
//				cout << "envio i (para calcular P" << endl;
//				//MPI_Recv(&P, 1, MPI_FLOAT, 0, contPros, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//				//cout << "P =>" <<P << endl;
//				///*MPI_Recv(&helloMessage, 100, MPI_CHAR, 0, contPros, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//				//cout << helloMessage << endl;*/
//				contPros++;
//				if (contPros == cantidadNodos) {
//					cout << "prosesos insuficientes" << endl;
//					for (int CP = 1; CP < cantidadNodos; CP++)
//					{
//						cout << "recive imagen desde " << CP << endl;
//						MPI_Recv(imgSuport, imgSize, MPI_UNSIGNED_CHAR, 0, CP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//						cout << "recivio imagen desde " << CP << endl;
//						vecImgs.push_back(imgSuport);
//					}
//					contPros = 1;
//				}
//			}
//
//			if (contPros < cantidadNodos) {
//				for (int CP = 1; CP = contPros; CP++)
//				{
//					cout << "recivirá imagen desde " << CP << endl;
//					MPI_Recv(imgSuport, imgSize, MPI_UNSIGNED_CHAR, 0, CP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//					cout << "recivio imagen desde " << CP << endl;
//					vecImgs.push_back(imgSuport);
//				}
//			}
//			bucle = 1;
//			for (int i = 1; i < cantidadNodos; ++i) {
//				MPI_Send(&bucle, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
//			}
//			
//
//			/*CreateFolder("images_Output");*/
//			if (!fs::exists("images_Output")) {
//				fs::create_directory("images_Output");
//			}
//
//			for (int i = 0; i < vecImgs.size(); i++) {
//				string name = "images_Output/output_" + std::to_string(i) + ".png";
//				const char* nameChar = name.c_str();
//				int width = 0;
//				int height = 0;
//				int channels = 0;
//				stbi_write_png(nameChar, width, height, channels, vecImgs[i], width * channels);
//			}
//
//			stbi_image_free(imgInit); // Libera la memoria cuando termines
//			stbi_image_free(imgEnd);
//		}
//		else {
//			int widthInit = 0;
//			int heightInit = 0;
//			int channelsInit = 0;
//			int widthEnd = 0;
//			int heightEnd = 0;
//			int channelsEnd = 0;
//			unsigned char* imgEnd = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);
//			unsigned char* imgInit = stbi_load("test1_b.jpg", &widthInit, &heightInit, &channelsInit, 0);
//			int imgSize = widthInit * heightInit * channelsInit;
//
//			int frames = 0;
//
//			MPI_Recv(&frames, 5, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//
//			MPI_Recv(&widthInit, 5, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//			MPI_Recv(&heightInit, 5, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//			MPI_Recv(&channelsInit, 5, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//			MPI_Recv(imgInit, imgSize, MPI_UNSIGNED_CHAR, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//
//			MPI_Recv(&widthEnd, 5, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//			MPI_Recv(&heightEnd, 5, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//			MPI_Recv(&channelsEnd, 5, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//			MPI_Recv(imgEnd, imgSize, MPI_UNSIGNED_CHAR, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//
//			float P;
//			int IterImage = 0;
//			int bucle = 0;
//			unsigned char* imgResult = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);
//
//			while (bucle == 0) {
//
//				MPI_Recv(&bucle, 1, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//				MPI_Send(&bucle, 1, MPI_INT, 0, node, MPI_COMM_WORLD);
//				if (bucle == 0) {
//
//					MPI_Recv(&IterImage, 1, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//					P = static_cast<float>(IterImage) / frames;
//					/*MPI_Send(&P, 1, MPI_FLOAT, 0, node, MPI_COMM_WORLD);*/
//					for (int i = 0; i < 10 * channelsInit; i += channelsInit) {
//
//						imgResult[i] = ((int)imgInit[i] * (1 - P) + (int)imgEnd[i] * P); // Canal rojo
//						imgResult[i + 1] = ((int)imgInit[i + 1] * (1 - P) + (int)imgEnd[i + 1] * P); // Canal verde
//						imgResult[i + 2] = ((int)imgInit[i + 2] * (1 - P) + (int)imgEnd[i + 2] * P); // Canal azul
//					}
//					
//
//					MPI_Send(imgResult, imgSize, MPI_UNSIGNED_CHAR, 0, node, MPI_COMM_WORLD);
//
//				}
//			}
//		}
//
//	}
//		
//	MPI_Finalize();
//}
