#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;

void displaySizes(char type)
{
    if(type == 'A')
        cout << "XXS   XS    S    M    L   XL  2XL  3XL";
    else if(type == 'C')
        cout << "  2    4    6    8   10   12   14     ";
    else if(type == 'L')
        cout << "                                      ";
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

void displayCatagory(string catagory,vector< vector<int> > & styles,vector<string> & styleNames,char type, int total){
    cout << catagory << setw(14-catagory.size()) << " ";
    displaySizes(type);
    cout << setw(10) << total << endl;
    for(int i = 0; i < styles.size(); i++){
        cout << setw(11) << styleNames[i] << " ";
        displayNumbers(styles[i]);
    }
    cout << endl;
}

void countFiles(vector<int> & numEachSize, char & type, int & total)
{ 
    string file;
    ifstream tempFile ("temp.txt");
    if (tempFile.is_open())
    {
        while ( tempFile >> file )
        {
            if(isalpha(file[0])){
                // need to seperate leggings and adult
                if(file.size() >= 4){ // just need to check before I access [3]
                    if(file[1] == '_'){ // and need to get rid of any weirdos
                        if(isalpha(file[2]) || file[3] == 'X'){
                            // is adult ex. "A_XXS" or "H_3XL"
                            numEachSize[file[0] - 'A']++;
                            total++;
                            type = 'A';
                        } else if (isdigit(file[2])){
                            // is leggings ex. "S_01"
                            numEachSize[0] += atoi(file.substr(2,2).c_str());
                            total++;
                            type = 'L';
                        }
                    }
                }
            } else if(isdigit(file[0])){
                // need to seperate leggings and kids
                int num = atoi(file.substr(0,2).c_str());
                if(file.size() < 3){
                    // is leggings
                    numEachSize[0] += num;
                    type = 'L';
                    total += num;
                } else {
                    if(file[2] == '_' && num%2 == 0){
                        // is kids
                        numEachSize[num/2-1]++;
                        type = 'C';
                        total++;
                    } else {
                        // is leggings
                        numEachSize[0] += num;
                        type = 'L';
                        total += num;
                    }
                }
            }
        }
        tempFile.close();
    }
    else {
        cout << "The system command to create temp.txt didn't work\n";
    }
}

int main(){
    string word,catagory;
    vector< vector<int> > styles;
    vector<string> styleNames;
    char type;
    int total = 0;
    int catagoryTotal = 0;
    ifstream foldersFile ("styles.txt");
    if (foldersFile.is_open())
    {
        while ( getline(foldersFile, word) )
        {
            if(word[0] == '-'){
                if(catagoryTotal)
                    displayCatagory(catagory,styles,styleNames,type,catagoryTotal);
                catagory = word.substr(1);
                styles.clear();
                styleNames.clear();
                total += catagoryTotal;
                catagoryTotal = 0;
            } else if (isalpha(word[0])) {
                if( access( ("..\\" + word).c_str(), F_OK ) != -1 ){
                    string command = "dir /b \"..\\"+word+"\" > temp.txt"; 
                    system(command.c_str());
                    styleNames.push_back(word);
                    vector<int> numEachSize(8,0);
                    countFiles(numEachSize,type,catagoryTotal);
                    styles.push_back(numEachSize);
                }
                else {
                    cout << "Couldn't find the \"" << word << "\" folder\n";
                }
            }
        }
        foldersFile.close();
        displayCatagory(catagory,styles,styleNames,type,catagoryTotal);
        cout << "Total: " << total+catagoryTotal << endl;
    } else {
        cout << "Couldn't find \"styles.txt\"\n";
    }
    return 0;
}
