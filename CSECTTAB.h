#include <bits/stdc++.h>
using namespace std;

struct info_csecttab
{
    string LOCCTR;
    int sect_num;
    int length;
    map<string, string> EXTDEFTAB;
    map<string, string> EXTREFTAB;
};

map<string, info_csecttab> CSECTTAB;

void write_extdeftab()
{
    ofstream extdeftabFile;
    extdeftabFile.open("extdeftab_file.txt");
    if (!extdeftabFile)
    {
        cout << "Unable to open file: extdeftab_file" << endl;
        exit(1);
    }

    extdeftabFile << setw(10) << "Section" << setw(10) << "Symbol" << setw(10) << "Address" << endl;
    extdeftabFile << "==============================" << endl;
    for (auto &it : CSECTTAB)
    {
        if (it.first != "")
        {
            for (auto &it1 : it.second.EXTDEFTAB)
            {
                if (it1.first != "")
                    extdeftabFile << setw(10) << it.first << setw(10) << it1.first << setw(10) << it1.second << endl;
            }
        }
    }

    extdeftabFile.close();
}

void write_extreftab()
{
    ofstream extreftabFile;
    extreftabFile.open("extreftab_file.txt");
    if (!extreftabFile)
    {
        cout << "Unable to open file: extreftab_file" << endl;
        exit(1);
    }

    extreftabFile << setw(10) << "Section" << setw(10) << "Symbol" << setw(10) << "Address" << endl;
    extreftabFile << "==============================" << endl;
    for (auto &it : CSECTTAB)
    {
        if (it.first != "")
        {
            for (auto &it1 : it.second.EXTREFTAB)
            {
                if (it1.first != "")
                    extreftabFile << setw(10) << it.first << setw(10) << it1.first << setw(10) << it1.second << endl;
            }
        }
    }

    extreftabFile.close();
}