#include <bits/stdc++.h>
#include "Pass1.h"
#include "Pass2.h"
using namespace std;

int main()
{
    fileName = "input.txt";
    programLength = 0;

    //Filling Tables
    fill_optab();
    fill_regtab();

    //Performing Pass 1
    pass1();

    //Writing Symbol Table
    write_symtab();

    //Writing Literal Table
    write_littab();
    
    //Writing EXTDEF Table
    write_extdeftab();

    //Writing EXTREF Table
    write_extreftab();

    //Performing Pass 2
    pass2();
}