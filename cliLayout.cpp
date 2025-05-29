#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include <ctime>
#include <unordered_map>
#include "Console.h"
#include <cstdlib>
using namespace std;

std::unordered_map<std::string, Console> screens;


void SetCursorPosition(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int computeSpace(int width, string string1, string string2){
    return width - string1.length() - string2.length();
}

void drawLine(int innerWidth, int innerWidth2, string string1, string string2, string string3, string string4, string string5){
     cout << "|" << string1 << string(computeSpace(innerWidth, string1, string2), ' ') << string2 << "|" << string3 << string(computeSpace(innerWidth2, string3, string4), ' ') << string4 << "|" << string5 << string(innerWidth2 + 1 - string5.length(), ' ') << "|";
}

void DrawBox(int x, int y, int width, int height) {
    SetCursorPosition(x, y);
    cout << "+" << string(width - 2, '-') << "+";
    // header information
    string header1 = " NVIDIA-SMI 551.86";
    string header2 = "Driver Version: 551.86";
    string header3 = "CUDA Version 12.4 ";
    string gpuName = "GPU Name";
    string tccWddm = "TCC/WDDM ";
    string fanTempPerf = "Fan Temp  Perf";
    string pwrUsageCap = "Pwr: Usage/Cap";
    string busID = "Bus-Id";
    string dispA = "Disp.A";
    string memoryUsage = "Memory-Usage";
    string volatileUnc = " Volatile Uncorr. ECC";
    string gpuUtil = " GPU-Util Compute M.";
    string migM = " MIG M. ";

    string nvidiaGforce = "   0 NVIDIA GeForce GTX 1080";
    string wddm = "WDDM ";
    string busVal = "00000000:26:00.0";
    string dispVal = "On";
    string nA = "N/A";
    string fanTempPerfVal = "28%  37C    P8";
    string pwrUsageCapVal = "11W / 180W";
    string memoryUsageVal = "701MiB /   8192MiB";
    string gpuUtilCompute = "0%     Default";

    int spaceInBetween1;
    int spaceInBetween2;

    int innerWidth = width * 0.4 - 1, innerWidth2 = width * 0.3 - 1;

    for (int i = 1; i < height - 1; ++i) {
        SetCursorPosition(x, y + i);
        if(i == 2){
            cout << "|" << string(width - 2, '-')  << "|";
        } else if (i < 2) {
            int innerWidth = width - 2;
            int spaceL = (innerWidth - header1.length() - header2.length() - header3.length()) / 2;
            int spaceR = innerWidth - header1.length() - header2.length() - header3.length() - spaceL;
            cout << "|" << header1 << string(spaceL, ' ') << header2 << string(spaceR, ' ') << header3  << "|";
        } else if (i == 6){
            cout << "|" << string(width - 2, '=')  << "|";
        } else {
        if(i == 3){
            drawLine(innerWidth, innerWidth2, gpuName, tccWddm, busID, dispA, volatileUnc);
        } else if (i == 4) {
            drawLine(innerWidth, innerWidth2, fanTempPerf, pwrUsageCap, "", memoryUsage, gpuUtil);
        } else if (i == 5) {
            drawLine(innerWidth, innerWidth2, "", "", "", "", migM);
        } else if (i == 7) {
            drawLine(innerWidth, innerWidth2, nvidiaGforce, wddm, busVal, dispVal, nA);
        } else if (i == 8) {
            drawLine(innerWidth, innerWidth2, fanTempPerfVal, pwrUsageCap, "", memoryUsageVal, gpuUtilCompute);
        } else if (i == 9) {
            drawLine(innerWidth, innerWidth2, "", "", "", "", nA);
        }
        else{
            cout << "|" << string(width * 0.4 - 1, ' ') << "|" << string(width * 0.3 - 1, ' ') << "|" << string(width * 0.3, ' ') << "|";
        }
    }
    }
    SetCursorPosition(x, y + height - 1);
    cout << "+" << string(width - 2, '-') << "+";
}

void printProcesses(int x, int y, int width, int height, int i){
    int spacing = 3;
    
    for (const auto& pair : screens) {
        SetCursorPosition(x, y + i);
        srand(static_cast<unsigned int>(time(0))); // Seed the random number generator
        int randomNum = rand() % 9000 + 1000; // Generates a number between 1000 and 9999
        string line = "|" + string(spacing + 1, ' ') + '0' + string(spacing, ' ') + "N/A" +
              string(spacing - 1, ' ') + "N/A" + string(spacing + 1, ' ') +
              to_string(randomNum) + string(spacing, ' ') + "C+G" +
              string(spacing + 1, ' ') + pair.second.getName();
        cout << line << string(width - 10 - line.length(), ' ') << "N/A" << string(spacing * 2, ' ') << "|";
        ++i;
    }
    SetCursorPosition(x, y + screens.size() + height - 1);
    cout << "+" << string(width - 2, '-') << "+";
   
}
void DrawBox2(int x, int y, int width, int height) {
    SetCursorPosition(x, y);
    cout << "+" << string(width - 2, '-') << "+";
    // header information
    string processHeader = " Processes:";
    string leftHeader1 = "  GPU   GI   CI      PID   Type   Process name";
    string rightHeader1 = "GPU Memory ";
    string leftHeader2 = "         ID   ID";
    string rightHeader2 = "Usage ";
 
    int i;
    for (i = 1; i < height - 1; ++i) {
        SetCursorPosition(x, y + i);

        if (i == 1){
            cout << "|" << processHeader << string(width - 2 - processHeader.length(), ' ') << "|";
        } else if (i == 2){
            cout << "|" << leftHeader1 << string(width - 2 - leftHeader1.length() - rightHeader1.length(), ' ') << rightHeader1 << "|";
        } else if(i == 3){
            cout << "|" << leftHeader2 << string(width - 2 - leftHeader2.length() - rightHeader2.length(), ' ') << rightHeader2 << "|";
        } else if (i == 4){
            cout << "|" << string(width - 2, '=')  << "|";
        } 
    }

    printProcesses(x, y, width, height, i);

}


int main() {
    system("cls");
    for(int i = 0; i < 5; i++){
        string screenName = "screen" +  to_string(i);
        Console temp(screenName);
        screens.insert({ screenName, temp });
        cout << screens[screenName].getName();
    }
    DrawBox(1, 5, 90, 11);
    DrawBox2(1, 16, 90, 6);

    SetCursorPosition(7, 4);
    SetCursorPosition(0, 0);

    return 0;
}
