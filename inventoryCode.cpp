#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;

void displaySizes(bool adult = true)
{
    if(adult)
        cout << "XXS   XS    S    M    L   XL  2XL  3XL";
    else
        cout << "  2    4    6    8   10   12   14     ";
}

void displayNumbers(vector<int> & counts)
{
    int total = 0;
    for(int i = 0; i < counts.size(); i++){
        total += counts[i];
        cout << setw(5);
        if(counts[i])
            cout << counts[i];
        else
            cout << " ";
    }
    cout << setw(10) << total << endl;
}

void displayCatagory(string type,vector< vector<int> > & styles,vector<string> & styleNames,bool isAdult, int total){
    cout << type << setw(14-type.size()) << " ";
    displaySizes(isAdult);
    cout << setw(10) << total << endl;
    for(int i = 0; i < styles.size(); i++){
        cout << setw(11) << styleNames[i] << " ";
        displayNumbers(styles[i]);
    }
    cout << endl;
}

void countFiles(vector<int> & numEachSize, bool & isAdult, int & total)
{ 
    string file;
    ifstream tempFile ("temp.txt");
    if (tempFile.is_open())
    {
        while ( tempFile >> file )
        {
            string size = file.substr(0,file.find('_'));
            int kids = (size.size() == 2 ? 10+size[1]-'0' : size[0]-'0');
            if(size[0]-'A' >= 0 && size[0]-'A' < numEachSize.size()){
                numEachSize[size[0]-'A']++;
                total++;
            } else if(kids%2==0 && kids > 0 && kids <=14){
                isAdult = false;
                numEachSize[kids/2-1]++;
                total++;
            }
        }
        tempFile.close();
    }
    else {
        cout << "The system command to create temp.txt didn't work\n";
    }
}

int main(){
    string word,type;
    vector< vector<int> > styles;
    vector<string> styleNames;
    bool isAdult = true;
    int total = 0;
    int catagoryTotal = 0;
    ifstream foldersFile ("styles.txt");
    if (foldersFile.is_open())
    {
        while ( getline(foldersFile, word) )
        {
            if(word[0] == '-'){
                if(catagoryTotal)
                    displayCatagory(type,styles,styleNames,isAdult,catagoryTotal);
                type = word.substr(1);
                styles.clear();
                styleNames.clear();
                isAdult = true;
                total += catagoryTotal;
                catagoryTotal = 0;
            }
            else {
                if( access( word.c_str(), F_OK ) != -1 ){
                    string command = "dir /b \""+word+"\" > temp.txt";
                    system(command.c_str());
                    styleNames.push_back(word);
                    vector<int> numEachSize(8,0);
                    countFiles(numEachSize,isAdult,catagoryTotal);
                    styles.push_back(numEachSize);
                }
                else {
                    cout << "Couldn't find the \"" << word << "\" folder\n";
                }
            }
        }
        foldersFile.close();
        displayCatagory(type,styles,styleNames,isAdult,catagoryTotal);
        cout << "Total: " << total+catagoryTotal << endl;
    } else {
        cout << "Couldn't find \"styles.txt\"\n";
    }
    return 0;
}
