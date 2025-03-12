#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <windows.h>

using namespace std;

struct BMPImage {
private:
    string fileName;
    vector<vector<bool>> imageData;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    int width;
    int height;
    int padding;
    int bytesCount;

    void read() {
        ifstream file(fileName, ios::binary);

        if (!file) {
            cout << "Ошибка открытия файла";
            return;
        }

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

        width = infoHeader.biWidth;
        height = abs(infoHeader.biHeight);
        bytesCount = infoHeader.biBitCount / 8;
        padding = (4 - (width * bytesCount) % 4) % 4;


        for(int i = 0; i < height; i++) {
            vector<bool> row;
            for (int j = 0; j < width; j++) {
                unsigned char pixel[bytesCount];
                file.read(reinterpret_cast<char*>(pixel), bytesCount);

                row.push_back((pixel[0] == 0));

            }
            imageData.push_back(row);
            file.ignore(padding);

        }
        file.close();
    }

public:
    BMPImage(string fileName){
        this->fileName = fileName;
        read();
    }

    void crossBMP(){
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

    }

    void printBMP(){

        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                cout << imageData[i][j];
            }
            cout << endl;
        }
    }

    void saveBMP(){
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
};

int main(){
    setlocale(LC_ALL, "Russian");

    string fileName;
    cout << "Введите имя файла: ";
    cin >> fileName;

    BMPImage image(fileName);
    image.printBMP();
    image.crossBMP();
    image.saveBMP();
}
