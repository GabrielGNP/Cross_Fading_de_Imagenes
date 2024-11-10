#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <cstdlib>
#include <conio.h>  // Para getch() en Windows

#include <vector>

#include <windows.h>
#include <string>
#include <cstring>
#include <vector>

#include <filesystem> // Para crear carpetas

using namespace cv;
using namespace std;
namespace fs = std::filesystem;




// Parámetros
int fps = 24; //imagenes por segundo
int duration = 4; // segundos de duración del video
int frames = fps * duration; // cantidad de frames totales
std::string outputDir = "output_frames"; // directorio de salida

void SetColor(int textColor, int bgColor) {
	//0: Negro
	//1: Azul
	//2: Verde
	//3: Aqua
	//4: Rojo
	//5: Púrpura
	//6: Amarillo
	//7: Blanco (por defecto)
	//8: Gris
	//9: Azul claro
	//10: Verde claro, etc.
	// Obtener el identificador de la consola
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// Establecer el color
	SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

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

std::string pathProgram = GetPathProgram();

void CreateFolder(std::string folder) {
	// Crear la carpeta si no existe
	if (!fs::exists(folder)) {
		fs::create_directory(folder);
	}
}

// ========= Mostramos la imagen =========
void showImage(Mat img) {
	namedWindow("First OpenCV Application", WINDOW_AUTOSIZE);
	imshow("First OpenCV Application", img);
	moveWindow("First OpenCV Application", 0, 45);
	waitKey(0);
	destroyAllWindows();
}

int generatedVideo(Mat imgInit, Mat imgEnd) {

	Size frameSize = imgInit.size();

	// Inicializar el VideoWriter
	VideoWriter video("output_frames/output_video.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frameSize);

	if (!video.isOpened()) {
		std::cerr << "Error al abrir el archivo de video" << std::endl;
		return -1;
	}

	system("cls");
	for (int i = 0; i < frames; ++i) {
		float P = static_cast<float>(i) / frames;
		Mat result = imgInit.clone();
		/*addWeighted(imgInit, 1 - P, imgEnd, P, 0.0, result);*/ // función para crear las imagenes.

		 // Aplicar cross-fading manualmente en cada píxel
		for (int y = 0; y < imgInit.rows; ++y) {
			for (int x = 0; x < imgInit.cols; ++x) {
				Vec3b pixelInit = imgInit.at<Vec3b>(y, x);
				Vec3b pixelEnd = imgEnd.at<Vec3b>(y, x);
				Vec3b& pixelResult = result.at<Vec3b>(y, x);

				pixelResult[0] = static_cast<uchar>(pixelInit[0] * P + pixelEnd[0] * (1 - P)); // Canal azul
				pixelResult[1] = static_cast<uchar>(pixelInit[1] * P + pixelEnd[1] * (1 - P)); // Canal verde
				pixelResult[2] = static_cast<uchar>(pixelInit[2] * P + pixelEnd[2] * (1 - P)); // Canal rojo
			}
		}


		std::string fileName = "output_frames/frame_" + std::to_string(i) + ".jpg";
		/*imwrite(fileName, result);*/
		video.write(result);
		system("cls");
		std::cout << "<";
		for (int t = 0; t < 100; t++) {
			if ((t * 100) / frames <= (i * 100) / frames)
			{
				std::cout << "=";
			}
			else {
				std::cout << " ";
			}
		}
		std::cout << ">" << (i * 100) / frames;

	}
	std::cout << std::endl;

	// Liberar el archivo de video
	video.release();

	return 0;
}


void menu() {
	std::cout << "cambiar";



}

int main()
{
	int posMenu = 0;
	vector<String> options = { "Seleccionar imagenes","Cambiar fps", "Cambiar duracion", "Crear imagenes", "Crear video", "Salir" };

	while (true) {
		system("cls");
		for (int op = 0; op < options.size(); op++) {
			if (op == posMenu)
			{
				SetColor(0, 8);
				std::cout << "> " << options[op] << std::endl;
			}
			else
			{
				SetColor(7, 0);
				std::cout << "  " << options[op] << std::endl;
			}
			SetColor(7, 0);
		}
		cout << endl;
		cout << "fps: "; SetColor(2, 0); cout << fps << endl; SetColor(7, 0);
		cout << "duracion: "; SetColor(2, 0); cout << duration << endl; SetColor(7, 0);

		int key = _getch();  // Captura el primer carácter

		if (key == 224) {  // Si es una secuencia de tecla especial (teclas de dirección)
			key = _getch();

			switch (key) {
			case 72:  // Flecha arriba
				if (posMenu > 0)
					posMenu--;
				break;
			case 80:  // Flecha abajo
				if (posMenu < options.size() - 1)
					posMenu++;
				break;
			case 75:  // Flecha izquierda
			{
				switch (posMenu)
				{
				case 1:
					fps--;
					break;

				case 2:
					duration--;
					break;
				}
				break;
			}
			case 77:  // Flecha derecha
			{
				switch (posMenu)
				{
				case 1:
					fps++;
					break;

				case 2:
					duration++;
					break;
				}
				break;
			}
			}
		}
		else if (key == 27) {  // 27 es la tecla ESC
			std::cout << "Escape presionado. Saliendo..." << std::endl;
			break;
		}
	}

	/*CreateFolder(outputDir);

	Mat imgInit = imread(pathProgram +"/test1_a.jpg");
	Mat imgEnd = imread(pathProgram + "/test1_b.jpg");

	if (generatedVideo(imgInit, imgEnd) == 0)
		std::cout << "Vidéo creado" << endl;
	else
		std::cout << "no se pudo crear el video";*/


	return 0;

}
