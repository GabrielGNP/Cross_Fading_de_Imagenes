

#include <iostream>
#include <cstdlib>
#include <conio.h>  // Para getch() en Windows

#include <vector>

#include <windows.h>
#include <string>
#include <cstring>
#include <vector>

#include <filesystem> // Para crear carpetas

#include <chrono> // Para medir el tiempo


// Para stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image_write.h"



using namespace std;
namespace fs = std::filesystem;


// Parámetros
int fps = 24; //imagenes por segundo
int duration = 4; // segundos de duración del video
string videoOutputFolderName = "Video_Output";
string imagesOutputFolderName = "Images_Output";
string imagesInputFolderName = "Images";
string nameImgInit = "test1_a(x800).jpg";
string nameImgEnd = "test1_b(x800).jpg";


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



//int GenerateImages(Mat imgInit, Mat imgEnd) {
int GenerateImages(unsigned char* imgInit, unsigned char* imgEnd, int widthInit, int heightInit, int channelsInit) {
	int frames = fps * duration; // cantidad de frames totales
	int imgSize = widthInit * heightInit * channelsInit;

	vector<unsigned char*> listImgs;
	system("cls");
	cout << "calculando imagenes"<<endl;
	gotoxy(0, 1);
	std::cout << "<";
	gotoxy(51, 1);
	std::cout << ">";
	gotoxy(24, 2);
	cout << int(0 * 100) << "%";
	auto start = std::chrono::high_resolution_clock::now(); // <========= toma el tiempo actual para el inicio del cronometro
	auto startG = std::chrono::high_resolution_clock::now(); // <========= toma el tiempo actual para el inicio del cronometro
	for (int i = 0; i < frames; ++i) {
		float P = static_cast<float>(i) / frames;

		unsigned char* imgResult = new unsigned char[imgSize];


		for (int j = 0; j < imgSize; j += channelsInit) {

			imgResult[j] = ((int)imgInit[j] * (1 - P) + (int)imgEnd[j] * P); // Canal rojo
			imgResult[j + 1] = ((int)imgInit[j + 1] * (1 - P) + (int)imgEnd[j + 1] * P); // Canal verde
			imgResult[j + 2] = ((int)imgInit[j + 2] * (1 - P) + (int)imgEnd[j + 2] * P); // Canal azul
		}
		listImgs.push_back(imgResult);
				

		if (int(P) % 2 == 0) {
			gotoxy(int(P * 50) + 1, 1);
			std::cout << "=";
		}
		gotoxy(24, 2);
		cout << int(P * 100) << "%";
		
	}
	cout << endl;
	gotoxy(24, 2);
	cout << "100%";
	cout << endl;
	auto endG = std::chrono::high_resolution_clock::now();// <========= toma el tiempo actual para el fin del cronometro
	auto durationms = std::chrono::duration_cast<std::chrono::milliseconds>(endG - startG); // <======== calcula el tiempo en función del final y el inicio
	auto durationsec = std::chrono::duration_cast<std::chrono::seconds>(endG - startG); // <======== calcula el tiempo en función del final y el inicio
	gotoxy(0, 3);
	cout << "Tiempo tardado en generar las imagenes: " << durationms.count() << "ms" << endl;
	gotoxy(0, 4);
	cout << "Tiempo tardado en generar las imagenes: " << durationsec.count() << "seg" << endl;
	gotoxy(0, 5);
	cout << "escribiendo imagenes" << endl;
	gotoxy(0, 6);
	cout << "<";
	for (int i = 0; i < listImgs.size(); i++) {
		string name = "images_Output/output_" + std::to_string(i) + ".png";
		stbi_write_png(name.c_str(), widthInit, heightInit, channelsInit, listImgs[i], widthInit * channelsInit);
		if (int(i) % 2 == 0) {
			std::cout << "=";
		}
	}
	cout << ">" << endl;
	auto end = std::chrono::high_resolution_clock::now();// <========= toma el tiempo actual para el fin del cronometro
	durationms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); // <======== calcula el tiempo en función del final y el inicio
	durationsec = std::chrono::duration_cast<std::chrono::seconds>(end - start); // <======== calcula el tiempo en función del final y el inicio
	cout << "Tiempo tardado en generar las imagenes: " << durationms.count() << "ms" << endl;
	cout << "Tiempo tardado en generar las imagenes: " << durationsec.count() << "seg" << endl;

	stbi_image_free(imgInit); // Libera la memoria cuando termines
	stbi_image_free(imgEnd);
	
	std::cout << std::endl;

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


	string nameImageInit = pathProgram + "/" + imagesInputFolderName + "/" + nameImgInit;
	string nameImageEnd = pathProgram + "/" + imagesInputFolderName + "/" + nameImgEnd;

	int widthInit = 0, heightInit = 0, channelsInit = 0;
	int widthEnd = 0, heightEnd = 0, channelsEnd = 0;
	unsigned char* imgInit = stbi_load(nameImageInit.c_str(), &widthInit, &heightInit, &channelsInit, 0);
	if (imgInit == nullptr) {
		cerr << "Error cargando la imagen inicial\n";
	}

	unsigned char* imgEnd = stbi_load(nameImageEnd.c_str(), &widthEnd, &heightEnd, &channelsEnd, 0);
	if (imgEnd == nullptr) {
		cerr << "Error cargando la imagen final\n";
	}

	if (GenerateImages(imgInit, imgEnd, widthInit, heightInit, channelsInit) == 0)
	{
		std::cout << "Imágenes creadas" << endl << endl;
		std::cout << "            Presione cualquier tecla para continuar..." << endl;
		_getch();
		system("cls");
	}

	else
		std::cout << "Hubo un error al crear las imágenes";
}

void PrintOptions(int posMenu, vector<string> options) {
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

void menu(int posMenu, vector<string> options) {
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
				/*CreateVideo();*/
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
	vector<string> options = { "Seleccionar imágenes","Cambiar fps", "Cambiar duración", "Crear imágenes", "Crear video (deshabilitado)", "Salir" };

	system("cls");
	menu(posMenu, options);
	system("cls");
	std::cout << endl;


	return 0;

}
