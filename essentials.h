#ifndef ESSENTIALS_H
#define ESSENTIALS_H

#include <bits/stdc++.h>
#include "OPTAB.h"
#include "SYMTAB.h"
#include "REGTAB.h"
#include "LITTAB.h"
#include "CSECTTAB.h"

using namespace std;

class EvaluateString
{
public:
    int getResult();
    EvaluateString(string data);

private:
    string storedData;
    int index;
    char peek();
    char get();
    int term();
    int factor();
    int number();
};

EvaluateString::EvaluateString(string data)
{
    storedData = data;
    index = 0;
}

int EvaluateString::getResult()
{
    int result = term();
    while (peek() == '+' || peek() == '-')
    {
        if (get() == '+')
        {
            result += term();
        }
        else
        {
            result -= term();
        }
    }
    return result;
}

int EvaluateString::term()
{
    int result = factor();
    while (peek() == '*' || peek() == '/')
    {
        if (get() == '*')
        {
            result *= factor();
        }
        else
        {
            result /= factor();
        }
    }
    return result;
}

int EvaluateString::factor()
{
    if (peek() >= '0' && peek() <= '9')
    {
        return number();
    }
    else if (peek() == '(')
    {
        get();
        int result = getResult();
        get();
        return result;
    }
    else if (peek() == '-')
    {
        get();
        return -factor();
    }
    return 0;
}

int EvaluateString::number()
{
    int result = get() - '0';
    while (peek() >= '0' && peek() <= '9')
    {
        result = 10 * result + get() - '0';
    }
    return result;
}

char EvaluateString::get()
{
    return storedData[index++];
}

char EvaluateString::peek()
{
    return storedData[index];
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

bool checkLabelExistsInLITTAB(string LABEL)
{
    if (LITTAB.find(LABEL) != LITTAB.end())
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

void readByteOperand(string fileLine, int &index, string &OPERAND)
{
    OPERAND = "";
    if (fileLine[index] == 'C')
    {
        OPERAND += fileLine[index++];
        char identifier = fileLine[index++];
        OPERAND += identifier;
        while (index < fileLine.length() && fileLine[index] != identifier)
        {
            OPERAND += fileLine[index];
            index++;
        }
        fileLine += identifier;
        index++;
    }
    else
    {
        while (index < fileLine.length() && fileLine[index] != ' ' && fileLine[index] != '\t')
        {
            OPERAND += fileLine[index];
            index++;
        }
    }

    while (index < fileLine.length() && fileLine[index] == ' ' && fileLine[index] == '\t')
    {
        index++;
    }
}

void evaluateExp(string exp, bool &relative, string &tempOPERAND, int lineNum, ofstream &errorFile, bool &errorFlag)
{
    string singleOperand = "?", singleOperator = "?", valueString = "", valueTemp = "", writeData = "";
    int lastOperand = 0, lastOperator = 0, pairCount = 0;
    char lastByte = ' ';
    bool Illegal = false;

    for (int i = 0; i < exp.length();)
    {
        singleOperand = "";

        lastByte = exp[i];
        while ((lastByte != '+' && lastByte != '-' && lastByte != '/' && lastByte != '*') && i < exp.length())
        {
            singleOperand += lastByte;
            lastByte = exp[++i];
        }

        if (checkLabelExistsInSYMTAB(singleOperand))
        {
            lastOperand = (SYMTAB[singleOperand].type == 'R' ? 1 : 0);
            valueTemp = to_string(stoi(SYMTAB[singleOperand].address, nullptr, 16));
        }
        else if ((singleOperand != "" || singleOperand != "?") && checkIfStringIsNumeric(singleOperand))
        {
            lastOperand = 0;
            valueTemp = singleOperand;
        }
        else
        {
            errorFile << "Line " << lineNum << " : Can't find symbol" << endl;
            Illegal = true;
            break;
        }

        if (lastOperand * lastOperator == 1)
        {
            errorFile << "Line " << lineNum << " : Illegal expression" << endl;
            errorFlag = true;
            Illegal = true;
            break;
        }
        else if ((singleOperator == "-" || singleOperator == "+" || singleOperator == "?") && lastOperand == 1)
        {
            if (singleOperator == "-")
            {
                pairCount--;
            }
            else
            {
                pairCount++;
            }
        }

        valueString += valueTemp;

        singleOperator = "";
        while (i < exp.length() && (lastByte == '+' || lastByte == '-' || lastByte == '/' || lastByte == '*'))
        {
            singleOperator += lastByte;
            lastByte = exp[++i];
        }

        if (singleOperator.length() > 1)
        {
            errorFile << "Line " << lineNum << " : Illegal operator in expression" << endl;
            errorFlag = true;
            Illegal = true;
            break;
        }

        if (singleOperator == "*" || singleOperator == "/")
        {
            lastOperator = 1;
        }
        else
        {
            lastOperator = 0;
        }

        valueString += singleOperator;
    }

    if (!Illegal)
    {
        if (pairCount == 1)
        {
            relative = 1;
            EvaluateString tempOBJ(valueString);
            char temp[10];
            sprintf(temp, "%05X", tempOBJ.getResult());
            tempOPERAND = temp;
        }
        else if (pairCount == 0)
        {
            relative = 0;
            cout << valueString << endl;
            EvaluateString tempOBJ(valueString);
            char temp[10];
            sprintf(temp, "%05X", tempOBJ.getResult());
            tempOPERAND = temp;
        }
        else
        {
            errorFile << "Line " << lineNum << " : Illegal expression" << endl;
            errorFlag = true;
            tempOPERAND = "00000";
            relative = 0;
        }
    }
    else
    {
        tempOPERAND = "00000";
        errorFlag = true;
        relative = 0;
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

    if (OPCODE == "BYTE")
    {
        readByteOperand(fileLine, index, OPERAND);
    }
    else
    {
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
}

void processEXTDEFOperand(string currSectName, string OPERAND)
{
    string tempLABEL;
    for (int i = 0; i < OPERAND.length(); i++)
    {
        while (OPERAND[i] != ',' && i < OPERAND.length())
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
    for (int i = 0; i < OPERAND.length(); i++)
    {
        while (OPERAND[i] != ',' && i < OPERAND.length())
        {
            tempLABEL += OPERAND[i];
            i++;
        }
        CSECTTAB[currSectName].EXTREFTAB[tempLABEL] = "";
        tempLABEL = "";
    }
}


#endif