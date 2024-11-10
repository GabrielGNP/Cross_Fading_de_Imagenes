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
void gotoxy(int x, int y) {
	// Obtenemos el handle de la consola
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Establecemos la posición del cursor
	COORD coord;
	coord.X = x;
	coord.Y = y;

	// Verificamos si se puede mover el cursor
	if (!SetConsoleCursorPosition(hConsole, coord)) {
		std::cerr << "Error al mover el cursor" << std::endl;
	}
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

int GenerateImages(Mat imgInit, Mat imgEnd) {
	int frames = fps * duration; // cantidad de frames totales
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

				pixelResult[0] = static_cast<uchar>(pixelInit[0] * (1 - P) + pixelEnd[0] * P); // Canal azul
				pixelResult[1] = static_cast<uchar>(pixelInit[1] * (1 - P) + pixelEnd[1] * P); // Canal verde
				pixelResult[2] = static_cast<uchar>(pixelInit[2] * (1 - P) + pixelEnd[2] * P); // Canal rojo
			}
		}

		std::string fileName = "Images_output/frame_" + std::to_string(i) + ".jpg";
		imwrite(fileName, result);
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

	return 0;
}

int GeneratedVideo(Mat imgInit, Mat imgEnd) {
	int frames = fps * duration; // cantidad de frames totales
	Size frameSize = imgInit.size();

	// Inicializar el VideoWriter
	VideoWriter video("Video_Output/output_video.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frameSize);

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

				pixelResult[0] = static_cast<uchar>(pixelInit[0] * (1 - P) + pixelEnd[0] * P); // Canal azul
				pixelResult[1] = static_cast<uchar>(pixelInit[1] * (1 - P) + pixelEnd[1] * P); // Canal verde
				pixelResult[2] = static_cast<uchar>(pixelInit[2] * (1 - P) + pixelEnd[2] * P); // Canal rojo
			}
		}

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

void CreateImages() {
	CreateFolder("Images_Output");

	Mat imgInit = imread(pathProgram + "/Images/test1_a.jpg");
	Mat imgEnd = imread(pathProgram + "/Images/test1_b.jpg");

	if (GenerateImages(imgInit, imgEnd) == 0)
	{
		std::cout << "Imagenes creadas" << endl << endl;
		std::cout << "            Presione cualquier tecla par acontinuar..." << endl;
		_getch();
		system("cls");
	}

	else
		std::cout << "Hubo un error al crear las imagenes";
}
void CreateVideo() {
	int stop;
	CreateFolder("Video_Output");
	system("cls");
	
	Mat imgInit = imread(pathProgram + "/Images/test1_a.jpg");
	Mat imgEnd = imread(pathProgram + "/Images/test1_b.jpg");

	if (GeneratedVideo(imgInit, imgEnd) == 0)
	{
		std::cout << "Video creado" << endl << endl;
		std::cout << "            Presione cualquier tecla par acontinuar..." << endl;
		_getch();
		system("cls");
	}		
	else
		std::cout << "no se pudo crear el video";
}

void PrintOptions(int posMenu, vector<String> options) {
	for (int op = 0; op < options.size(); op++) {
		if (op == posMenu)
		{
			SetColor(0, 8);
			std::cout << "> " << options[op] << endl;
		}
		else
		{
			SetColor(7, 0);
			std::cout << "  " << options[op];
			switch (op)
			{
			case 1:
				SetColor(7, 0);
				std::cout << " => <";
				SetColor(8, 0);
				std::cout << fps;
				SetColor(7, 0);
				std::cout << ">" << std::endl;
				break;

			case 2:
				SetColor(7, 0);
				std::cout << " => <";
				SetColor(8, 0);
				std::cout << duration;
				SetColor(7, 0);
				std::cout << ">" << std::endl;
				break;
			default:
				std::cout << std::endl;
				break;
			}
		}
		SetColor(7, 0);
	}
}

