#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <windows.h>

using namespace std;

void printBMP (const vector<vector<bool>>& A) {
    int row = A.size();
    int col = A[0].size();

    for (int i = row - 1; i >= 0; i--) {
        for (int j = 0; j < col; j++) {
            cout << A[i][j];
        }
        cout << endl;
    }
}

void BMP(string fileName) {

    // Чтение данных
    ifstream file(fileName, ios::binary);

    if (!file) {
        cout << "Ошибка открытия файла";
        return;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (fileHeader.bfType != 0x4D42) {
        cout << "Неверный формат файла";
        return;
    }

    if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
        cout << "Поддерживаются только 24 и 32 битные BMP";
        return;
    }

    int width = infoHeader.biWidth;
    int height = abs(infoHeader.biHeight);
    int bytesCount = infoHeader.biBitCount / 8;
    int padding = (4 - (width * bytesCount) % 4) % 4;

    vector<vector<bool>> imageData(height, vector<bool>(width));

    for(int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char pixel[bytesCount];
            file.read(reinterpret_cast<char*>(pixel), bytesCount);

            imageData[i][j] = (pixel[0] == 0) ? true : false;
        }
        file.ignore(padding);
    }
    file.close();

    //Вывод изображения
    system("cls");
    printBMP(imageData);

    //Модификация изображения
    double k = (double) (width - 1) / (height - 1);

    for(int i = 0; i < height - 1; i++) {

        int j = round(k * i);
        int jNext = round(k * (i + 1));

        imageData[i][j] = !imageData[i][j];
        imageData[i][width - 1 -j] = !imageData[i][width - 1 - j];

        for(int m = j + 1; m <= jNext; m ++){
            imageData[i][m] = !imageData[i][m];
            imageData[i][width - 1 - m] = !imageData[i][width - 1 - m];
        }
    }
    imageData[height - 1][width - 1] = !imageData[height - 1][width - 1];
    imageData[height - 1][0] = !imageData[height - 1][0];

    //Вывод модифицированного изображения
    cout << endl << endl;
    printBMP(imageData);

    //Сохранение файла
    string newFilename;
    cout << "Введите название файла (без расширения):";
    cin >> newFilename;
    newFilename += ".bmp";

    ofstream newFile(newFilename, ios::binary);
    if (!newFile) {
        cerr << "Ошибка создания нового файла!" << endl;
        return;
    }

    newFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    newFile.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    for(int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char pixel[bytesCount];
            if(imageData[i][j]) {
                pixel[0] = 0; pixel[1] = 0; pixel[2] = 0;}
            else{
                pixel[0] = 255; pixel[1] = 255; pixel[2] = 255;}

            newFile.write(reinterpret_cast<char*>(pixel), bytesCount);
        }
        vector<char> pad(padding, 0);
        newFile.write(pad.data(), padding);
    }
    newFile.close();

    cout << "Файл сохранен";
}


int main(){
    setlocale(LC_ALL, "Russian");

    string fileName;
    cout << "Введите имя файла: ";
    cin >> fileName;

    BMP(fileName);
}
