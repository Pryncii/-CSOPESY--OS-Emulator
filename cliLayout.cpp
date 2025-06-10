#include <iostream>
#include <string>
#include <ctime>
#include <unordered_map>
#include "Console.h"
#include <cstdlib>

#define WIDTH 90
#define HEIGHT 11
#define HEIGHT2 6
using namespace std;

std::unordered_map<std::string, Console> screens;

int computeSpace(int width, string string1, string string2){
    return width - string1.length() - string2.length();
}

void drawLine(int innerWidth, int innerWidth2, string string1, string string2, string string3, string string4, string string5){
    cout << "|" << string1 << string(computeSpace(innerWidth, string1, string2), ' ') << string2 << "|" <<
    string3 << string(computeSpace(innerWidth2, string3, string4), ' ') << string4 << "|"  << string(innerWidth2 - string5.length(), ' ') << string5 << "|";
}

void DrawBox(int width, int height) {
 
    // header information
    string header1 = " NVIDIA-SMI 551.86";
    string header2 = "Driver Version: 551.86";
    string header3 = "CUDA Version 12.4   ";

    string gpuName = " GPU Name";
    string tccWddm = "TCC/WDDM  ";
    string fanTempPerf = " Fan Temp  Perf";
    string pwrUsageCap = "Pwr: Usage/Cap ";
    string busID = " Bus-Id";
    string dispA = "Disp.A ";
    string memoryUsage = "Memory-Usage ";
    string volatileUnc = " Volatile Uncorr. ECC ";
    string gpuUtil = "   GPU-Util Compute M. ";
    string migM = " MIG M. ";

    string nvidiaGforce = "   0 NVIDIA GeForce GTX 1080";
    string wddm = "WDDM  ";
    string busVal = "  00000000:26:00.0";
    string dispVal = "On ";
    string nA = "N/A ";
    string fanTempPerfVal = " 28%  37C    P8";
    string pwrUsageCapVal = "11W / 180W ";
    string memoryUsageVal = "701MiB /   8192MiB ";
    string gpuUtilCompute = "0%     Default ";

    int innerWidth = width * 0.4 + 4, innerWidth2 = width * 0.3 - 3;
    
    // Get and display the current time
    time_t now = time(0);                   
    char buffer[80];                        
    struct tm * timeinfo = localtime(&now); 
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", timeinfo);
    cout << buffer << "\n";
    
    cout << "+" << string(width - 2, '-') << "+\n";

    // Draw the header and the rest of the box
    for (int i = 1; i < height - 1; ++i) {
        
        if(i == 2){
            cout << "|" << string(innerWidth, '-') << "+" << string(innerWidth2, '-') << "+" << string(innerWidth2, '-') << "|";
            } else if (i < 2) {
                int innerWidth = width - 2;
                int spaceL = (innerWidth - header1.length() - header2.length() - header3.length()) / 2;
                int spaceR = innerWidth - header1.length() - header2.length() - header3.length() - spaceL;
                cout << "|" << header1 << string(spaceL, ' ') << header2 << string(spaceR, ' ') << header3  << "|";
            } else if (i == 6){
                cout << "|" << string(innerWidth, '=') << "+" << string(innerWidth2, '=') << "+" << string(innerWidth2, '=') << "|";
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
                drawLine(innerWidth, innerWidth2, fanTempPerfVal, pwrUsageCapVal, "", memoryUsageVal, gpuUtilCompute);
            } else if (i == 9) {
                drawLine(innerWidth, innerWidth2, "", "", "", "", nA);
            } 
        }
        cout << "\n";
    }
    cout << "+" << string(width - 2, '-') << "+";
    cout << "\n";
}

void printProcesses(int width, int height){
    int spacing = 3;
    srand(static_cast<unsigned int>(time(0)));
    
    // Print the needed data for the processes
    for (const auto& pair : screens) {
        int randomNum = rand() % 9000 + 1000; // Generates a number between 1000 and 9999
        string displayName = pair.second.getName();
        if (displayName.length() > 39){
            displayName = "..." + displayName.substr(displayName.length() - 36);
        }
        string line = "|" + string(spacing + 1, ' ') + '0' + string(spacing, ' ') + "N/A" +
              string(spacing - 1, ' ') + "N/A" + string(spacing + 1, ' ') +
              to_string(randomNum) + string(spacing + 1, ' ') + "C+G" +
              string(spacing + 1, ' ') + displayName;
        cout << line << string(width - 10 - line.length(), ' ') << "N/A" << string(spacing * 2, ' ') << "|\n";
    }
    cout << "+" << string(width - 2, '-') << "+";
   
}
void DrawBox2(int width, int height) {
    // header information
    string processHeader = " Processes:";
    string leftHeader1 = "  GPU   GI   CI      PID   Type   Process name";
    string rightHeader1 = "GPU Memory ";
    string leftHeader2 = "        ID   ID";
    string rightHeader2 = "Usage      ";
 
    int i;
    cout << "+" << string(width - 2, '-') << "+\n";

    // Draw the header and the rest of the box without the processes
    for (i = 1; i < height - 1; ++i) {

        if (i == 1){
            cout << "|" << processHeader << string(width - 2 - processHeader.length(), ' ') << "|";
        } else if (i == 2){
            cout << "|" << leftHeader1 << string(width - 2 - leftHeader1.length() - rightHeader1.length(), ' ') << rightHeader1 << "|";
        } else if(i == 3){
            cout << "|" << leftHeader2 << string(width - 2 - leftHeader2.length() - rightHeader2.length(), ' ') << rightHeader2 << "|";
        } else if (i == 4){
            cout << "|" << string(width - 2, '=')  << "|";
        }
        cout << "\n";
    }

    printProcesses(width, height);
}

int main() {
    string screenName = "";

    // Initialize the screens with some dummy data
    for(int i = 0; i < 5; i++){
        if(i % 2 == 0){
            screenName = "C:\\Users\\Prince Buencamino\\Desktop\\Acads\\3rd Year T3\\[CSOPESY]-OS-Emulator" + to_string(i);
        } else {
            screenName = "C:\\Users\\Sophia\\Desktop\\Acads\\" + to_string(i);
        }
        Console temp(screenName);
        screens.insert({ screenName, temp });
    }
    DrawBox(WIDTH, HEIGHT);
    cout << "\n";
    DrawBox2(WIDTH, HEIGHT2);

    return 0;
}