void menu(int posMenu, vector<String> options) {
	bool exit = false;
	PrintOptions(posMenu, options);
	while (!exit) {
		int key = _getch();  // Captura el primer carácter

		if (key == 224) {  // Si es una secuencia de tecla especial (teclas de dirección)
			key = _getch();

			switch (key) {
			case 72:  // Flecha arriba
				if (posMenu > 0)
				{
					posMenu--;
					gotoxy(0, posMenu);
					SetColor(0, 8);
					std::cout << "> " << options[posMenu];
					SetColor(7, 0);
					switch (posMenu)
					{
					case 1:
						std::cout << " => <";
						SetColor(2, 0);
						std::cout << fps;
						SetColor(7, 0);
						std::cout << ">" << std::endl;
						break;

					case 2:
						std::cout << " => <";
						SetColor(2, 0);
						std::cout << duration;
						SetColor(7, 0);
						std::cout << ">" << std::endl;
						break;
					default:
						std::cout << std::endl;
						break;
					}
					gotoxy(0, posMenu + 1);
					SetColor(7, 0);
					std::cout << "  " << options[posMenu + 1];
					switch (posMenu + 1)
					{
					case 1:
						SetColor(7, 0);
						std::cout << " => <";
						SetColor(8, 0);
						std::cout << fps;
						SetColor(7, 0);
						std::cout << ">" << std::endl;
						break;

					case 2:
						SetColor(7, 0);
						std::cout << " => <";
						SetColor(8, 0);
						std::cout << duration;
						SetColor(7, 0);
						std::cout << ">" << std::endl;
						break;
					default:
						std::cout << std::endl;
						break;
					}
				}
				else {
					SetColor(7, 0);
					gotoxy(0, posMenu);
					std::cout << "  " << options[0];
					posMenu = options.size() - 1;
					SetColor(0, 8);
					gotoxy(0, posMenu);
					std::cout << "  " << options[options.size() - 1];
				}
				break;
			case 80:  // Flecha abajo
				if (posMenu < options.size() - 1) {
					posMenu++;
					gotoxy(0, posMenu);
					SetColor(0, 8);
					std::cout << "> " << options[posMenu];
					SetColor(7, 0);
					switch (posMenu)
					{
					case 1:
						std::cout << " => <";
						SetColor(2, 0);
						std::cout << fps;
						SetColor(7, 0);
						std::cout << ">" << std::endl;
						break;

					case 2:
						std::cout << " => <";
						SetColor(2, 0);
						std::cout << duration;
						SetColor(7, 0);
						std::cout << ">" << std::endl;
						break;
					default:
						std::cout << std::endl;
						break;
					}
					gotoxy(0, posMenu - 1);
					SetColor(7, 0);
					std::cout << "  " << options[posMenu - 1];
					switch (posMenu - 1)
					{
					case 1:
						SetColor(7, 0);
						std::cout << " => <";
						SetColor(8, 0);
						std::cout << fps;
						SetColor(7, 0);
						std::cout << ">" << std::endl;
						break;

					case 2:
						SetColor(7, 0);
						std::cout << " => <";
						SetColor(8, 0);
						std::cout << duration;
						SetColor(7, 0);
						std::cout << ">" << std::endl;
						break;
					default:
						std::cout << std::endl;
						break;
					}
				}
				else {
					SetColor(7, 0);
					gotoxy(0, posMenu);
					std::cout << "  " << options[options.size() - 1];
					posMenu = 0;
					SetColor(0, 8);
					gotoxy(0, posMenu);
					std::cout << "  " << options[0];
					posMenu = 0;
				}

				break;
			case 75:  // Flecha izquierda
			{
				switch (posMenu)
				{
				case 1:
					fps--;
					gotoxy(0, posMenu);
					SetColor(0, 8);
					std::cout << "> " << options[posMenu];
					SetColor(7, 0);
					std::cout << " => <";
					SetColor(2, 0);
					std::cout << fps;
					SetColor(7, 0);
					std::cout << ">" << std::endl;
					break;

				case 2:
					duration--;
					gotoxy(0, posMenu);
					SetColor(0, 8);
					std::cout << "> " << options[posMenu];
					SetColor(7, 0);
					std::cout << " => <";
					SetColor(2, 0);
					std::cout << duration;
					SetColor(7, 0);
					std::cout << ">" << std::endl;
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
					gotoxy(0, posMenu);
					SetColor(0, 8);
					std::cout << "> " << options[posMenu];
					SetColor(7, 0);
					std::cout << " => <";
					SetColor(2, 0);
					std::cout << fps;
					SetColor(7, 0);
					std::cout << ">" << std::endl;
					break;

				case 2:
					duration++;
					gotoxy(0, posMenu);
					SetColor(0, 8);
					std::cout << "> " << options[posMenu];
					SetColor(7, 0);
					std::cout << " => <";
					SetColor(2, 0);
					std::cout << duration;
					SetColor(7, 0);
					std::cout << ">" << std::endl;
					break;
				}
				break;
			}
			}
		}
		else if (key == 27) {  // 27 es la tecla ESC
			std::cout << "Escape presionado. Saliendo..." << std::endl;
			exit = true;
		}
		else if (key == 13) {
			switch (posMenu) {
			case 0:
				break;
			case 3:
				CreateImages();
				posMenu = 0;
				PrintOptions(posMenu, options);
				break;
			case 4:
				CreateVideo();
				posMenu = 0;
				PrintOptions(posMenu, options);
				break;
			case 5:
				SetColor(7, 0);
				exit = true;
				break;
			}
		}
	}



}

int main()
{
	
	int posMenu = 0;
	vector<String> options = { "Seleccionar imagenes","Cambiar fps", "Cambiar duracion", "Crear imagenes", "Crear video", "Salir" };

	system("cls");
	menu(posMenu, options);
	std::cout << endl;




	return 0;

}
