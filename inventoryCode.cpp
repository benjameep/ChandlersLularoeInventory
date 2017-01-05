#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
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

void displayNumbers(vector<int> & counts,int price)
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
    cout << setw(10) << total;
	if(price != 0)
		cout << "\t$" << total*price << "\t($" << price << ")";
	cout << endl;
}

void displayCatagory(string catagory,vector< vector<int> > & styles,vector<string> & styleNames,vector<int> & stylePrices,char type, int totalItems,int totalValue){
    cout << catagory << setw(14-catagory.size()) << " ";
    displaySizes(type);
    cout << setw(10) << totalItems;
	if(totalValue)
		cout << "\t$" << totalValue;
	cout << endl;
    for(int i = 0; i < styles.size(); i++){
        cout << setw(11) << styleNames[i] << " ";
        displayNumbers(styles[i],stylePrices[i]);
    }
    cout << endl;
}

void countFiles(vector<int> & numEachSize, char & type, int & totalItems,int price,int & totalValue)
{ 
    string file;
    ifstream tempFile ("temp.txt");
    if (tempFile.is_open())
    {
		int initTotal = totalItems;
        while ( tempFile >> file )
        {
            if(isalpha(file[0])){
                // need to seperate leggings and adult
                if(file.size() >= 4){ // just need to check before I access [3]
                    if(file[1] == '_'){ // and need to get rid of any weirdos
                        if(isalpha(file[2]) || file[3] == 'X'){
                            // is adult ex. "A_XXS" or "H_3XL"
                            numEachSize[file[0] - 'A']++;
                            totalItems++;
                            type = 'A';
                        } else if (isdigit(file[2])){
                            // is leggings ex. "S_01"
                            int num = atoi(file.substr(2,2).c_str());
							numEachSize[0] += num;
                            totalItems += num;
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
                    totalItems += num;
                } else {
                    if(file[2] == '_' && num%2 == 0){
                        // is kids
                        numEachSize[num/2-1]++;
                        type = 'C';
                        totalItems++;
                    } else {
                        // is leggings
                        numEachSize[0] += num;
                        type = 'L';
                        totalItems += num;
                    }
                }
            }
        }
		totalValue += (totalItems-initTotal)*price;
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
	vector<int> stylePrices;
    char type;
    int totalItems, catagoryTotal, catagoryValue, totalValue;
	totalItems = catagoryTotal = catagoryValue = totalValue = 0;
    ifstream foldersFile ("styles.txt");
    if (foldersFile.is_open())
    {
        while ( getline(foldersFile, word) )
        {
            if(word[0] == '-'){
                if(catagoryTotal)
                    displayCatagory(catagory,styles,styleNames,stylePrices,type,catagoryTotal,catagoryValue);
                catagory = word.substr(1);
                styles.clear();
                styleNames.clear();
				stylePrices.clear();
                totalItems += catagoryTotal;
				totalValue += catagoryValue;
				catagoryTotal = catagoryValue = 0;
            } else if (isalpha(word[0])) {
                string price;
                stringstream ss(word);
                ss >> word >> price;
				if(price[0] && price[0] != '$'){
					word += ' '+price;
					ss >> price;
				}
                if( access( ("..\\" + word).c_str(), F_OK ) != -1 ){
                    string command = "dir /b \"..\\"+word+"\" > temp.txt"; 
                    system(command.c_str());
                    styleNames.push_back(word);
					if(price[0] == '$')
	                	stylePrices.push_back(atoi(price.substr(1).c_str()));
					else
						stylePrices.push_back(0);
					vector<int> numEachSize(8,0);
                    countFiles(numEachSize,type,catagoryTotal,stylePrices.back(),catagoryValue);
                    styles.push_back(numEachSize);
                }
                else {
                    cout << "Couldn't find the \"" << word << "\" folder\n";
                }
            }
        }
        foldersFile.close();
        displayCatagory(catagory,styles,styleNames,stylePrices,type,catagoryTotal,catagoryValue);
        cout << "Total: " << totalItems+catagoryTotal << " $" << totalValue+catagoryValue << endl;
    } else {
        cout << "Couldn't find \"styles.txt\"\n";
    }
    return 0;
}
