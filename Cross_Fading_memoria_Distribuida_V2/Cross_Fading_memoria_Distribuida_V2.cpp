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
#include <windows.h> // Para para el goto y obtener el Path del programa
#include <conio.h>  // Para getch() en Windows

#include <mpi.h>

#include <filesystem> // Para crear carpetas

#include <chrono> // Para medir el tiempo

//using namespace cv;
using namespace std;
namespace fs = std::filesystem;
std::string GetPathProgram() {

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	// Eliminar el nombre del archivo (obtiene solo el directorio)
	char* lastSlash = strrchr(buffer, '\\');
	if (lastSlash != nullptr) {
		*lastSlash = '\0';  // Reemplazar la última barra con un terminador de cadena
	}

	std::string path(buffer);

	return path;
}

void CreateFolder(std::string folder) {
	// Crear la carpeta si no existe
	if (!fs::exists(folder)) {
		fs::create_directory(folder);
	}
}

int createImage(string name, int width, int height, int channels, unsigned char* img) {
	/*CreateFolder("images_Output");*/
	if (!fs::exists("images_Output")) {
		fs::create_directory("images_Output");
		return -1;
	}

	stbi_write_png(name.c_str(), width, height, channels, img, width * channels);
	return 0;
}

int main(int argc, char* argv[])
{
	
	int node, cantidadNodos;
	int nodoPrincipal = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &node);
	MPI_Comm_size(MPI_COMM_WORLD, &cantidadNodos);


	if (cantidadNodos > 1) {
		
		if (node == 0) {
			std::string pathProgram = GetPathProgram();
			string nameInputFolder = "Images";
			string nameOutputImages = "Images_Output";
			CreateFolder(nameOutputImages);

			string nameImageInit = pathProgram + "/" + nameInputFolder + "/test1_a(x5000).jpg";
			string nameImageEnd = pathProgram + "/" + nameInputFolder + "/test1_b(x5000).jpg";

			int widthInit = 0, heightInit = 0, channelsInit = 0;
			int widthEnd = 0, heightEnd = 0, channelsEnd = 0;

			
			unsigned char* imgInit = stbi_load(nameImageInit.c_str(), &widthInit, &heightInit, &channelsInit, 0);
			if (imgInit == nullptr) {
				cerr << "Error cargando la imagen inicial\n";
				return -1;
			}
			
			unsigned char* imgEnd = stbi_load(nameImageEnd.c_str(), &widthEnd, &heightEnd, &channelsEnd, 0);
			if (imgEnd == nullptr) {
				cerr << "Error cargando la imagen final\n";
				return -1;
			}

			int fps = 24;
			int duration = 4;
			int frames = fps * duration;
			int imgSize = widthInit * heightInit * channelsInit;
			vector<unsigned char*> vecImgs;
			cout << "comienza a cronometrar...";
			auto start = std::chrono::high_resolution_clock::now(); // <========= toma el tiempo actual para el inicio del cronometro
			for (int i = 1; i < cantidadNodos; ++i) {
				MPI_Send(&frames, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&widthInit, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&heightInit, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&channelsInit, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(imgInit, imgSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
				MPI_Send(imgEnd, imgSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
			}
			/*cout << "termino bucle de envios" <<endl;*/

			int contPros = 1;
			int bucle = 0;
			float P = 0.0;
			int cont = 0;
			for (int i = 0; i < frames; i++) {
				MPI_Send(&bucle, 1, MPI_INT, contPros, 0, MPI_COMM_WORLD);
				P = static_cast<float>(i) / frames;
				MPI_Send(&P, 1, MPI_FLOAT, contPros, 0, MPI_COMM_WORLD);
				/*cout << "envio P="<<P<<"" << endl;*/

				contPros++;
				if (contPros == cantidadNodos) {
					/*cout << "prosesos insuficientes" << endl;
					*/
					/*std::cout << "creando imagenes";*/
					for (int CP = 1; CP < cantidadNodos; CP++)
					{
						unsigned char* imgResult = new unsigned char[imgSize];
						MPI_Recv(imgResult, imgSize, MPI_UNSIGNED_CHAR, CP, CP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						vecImgs.push_back(imgResult);
						/*string name = pathProgram + "/"+ nameOutputImages + "/output_" + std::to_string(CP+i-cantidadNodos+1) + ".png";
						createImage(name, widthInit, heightInit, channelsInit, imgResult);*/
						cont = i;
						/*std::cout << ".";*/
					}
					contPros = 1;
				}
			}
			/*cout << "comienza a traer toda la info faltante" << endl;*/
			if (contPros > 1) {
				for (int CP = 1; CP = contPros; CP++)
				{
					unsigned char* imgResult = new unsigned char[imgSize];
					MPI_Recv(imgResult, imgSize, MPI_UNSIGNED_CHAR, CP, CP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					/*vecImgs.push_back(imgResult);*/
					/*string name = pathProgram + "/" + nameOutputImages + "/output_" + std::to_string(CP + cont - cantidadNodos + 1) + ".png";
					createImage(name, widthInit, heightInit, channelsInit, imgResult);*/
				}
			}

			/*cout << "==== ya trajo todo ====" << endl;*/
			bucle = 1;
			/*cout << "comienza a finalizar los procesos esclavos" << endl;*/
			for (int i = 1; i < cantidadNodos; ++i) {
				MPI_Send(&bucle, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
			/*cout << "==== finalizó todos los procesos esclavos ====" << endl;*/
			cout << endl;
			auto end = std::chrono::high_resolution_clock::now();// <========= toma el tiempo actual para el fin del cronometro
			auto durationms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); // <======== calcula el tiempo en función del final y el inicio
			auto durationsec = std::chrono::duration_cast<std::chrono::seconds>(end - start); // <======== calcula el tiempo en función del final y el inicio
			cout << "Tiempo tardado en generar el video: " << durationms.count() << "ms" << endl;
			cout << "Tiempo tardado en generar el video: " << durationsec.count() << "seg" << endl;
			cout << "comienza a generar la imagenes" << endl;
			for (int i = 0; i < vecImgs.size(); i++) {
				string name = "images_Output/output_" + std::to_string(i) + ".png";
				stbi_write_png(name.c_str(), widthInit, heightInit, channelsInit, vecImgs[i], widthInit * channelsInit);
			}
			vecImgs.clear();
			cout << " ======= Imagenes generadas completamente =======" << endl;
			cout << " presione cualquier tecla para salir..." << endl;
			cin>> nameInputFolder;
			stbi_image_free(imgInit); // Libera la memoria cuando termines
			stbi_image_free(imgEnd);
			MPI_Finalize();
		}
		else {
			int widthInit = 0, heightInit = 0, channelsInit = 0;
			int frames = 0;

			MPI_Recv(&frames, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			MPI_Recv(&widthInit, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&heightInit, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&channelsInit, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			int imgSize = widthInit * heightInit * channelsInit;
			unsigned char* imgInit = new unsigned char[imgSize];
			unsigned char* imgResult = new unsigned char[imgSize];
			unsigned char* imgEnd = new unsigned char[imgSize];

			MPI_Recv(imgInit, imgSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(imgEnd, imgSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			float P = 0.0;
			int bucle = 0;

			while (bucle == 0) {

				MPI_Recv(&bucle, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				if (bucle == 0) {

					MPI_Recv(&P, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					for (int i = 0; i < imgSize; i += channelsInit) {
					
						imgResult[i] = ((int)imgInit[i] * (1 - P) + (int)imgEnd[i] * P); // Canal rojo
						imgResult[i + 1] = ((int)imgInit[i + 1] * (1 - P) + (int)imgEnd[i + 1] * P); // Canal verde
						imgResult[i + 2] = ((int)imgInit[i + 2] * (1 - P) + (int)imgEnd[i + 2] * P); // Canal azul
					}
					
					MPI_Send(imgResult, imgSize, MPI_UNSIGNED_CHAR, 0, node, MPI_COMM_WORLD);

				}
			}
		}

	}
	else {

		std::string pathProgram = GetPathProgram();
		string nameInputFolder = "Images";
		string nameOutputImages = "Images_Output";
		CreateFolder(nameOutputImages);
		string nameImageInit = pathProgram + "/" + nameInputFolder + "/test1_a(x800).jpg";
		string nameImageEnd = pathProgram + "/" + nameInputFolder + "/test1_b(x800).jpg";
		
		int widthInit = 0, heightInit = 0, channelsInit = 0;
		int widthEnd = 0, heightEnd = 0, channelsEnd = 0;

		unsigned char* imgInit = stbi_load(nameImageInit.c_str(), &widthInit, &heightInit, &channelsInit, 0);
		if (imgInit == nullptr) {
			cerr << "Error cargando la imagen inicial\n";
			return -1;
		}

		unsigned char* imgEnd = stbi_load(nameImageEnd.c_str(), &widthEnd, &heightEnd, &channelsEnd, 0);
		if (imgEnd == nullptr) {
			cerr << "Error cargando la imagen final\n";
			return -1;
		}

		int fps = 24;
		int duration = 4;
		int frames = fps * duration;
		int imgSize = widthInit * heightInit * channelsInit;
		vector<unsigned char*> vecImgs;

		float P = 0.0;
		cout << "comienza a cronometrar...";
		auto start = std::chrono::high_resolution_clock::now(); // <========= toma el tiempo actual para el inicio del cronometro
		for (int f = 0; f < frames; f++) {
			P = static_cast<float>(f) / frames;
			unsigned char* imgResult = new unsigned char[imgSize];
			for (int i = 0; i < imgSize; i += channelsInit) {
				imgResult[i] = ((int)imgInit[i] * (1 - P) + (int)imgEnd[i] * P); // Canal rojo
				imgResult[i + 1] = ((int)imgInit[i + 1] * (1 - P) + (int)imgEnd[i + 1] * P); // Canal verde
				imgResult[i + 2] = ((int)imgInit[i + 2] * (1 - P) + (int)imgEnd[i + 2] * P); // Canal azul
			}
			/*string name = pathProgram + "/" + nameOutputImages + "/output_" + std::to_string(f) + ".png";
			createImage(name, widthInit, heightInit, channelsInit, imgResult);*/
			vecImgs.push_back(imgResult);
		}
		cout << endl;
		auto end = std::chrono::high_resolution_clock::now();// <========= toma el tiempo actual para el fin del cronometro
		auto durationms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); // <======== calcula el tiempo en función del final y el inicio
		auto durationsec = std::chrono::duration_cast<std::chrono::seconds>(end - start); // <======== calcula el tiempo en función del final y el inicio
		cout << "Tiempo tardado en generar el video: " << durationms.count() << "ms" << endl;
		cout << "Tiempo tardado en generar el video: " << durationsec.count() << "seg" << endl;
		for (int i = 0; i < vecImgs.size(); i++) {
			string name = "images_Output/output_" + std::to_string(i) + ".png";
			stbi_write_png(name.c_str(), widthInit, heightInit, channelsInit, vecImgs[i], widthInit * channelsInit);
		}
		cout << " ======= Imagenes generadas completamente =======" << endl;
		cout << " presione cualquier tecla para salir..." << endl;
		/*_getch();*/
		vecImgs.clear();
		cin >> nameInputFolder;
		stbi_image_free(imgInit); // Libera la memoria cuando termines
		stbi_image_free(imgEnd);
		MPI_Finalize();

		
	}
		
	MPI_Finalize();
}
