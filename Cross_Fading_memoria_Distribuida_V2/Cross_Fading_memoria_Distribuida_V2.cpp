#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image_write.h"

#include <iostream>

#include <mpi.h>

#include <filesystem> // Para crear carpetas

using namespace cv;
using namespace std;
namespace fs = std::filesystem;


int main(int argc, char* argv[])
{
	cout << "inicia todo";
	int node, cantidadNodos;
	int nodoPrincipal = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &node);
	MPI_Comm_size(MPI_COMM_WORLD, &cantidadNodos);


	if (cantidadNodos > 1) {
		cout << "inicia 0";
		if (node == 0) {

			cout << "inicia 0";
			int widthInit = 0;
			int heightInit = 0;
			int channelsInit = 0;
			int widthEnd = 0;
			int heightEnd = 0;
			int channelsEnd = 0;
			int imgSize = widthInit * heightInit * channelsInit;
			unsigned char* imgEnd = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);
			unsigned char* imgInit = stbi_load("test1_b.jpg", &widthInit, &heightInit, &channelsInit, 0);

			int fps = 24;
			int duration = 4;
			int frames = fps * duration;




			unsigned char* imgSuport = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);
			vector<unsigned char*> vecImgs;
			if (imgInit == nullptr) {
				cerr << "Error cargando la imagen\n";
				return -1;
			}

			for (int i = 1; i < cantidadNodos - 1; i++) {
				cout << "algo1";
				MPI_Send(&widthInit, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				cout << "algo2";
				MPI_Send(&heightInit, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				cout << "algo3";
				MPI_Send(&channelsInit, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				cout << "algo4";
				MPI_Send(imgInit, imgSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
				cout << "algo5";
				MPI_Send(&widthEnd, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				cout << "algo6";
				MPI_Send(&heightEnd, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				cout << "algo7";
				MPI_Send(&channelsEnd, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				cout << "algo8";
				MPI_Send(imgEnd, imgSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
				cout << "algo9";
			}

			int contPros = 1;
			int bucle = 0;
			for (int i = 0; i < frames; i++) {
				float P = static_cast<float>(i) / frames;
				MPI_Send(&bucle, 1, MPI_INT, contPros, 0, MPI_COMM_WORLD);
				MPI_Send(&P, 1, MPI_FLOAT, contPros, 0, MPI_COMM_WORLD);
				cout << i;
				contPros++;
				if (contPros == cantidadNodos - 1) {
					for (int CP = 1; CP < cantidadNodos - 1; CP++)
					{
						MPI_Recv(imgSuport, imgSize, MPI_UNSIGNED_CHAR, CP, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						vecImgs.push_back(imgSuport);
					}
					contPros = 1;
				}
			}

			if (contPros < cantidadNodos) {
				for (int CP = 1; CP < contPros - 1; CP++)
				{
					MPI_Recv(imgSuport, imgSize, MPI_INT, CP, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					vecImgs.push_back(imgSuport);
				}
			}
			bucle = 1;
			MPI_Send(&bucle, 1, MPI_INT, 0, contPros, MPI_COMM_WORLD);

			/*CreateFolder("images_Output");*/
			if (!fs::exists("images_Output")) {
				fs::create_directory("images_Output");
			}

			for (int i = 0; i < vecImgs.size(); i++) {
				string name = "images_Output/output_" + to_string(i) + ".png";
				const char* nameChar = name.c_str();
				int width = 0;
				int height = 0;
				int channels = 0;
				stbi_write_png(nameChar, width, height, channels, vecImgs[i], width * channels);
			}

			stbi_image_free(imgInit); // Libera la memoria cuando termines
			stbi_image_free(imgEnd);
		}
		else {
			int widthInit = 0;
			int heightInit = 0;
			int channelsInit = 0;
			int widthEnd = 0;
			int heightEnd = 0;
			int channelsEnd = 0;
			unsigned char* imgEnd = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);
			unsigned char* imgInit = stbi_load("test1_b.jpg", &widthInit, &heightInit, &channelsInit, 0);
			int imgSize = widthInit * heightInit * channelsInit;

			MPI_Recv(&widthInit, 1, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&heightInit, 1, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&channelsInit, 1, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(imgInit, imgSize, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			MPI_Recv(&widthEnd, 1, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&heightEnd, 1, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&channelsEnd, 1, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(imgEnd, imgSize, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			float P = 0.0;
			int bucle = 0;
			unsigned char* imgResult = stbi_load("test1_a.jpg", &widthEnd, &heightEnd, &channelsEnd, 0);

			while (bucle == 0) {
				MPI_Recv(&bucle, 1, MPI_INT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if (bucle == 0) {
					MPI_Recv(&P, 1, MPI_FLOAT, 0, node, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

					for (int i = 0; i < 10 * channelsInit; i += channelsInit) {

						imgResult[i] = ((int)imgInit[i] * (1 - P) + (int)imgEnd[i] * P); // Canal rojo
						imgResult[i + 1] = ((int)imgInit[i + 1] * (1 - P) + (int)imgEnd[i + 1] * P); // Canal verde
						imgResult[i + 2] = ((int)imgInit[i + 2] * (1 - P) + (int)imgEnd[i + 2] * P); // Canal azul
					}

					MPI_Send(imgResult, imgSize, MPI_UNSIGNED_CHAR, 0, node, MPI_COMM_WORLD);
				}
			}
		}

	}
		
	MPI_Finalize();
}

