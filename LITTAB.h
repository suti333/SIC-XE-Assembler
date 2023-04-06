#include <bits/stdc++.h>
using namespace std;

map<string, string> LITTAB;

void write_littab()
{
    ofstream littabFile;
    littabFile.open("littab_file.txt");
    if (!littabFile)
    {
        cout << "Unable to open file: littab_file" << endl;
        exit(1);
    }

    littabFile << setw(10) << "Symbol" << setw(10) << "Address" << endl;
    littabFile << "====================" << endl;
    for (auto &it : LITTAB)
    {
        if (it.first != "")
            littabFile << setw(10) << it.first << setw(10) << it.second << endl;
    }

    littabFile.close();
}