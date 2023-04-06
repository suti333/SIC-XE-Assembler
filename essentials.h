#ifndef ESSENTIALS_H
#define ESSENTIALS_H

#include <bits/stdc++.h>
#include "OPTAB.h"
#include "SYMTAB.h"
#include "REGTAB.h"
#include "CSECTTAB.h"

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

bool checkLabelExistsInEXTDEFTAB(string currSectName, string LABEL)
{
    if (CSECTTAB[currSectName].EXTDEFTAB.find(LABEL) != CSECTTAB[currSectName].EXTDEFTAB.end())
    {
        return true;
    }
    return false;
}

bool checkLabelExistsInEXTREFTAB(string currSectName, string LABEL)
{
    if (CSECTTAB[currSectName].EXTREFTAB.find(LABEL) != CSECTTAB[currSectName].EXTREFTAB.end())
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

    int spCtr = 0;
    while (fileLine[index] == ' ' || fileLine[index] == '\t')
    {
        index++;
        spCtr++;
    }
    while ((index <= fileLine.length() - 1) && (fileLine[index] != ' ' && fileLine[index] != '\t' && (spCtr < 10)))
    {
        address += fileLine[index];
        index++;
    }

    while (fileLine[index] == ' ' || fileLine[index] == '\t')
    {
        index++;
        spCtr++;
    }
    while ((index <= fileLine.length() - 1) && (fileLine[index] != ' ' && fileLine[index] != '\t') && (spCtr < 20))
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

void processEXTDEFOperand(string currSectName, string OPERAND)
{
    string tempLABEL;
    for (int i=0; i<OPERAND.length(); i++)
    {
        while (OPERAND[i] != ',' && i<OPERAND.length())
        {
            tempLABEL += OPERAND[i];
            i++;
        }
        CSECTTAB[currSectName].EXTDEFTAB[tempLABEL] = "";
        tempLABEL = "";
    }
}

void processEXTREFOperand(string currSectName, string OPERAND)
{
    string tempLABEL;
    for (int i=0; i<OPERAND.length(); i++)
    {
        while (OPERAND[i] != ',' && i<OPERAND.length())
        {
            tempLABEL += OPERAND[i];
            i++;
        }
        CSECTTAB[currSectName].EXTREFTAB[tempLABEL] = "";
        tempLABEL = "";
    }
}

#endif