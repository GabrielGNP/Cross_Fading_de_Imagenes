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
string videoOutputFolderName = "Video_Output";
string imagesOutputFolderName = "Images_Output";
string imagesInputFolderName = "Images";
string nameImgInit = "test1_a.jpg";
string nameImgEnd = "test1_b.jpg";


//std::string outputDir = "output_frames"; // directorio de salida

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
	gotoxy(0, 1);
	std::cout << "<";
	gotoxy(51, 1);
	std::cout << ">";
	gotoxy(24, 2);
	cout << "%" << int(0 * 100);
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

		std::string fileName = imagesOutputFolderName+"/frame_" + std::to_string(i) + ".jpg";
		imwrite(fileName, result);
		if (int(P) % 2 == 0) {
			gotoxy(int(P * 50) + 1, 1);
			std::cout << "=";
		}
		gotoxy(24, 2);
		cout << "%" << int(P * 100);

	}
	std::cout << std::endl;

	return 0;
}

int GeneratedVideo(Mat imgInit, Mat imgEnd) {
	int frames = fps * duration; // cantidad de frames totales
	Size frameSize = imgInit.size();

	// Inicializar el VideoWriter
	VideoWriter video(videoOutputFolderName+"/output_video.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frameSize);

	if (!video.isOpened()) {
		std::cerr << "Error al abrir el archivo de video" << std::endl;
		return -1;
	}

	system("cls");
	gotoxy(0, 1);
	std::cout << "<";
	gotoxy(51, 1);
	std::cout << ">";
	gotoxy(24, 2);
	cout << "%" << int(0 * 100);
	for (int i = 0; i <= frames; ++i) {
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
		
		if (int(P) % 2 == 0) {
			gotoxy(int(P * 50) + 1, 1);
			std::cout << "=";
		}
		gotoxy(24, 2);
		cout<< "%" << int(P * 100);
	}
	std::cout << std::endl;
	
	// Liberar el archivo de video
	video.release();

	return 0;
}

string SelectImage(string title, vector<string> files) {
	int op = 0;

	SetColor(7, 0);
	for (int num = 0; num < files.size(); num++) {
		gotoxy(50, num + 1);
		if (op == num) {
			SetColor(0, 8);
			std::cout << "> " << files[num] << std::endl;
			SetColor(7, 0);
		}else std::cout << "  " << files[num] << std::endl;
		
	}
	SetColor(7, 0);
	bool exitBucle = false;
	while (!exitBucle) {
		int key = _getch();  // Captura el primer carácter

		if (key == 224) {  // Si es una secuencia de tecla especial (teclas de dirección)
			key = _getch();

			switch (key) {
			case 72:  // Flecha arriba
				if (op > 0)
				{
					SetColor(7, 0);
					gotoxy(50, op + 1);
					std::cout << "  " << files[op] << endl;
					op--;
					gotoxy(50, op + 1);
					SetColor(0, 8);
					std::cout << "> " << files[op];
					SetColor(7, 0);
				}
				else {
					SetColor(7, 0);
					gotoxy(50, op + 1);
					std::cout << "  " << files[0];
					op = files.size() - 1;
					SetColor(0, 8);
					gotoxy(50, op + 1);
					std::cout << "> " << files[files.size() - 1];
				}
				break;
			case 80:  // Flecha abajo
				if (op < files.size() - 1) {
					op++;
					gotoxy(50, op + 1);
					SetColor(0, 8);
					std::cout << "> " << files[op];
					SetColor(7, 0);
					gotoxy(50, op);
					SetColor(7, 0);
					std::cout << "  " << files[op - 1];

				}
				else {
					SetColor(7, 0);
					gotoxy(50, op + 1);
					std::cout << "  " << files[files.size() - 1];
					op = 0;
					SetColor(0, 8);
					gotoxy(50, op + 1);
					std::cout << "> " << files[0];
					op = 0;
				}
				break;
			}
		}
		else if (key == 27) {  // 27 es la tecla ESC
			exitBucle = true;
			return "";
		}
		else if (key == 13) {
			exitBucle = true;
			return files[op];
		}
	}
	
}

void printMenuSelectedImages(vector<string> files, vector<string> options, int op) {
	SetColor(8, 0);
	for (int num = 0; num < files.size(); num++) {
		gotoxy(50, num + 1);
		std::cout << "  " << files[num] << std::endl;
	}
	SetColor(7, 0);
	gotoxy(0, 1);
	for (int i = 0; i < options.size(); i++) {
		if (op == i)
		{
			SetColor(0, 8);
			std::cout << "> " << options[i];
			SetColor(7, 0);
			if (i == 0) {
				SetColor(8, 0);
				std::cout << ": " << nameImgInit << endl;
			}
			else if (i == 1) {
				SetColor(8, 0);
				std::cout << ": " << nameImgEnd << endl;
			}
		}
		else
		{
			SetColor(7, 0);
			std::cout << "  " << options[i];
			if (i == 0) {
				SetColor(8, 0);
				std::cout << ": " << nameImgInit << endl;
			}
			else if (i == 1) {
				SetColor(8, 0);
				std::cout << ": " << nameImgEnd << endl;
			}
		}
		SetColor(7, 0);
	}
}

int MenuSelectImages() {
	vector<string> files;
	vector<string> options = {"imagen inicial", "imagen final", "salir"};
	int op = 0;
	try {
		system("cls");
		for (const auto& entry : fs::directory_iterator("./"+ imagesInputFolderName)) {
			if (fs::is_regular_file(entry.status())) { // Filtra solo archivos regulares
				files.push_back(entry.path().filename().string());
			}
		}
		printMenuSelectedImages(files, options, op);
		
		bool exit = false;
		while (!exit) {
			int key = _getch();  // Captura el primer carácter

			if (key == 224) {  // Si es una secuencia de tecla especial (teclas de dirección)
				key = _getch();

				switch (key) {
				case 72:  // Flecha arriba
					if (op > 0)
					{
						SetColor(7, 0);
						gotoxy(0, op + 1);
						std::cout << "  " << options[op]<<endl;
						op--;
						gotoxy(0, op+1);
						SetColor(0, 8);
						std::cout << "> " << options[op];
						SetColor(7, 0);	
					}
					else {
						SetColor(7, 0);
						gotoxy(0, op + 1);
						std::cout << "  " << options[0];
						op = options.size() - 1;
						SetColor(0, 8);
						gotoxy(0, op+1);
						std::cout << "> " << options[options.size() - 1];
					}
					break;
				case 80:  // Flecha abajo
					if (op < options.size() - 1) {
						op++;
						gotoxy(0, op + 1);
						SetColor(0, 8);
						std::cout << "> " << options[op];
						SetColor(7, 0);
						gotoxy(0, op);
						SetColor(7, 0);
						std::cout << "  " << options[op - 1];
						
					}
					else {
						SetColor(7, 0);
						gotoxy(0, op + 1);
						std::cout << "  " << options[options.size() - 1];
						op = 0;
						SetColor(0, 8);
						gotoxy(0, op + 1);
						std::cout << "> " << options[0];
						op = 0;
					}
					break;
				}
			}
			else if (key == 27) {  // 27 es la tecla ESC
				std::cout << "Escape presionado. Saliendo..." << std::endl;
				exit = true;
			}
			else if (key == 13) {
				switch (op) {
				case 0:
					nameImgInit = SelectImage("Imagen Inicial", files);
					printMenuSelectedImages(files, options, op);
					break;
				case 1:
					nameImgEnd = SelectImage("Imagen Final", files);
					printMenuSelectedImages(files, options, op);
					break;
				case 2:
					SetColor(7, 0);
					exit = true;
					break;
				}
			}
		}
	}
	catch (const fs::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return -1;
	}
	return 0;
}

void CreateImages() {
	CreateFolder(imagesOutputFolderName);

	Mat imgInit = imread(pathProgram + "/" + imagesInputFolderName + "/" + nameImgInit);
	Mat imgEnd = imread(pathProgram + "/" + imagesInputFolderName + "/" + nameImgEnd);

	if (GenerateImages(imgInit, imgEnd) == 0)
	{
		std::cout << "Imágenes creadas" << endl << endl;
		std::cout << "            Presione cualquier tecla para continuar..." << endl;
		_getch();
		system("cls");
	}

	else
		std::cout << "Hubo un error al crear las imágenes";
}

void CreateVideo() {
	int stop;
	CreateFolder(videoOutputFolderName);

	Mat imgInit = imread(pathProgram + "/" + imagesInputFolderName + "/" + nameImgInit);
	Mat imgEnd = imread(pathProgram + "/" + imagesInputFolderName + "/" + nameImgEnd);

	system("cls");
	if (GeneratedVideo(imgInit, imgEnd) == 0)
	{
		std::cout << "Video creado" << endl << endl;
		std::cout << "            Presione cualquier tecla para continuar..." << endl;
		_getch();
		system("cls");
	}		
	else
		std::cout << "no se pudo crear el video";
}

void PrintOptions(int posMenu, vector<String> options) {
	system("cls");
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
	cout << endl << endl;
	SetColor(8, 0);
	cout << "Imagen Inicial: "; SetColor(7, 0); cout << nameImgInit << endl;
	SetColor(8, 0);
	cout << "Imagen Final: "; SetColor(7, 0); cout << nameImgEnd << endl;
	SetColor(8, 0);
	cout << endl;
	
	std::cout << "↑ ↓ para moverse entre opciones"<<endl;
	std::cout << "→ ← para cambiar parámetros" << endl;
	std::cout << "'Enter' para seleccionar opción" << endl;
	std::cout << "'Esc' u opción Salir para salir del programa" << endl;
	SetColor(7, 0);
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
				MenuSelectImages();
				PrintOptions(posMenu, options);
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
	SetConsoleOutputCP(CP_UTF8);
	int posMenu = 0;
	vector<String> options = { "Seleccionar imágenes","Cambiar fps", "Cambiar duración", "Crear imágenes", "Crear video", "Salir" };

	system("cls");
	menu(posMenu, options);
	system("cls");
	std::cout << endl;


	return 0;

}
