#include <windows.h>
#include <iostream>
#include <string>
#include <map>
using namespace std;
/*
SetConsoleCursorPosition() – move cursor

SetConsoleTextAttribute() – set text color

FillConsoleOutputCharacter() – draw borders

WriteConsoleOutputCharacter() – write strings at positions

GetStdHandle(STD_OUTPUT_HANDLE) – get console handle
*/
void SetCursorPosition(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
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
    string migM = "MIG M. ";

    string nvidiaGforce = "   0 NVIDIA GeForce GTX 1080";
    string wddm = "WDDM ";
    string busVal = "00000000:26:00.0";
    string dispVal = "On";
    string nA = "N/A";
    string fanTempPerfVal = "28%  37C    P8";
    string pwrUsageCapVal = "11W / 180W";
    string memoryUsageVal = "701MiB /   8192MiB";


    std::map<string, string> headers = {
        {"header1", " NVIDIA-SMI 551.86"},
        {"header2", "Driver Version: 551.86"},
        {"header3", "CUDA Version 12.4 "},
        {"gpuName", "GPU Name"},
        {"tccWddm", "TCC/WDDM "},
        {"fanTempPerf", "Fan Temp  Perf"},
        {"pwrUsageCap", "Pwr: Usage/Cap"},
        {"busID", "Bus-Id"},
        {"dispA", "Disp.A"},
        {"memoryUsage", "Memory-Usage"},
        {"volatileUnc", " Volatile Uncorr. ECC"},
        {"gpuUtil", " GPU-Util Compute M."},
        {"migM", "MIG M. "}
    };

    for (int i = 1; i < height - 1; ++i) {
        SetCursorPosition(x, y + i);
        if(i == height/4){
            cout << "|" << string(width - 2, '-')  << "|";
        } else if (i < height/4) {
            int innerWidth = width - 2;
            int leftLen = header1.length();
            int centerLen = header2.length();
            int rightLen = header3.length();

            int spaceBetweenLeftAndCenter = (innerWidth - leftLen - centerLen - rightLen) / 2;
            int spaceBetweenCenterAndRight = innerWidth - leftLen - centerLen - rightLen - spaceBetweenLeftAndCenter;

            cout << "|"
                      << header1
                      << string(spaceBetweenLeftAndCenter, ' ')
                      << header2
                      << string(spaceBetweenCenterAndRight, ' ')
                      << header3
                      << "|";
        } else if (i == height/2 + 1){
            cout << "|" << string(width - 2, '=')  << "|";
        } else {
        if(i == 3){
            int innerWidth = width * 0.4 - 1;
            int innerWidth2= width * 0.3 - 1;
            int leftLen1 = gpuName.length();
            int rightLen1  = tccWddm.length();
            int spaceInBetween1 = innerWidth - leftLen1 - rightLen1;
            int leftLen2 = busID.length();
            int rightLen2  = dispA.length();
            int spaceInBetween2 = innerWidth2 - leftLen2 - rightLen2;
            cout << "|" << gpuName << string(spaceInBetween1, ' ') << tccWddm << "|" << busID << string(spaceInBetween2, ' ') << dispA << "|" << volatileUnc << string(innerWidth2 + 1 - volatileUnc.length(), ' ') << "|";
        } else if (i == 4){
            int innerWidth = width * 0.4 - 1;
            int innerWidth2= width * 0.3 - 1;
            int leftLen1 = fanTempPerf.length();
            int rightLen1  = pwrUsageCap.length();
            int spaceInBetween1 = innerWidth - leftLen1 - rightLen1;
            int len2 = memoryUsage.length();
            int spaceInBetween2 = innerWidth2 - len2;
            cout << "|" << fanTempPerf << string(spaceInBetween1, ' ') << pwrUsageCap << "|" << string(spaceInBetween2, ' ') << memoryUsage << "|" << gpuUtil << string(innerWidth2 + 1 - gpuUtil.length(), ' ') << "|";
        } else if (i == 5) {
            cout << "|" << string(width * 0.4 - 1, ' ') << "|" << string(width * 0.3 - 1, ' ') << "|" << string(width * 0.3 - 1 - migM.length(), ' ') << migM  << "|";
        }
        else{
            cout << "|" << string(width * 0.4 - 1, ' ') << "|" << string(width * 0.3 - 1, ' ') << "|" << string(width * 0.3, ' ') << "|";
        }
    }

    SetCursorPosition(x, y + height - 1);
    cout << "+" << string(width - 2, '-') << "+";
    }
}

void DrawBox2(int x, int y, int width, int height) {
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
    string migM = "MIG M. ";

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
            spaceInBetween1 = innerWidth - gpuName.length() - tccWddm.length();
            spaceInBetween2 = innerWidth2 - busID.length() - dispA.length();
            cout << "|" << gpuName << string(spaceInBetween1, ' ') << tccWddm << "|" << busID << string(spaceInBetween2, ' ') << dispA << "|" << volatileUnc << string(innerWidth2 + 1 - volatileUnc.length(), ' ') << "|";
        } else if (i == 4){
            spaceInBetween1 = innerWidth - fanTempPerf.length() - pwrUsageCap.length();
            spaceInBetween2 = innerWidth2 - memoryUsage.length();
            cout << "|" << fanTempPerf << string(spaceInBetween1, ' ') << pwrUsageCap << "|" << string(spaceInBetween2, ' ') << memoryUsage << "|" << gpuUtil << string(innerWidth2 + 1 - gpuUtil.length(), ' ') << "|";
        } else if (i == 5) {
            cout << "|" << string(width * 0.4 - 1, ' ') << "|" << string(width * 0.3 - 1, ' ') << "|" << string(width * 0.3 - 1 - migM.length(), ' ') << migM  << "|";
        } else if (i == 7) {
            spaceInBetween1 = innerWidth - nvidiaGforce.length(); - wddm.length();
            spaceInBetween2 = innerWidth2 - busVal.length(); - dispVal.length();
            cout << "|" << nvidiaGforce << string(spaceInBetween1, ' ') << wddm << "|"  << busVal << string(spaceInBetween2, ' ') << dispVal << "|" << string(innerWidth2 + 1 - gpuUtil.length(), ' ') << nA << "|";
        }
        else{
            cout << "|" << string(width * 0.4 - 1, ' ') << "|" << string(width * 0.3 - 1, ' ') << "|" << string(width * 0.3, ' ') << "|";
        }
    }

    SetCursorPosition(x, y + height - 1);
    cout << "+" << string(width - 2, '-') << "+";
    }
}


int main() {
    system("cls");
    DrawBox(1, 5, 80, 11);

    SetCursorPosition(7, 4);
    SetCursorPosition(0, 0);

    return 0;
}
