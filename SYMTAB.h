#include <bits/stdc++.h>
using namespace std;

struct info_symtab
{
    string address;
    char type;
};

map<string, info_symtab> SYMTAB;

void write_symtab()
{
    ofstream symtabFile;
    symtabFile.open("symtab_file.txt");
    if (!symtabFile)
    {
        cout << "Unable to open file: symtab_file" << endl;
        exit(1);
    }

    symtabFile << setw(10) << "Symbol" << setw(10) << "Address" << setw(10) << "Type" << endl;
    symtabFile << "==============================" << endl;
    for (auto &it : SYMTAB)
    {
        if (it.first != "")
            symtabFile << setw(10) << it.first << setw(10) << it.second.address << setw(10) << it.second.type << endl;
    }

    symtabFile.close();
}