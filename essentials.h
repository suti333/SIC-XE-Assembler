#ifndef ESSENTIALS_H
#define ESSENTIALS_H

#include <bits/stdc++.h>
#include "OPTAB.h"
#include "SYMTAB.h"
#include "REGTAB.h"

using namespace std;

void processFileLine()
{
}

bool checkCommentLine(string fileLine)
{
    if (fileLine[0] == '.')
    {
        return true;
    }
    return false;
}

bool checkLabelExistsInSYMTAB(string LABEL)
{
    if (SYMTAB.find(LABEL) != SYMTAB.end())
    {
        return true;
    }
    return false;
}

bool checkOpcodeExistsInOPTAB(string OPCODE)
{
    if (OPTAB.find(OPCODE) != OPTAB.end())
    {
        return true;
    }
    return false;
}

void readNextToken(string fileLine, int &index, string &token)
{
    token = "";
    while ((index <= fileLine.length() - 1) && (fileLine[index] != ' ' && fileLine[index] != '\t'))
    {
        token += fileLine[index];
        index++;
    }
    while (fileLine[index] == ' ' || fileLine[index] == '\t')
    {
        index++;
    }
}

void processIntermediateFileLine(string fileLine, int &lineNum, string &address, string &LABEL, string &OPCODE, string &OPERAND)
{
    int index = 0;
    string tempLineNum;
    address = "", LABEL = "", OPCODE = "", OPERAND = "";

    while (fileLine[index] == ' ' || fileLine[index] == '\t')
    {
        index++;
    }
    while ((index <= fileLine.length() - 1) && (fileLine[index] != ' ' && fileLine[index] != '\t'))
    {
        tempLineNum += fileLine[index];
        index++;
    }
    lineNum = stoi(tempLineNum);

    while (fileLine[index] == ' ' || fileLine[index] == '\t')
    {
        index++;
    }
    while ((index <= fileLine.length() - 1) && (fileLine[index] != ' ' && fileLine[index] != '\t'))
    {
        address += fileLine[index];
        index++;
    }

    int spCtr = 0;
    while (fileLine[index] == ' ' || fileLine[index] == '\t')
    {
        index++;
        spCtr++;
    }
    while ((index <= fileLine.length() - 1) && (fileLine[index] != ' ' && fileLine[index] != '\t') && (spCtr < 10))
    {
        LABEL += fileLine[index];
        index++;
    }

    while (fileLine[index] == ' ' || fileLine[index] == '\t')
    {
        index++;
    }
    while ((index <= fileLine.length() - 1) && (fileLine[index] != ' ' && fileLine[index] != '\t'))
    {
        OPCODE += fileLine[index];
        index++;
    }

    while (fileLine[index] == ' ' || fileLine[index] == '\t')
    {
        index++;
    }
    while ((index <= fileLine.length() - 1) && (fileLine[index] != ' ' && fileLine[index] != '\t'))
    {
        OPERAND += fileLine[index];
        index++;
    }
}

bool checkIfStringIsNumeric(string s)
{
    for (int i = 0; i < s.length(); i++)
    {
        if (!isdigit(s[i]))
        {
            return false;
        }
    }
    return true;
}

#endif