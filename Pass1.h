#include <bits/stdc++.h>
#include "essentials.h"
using namespace std;

ifstream sourceFile;
ofstream intermediateFile, errorFile;
string fileName, fileLine, LABEL, OPCODE, OPERAND, OPERAND2, tempOPCODE, tempOPERAND, currSectName, firstExecutableSect;
int lineNum = 0, index = 0, LOCCTR, prevLOCCTR = 0, startAddr, programLength, sectCtr;
bool format4Flag = false, errorFlag;

void pass1()
{
    sourceFile.open(fileName);
    if (!sourceFile)
    {
        cout << "Unable to open file: " << fileName << endl;
        exit(1);
    }

    intermediateFile.open("intermediate_file");
    if (!intermediateFile)
    {
        cout << "Unable to open file: intermediate_file" << endl;
        exit(1);
    }

    intermediateFile << setw(10) << "Line" << setw(10) << "Address" << setw(10) << "Label" << setw(10) << "Opcode" << setw(15) << "Operand" << endl;
    intermediateFile << "=======================================================" << endl;

    errorFile.open("error_file");
    if (!errorFile)
    {
        cout << "Unable to open file: error_file" << endl;
        exit(1);
    }

    getline(sourceFile, fileLine);
    lineNum += 5;
    readNextToken(fileLine, index, LABEL);
    readNextToken(fileLine, index, OPCODE);

    if (OPCODE == "START")
    {
        readNextToken(fileLine, index, OPERAND);
        startAddr = stoi(OPERAND, nullptr, 16);
        LOCCTR = startAddr;
        char address[10];
        sprintf(address, "%04X", (LOCCTR - prevLOCCTR));
        intermediateFile << setw(10) << lineNum << setw(10) << address << setw(10) << LABEL << setw(10) << OPCODE << setw(15) << OPERAND << endl;

        getline(sourceFile, fileLine);
        lineNum += 5;
        index = 0;
        readNextToken(fileLine, index, LABEL);
        readNextToken(fileLine, index, OPCODE);
    }
    else
    {
        LOCCTR = 0;
    }

    currSectName = "DEFAULT";
    sectCtr = 0;

    while (OPCODE != "END")
    {
        while (OPCODE != "CSECT" && OPCODE != "END")
        {
            if (!checkCommentLine(fileLine))
            {
                if (LABEL.length() != 0)
                {
                    if (checkLabelExistsInSYMTAB(LABEL))
                    {
                        errorFile << "Line " << lineNum << " : Duplicate symbol for '" << LABEL << "'. Previously defined at " << SYMTAB[LABEL].address << endl;
                        errorFlag = true;
                    }
                    else
                    {
                        char address[10];
                        sprintf(address, "%04X", LOCCTR);
                        SYMTAB[LABEL].address = address;
                        SYMTAB[LABEL].type = 'R';
                        if (checkLabelExistsInEXTDEFTAB(currSectName, LABEL))
                        {
                            CSECTTAB[currSectName].EXTDEFTAB[LABEL] = SYMTAB[LABEL].address;
                        }
                    }
                }

                if (OPCODE[0] == '+')
                {
                    format4Flag = true;
                    tempOPCODE = OPCODE.substr(1, OPCODE.length() - 1);
                }
                else
                {
                    format4Flag = false;
                    tempOPCODE = OPCODE;
                }

                if (checkOpcodeExistsInOPTAB(tempOPCODE))
                {
                    if (OPTAB[tempOPCODE].format == 3)
                    {
                        LOCCTR += 3;
                        prevLOCCTR += 3;
                        if (format4Flag)
                        {
                            LOCCTR++;
                            prevLOCCTR++;
                        }
                        if (tempOPCODE == "RSUB")
                        {
                            OPERAND = "";
                        }
                        else
                        {
                            readNextToken(fileLine, index, OPERAND);
                            if (OPERAND[OPERAND.length() - 1] == ',')
                            {
                                readNextToken(fileLine, index, OPERAND2);
                                OPERAND += OPERAND2;
                            }
                        }

                        if (OPCODE[0] == '=')
                        {
                            tempOPERAND = OPERAND.substr(1, OPERAND.length() - 1);
                            if (tempOPERAND == "*")
                            {
                                char address[10];
                                sprintf(address, "%06X", (LOCCTR - prevLOCCTR));
                                tempOPERAND = "X'" + string(address) + "'";
                            }
                            if (!checkLabelExistsInLITTAB(tempOPERAND))
                            {
                                LITTAB[tempOPERAND] = "";
                            }
                        }
                    }
                    else if (OPTAB[tempOPCODE].format == 1)
                    {
                        OPERAND = "";
                        LOCCTR++;
                        prevLOCCTR++;
                    }
                    else if (OPTAB[tempOPCODE].format == 2)
                    {
                        LOCCTR += 2;
                        prevLOCCTR += 2;
                        readNextToken(fileLine, index, OPERAND);
                        if (OPERAND[OPERAND.length() - 1] == ',')
                        {
                            readNextToken(fileLine, index, OPERAND2);
                            OPERAND += OPERAND2;
                        }
                    }
                }
                else if (tempOPCODE == "WORD")
                {
                    readNextToken(fileLine, index, OPERAND);
                    LOCCTR += 3;
                    prevLOCCTR += 3;
                }
                else if (tempOPCODE == "RESW")
                {
                    readNextToken(fileLine, index, OPERAND);
                    LOCCTR += 3 * stoi(OPERAND);
                    prevLOCCTR += 3 * stoi(OPERAND);
                }
                else if (tempOPCODE == "RESB")
                {
                    readNextToken(fileLine, index, OPERAND);
                    LOCCTR += stoi(OPERAND);
                    prevLOCCTR += stoi(OPERAND);
                }
                else if (tempOPCODE == "BYTE")
                {
                    readByteOperand(fileLine, index, OPERAND);
                    if (OPERAND[0] == 'X')
                    {
                        LOCCTR += (OPERAND.length() - 3) / 2;
                        prevLOCCTR += (OPERAND.length() - 3) / 2;
                    }
                    else if (OPERAND[0] == 'C')
                    {
                        LOCCTR += OPERAND.length() - 3;
                        prevLOCCTR += OPERAND.length() - 3;
                    }
                }
                else if (tempOPCODE == "BASE")
                {
                    readNextToken(fileLine, index, OPERAND);
                }
                else if (tempOPCODE == "LTORG")
                {
                    OPERAND = "";
                }
                else if (tempOPCODE == "ORG")
                {
                    readNextToken(fileLine, index, OPERAND);
                    char lastByte = OPERAND[OPERAND.length() - 1];
                    while (lastByte == '+' || lastByte == '-' || lastByte == '/' || lastByte == '*')
                    {
                        readNextToken(fileLine, index, tempOPERAND);
                        OPERAND += tempOPERAND;
                        lastByte = OPERAND[OPERAND.length() - 1];
                    }

                    if (checkLabelExistsInSYMTAB(OPERAND))
                    {
                        LOCCTR = stoi(SYMTAB[OPERAND].address, nullptr, 16);
                    }
                    else
                    {
                        bool relative;
                        errorFlag = false;
                        evaluateExp(OPERAND, relative, tempOPERAND, lineNum, errorFile, errorFlag);
                        if (!errorFlag)
                        {
                            LOCCTR = stoi(tempOPERAND, nullptr, 16);
                        }
                        errorFlag = false;
                    }
                }
                else if (tempOPCODE == "USE")
                {
                    readNextToken(fileLine, index, OPERAND);
                }
                else if (tempOPCODE == "EQU")
                {
                    readNextToken(fileLine, index, OPERAND);
                    tempOPERAND = "";
                    bool relative;

                    if (OPERAND == "*")
                    {
                        char address[10];
                        sprintf(address, "%06X", (LOCCTR - prevLOCCTR));
                        tempOPERAND = address;
                        relative = true;
                    }
                    else if (checkIfStringIsNumeric(OPERAND))
                    {
                        char address[10];
                        sprintf(address, "%06X", stoi(OPERAND));
                        tempOPERAND = address;
                        relative = false;
                    }
                    else
                    {
                        char lastByte = OPERAND[OPERAND.length() - 1];

                        while (lastByte == '+' || lastByte == '-' || lastByte == '/' || lastByte == '*')
                        {
                            readNextToken(fileLine, index, tempOPERAND);
                            OPERAND += tempOPERAND;
                            lastByte = OPERAND[OPERAND.length() - 1];
                        }
                        evaluateExp(OPERAND, relative, tempOPERAND, lineNum, errorFile, errorFlag);
                    }

                    SYMTAB[LABEL].address = tempOPERAND;
                    SYMTAB[LABEL].type = (relative ? 'R' : 'A');
                    prevLOCCTR = LOCCTR - stoi(tempOPERAND, nullptr, 16);
                }
                else if (tempOPCODE == "EXTDEF")
                {
                    readNextToken(fileLine, index, OPERAND);
                    processEXTDEFOperand(currSectName, OPERAND);
                }
                else if (tempOPCODE == "EXTREF")
                {
                    readNextToken(fileLine, index, OPERAND);
                    processEXTREFOperand(currSectName, OPERAND);
                }
                else
                {
                    errorFile << "Line " << lineNum << ": Invalid OPCODE";
                    errorFlag = true;
                }
            }

            if (OPCODE == "EXTDEF" || OPCODE == "EXTREF")
            {
                lineNum -= 5;
                intermediateFile << setw(10) << lineNum << setw(20) << LABEL << setw(10) << OPCODE << setw(15) << OPERAND << endl;
            }
            else
            {
                char address[10];
                sprintf(address, "%04X", (LOCCTR - prevLOCCTR));
                intermediateFile << setw(10) << lineNum << setw(10) << address << setw(10) << LABEL << setw(10) << OPCODE << setw(15) << OPERAND << endl;
            }

            getline(sourceFile, fileLine);
            lineNum += 5;
            index = 0;
            prevLOCCTR = 0;
            readNextToken(fileLine, index, LABEL);
            readNextToken(fileLine, index, OPCODE);
        }

        if (OPCODE == "CSECT")
        {
            lineNum -= 5;
            OPERAND = "";
        }
        if (OPCODE != "END")
        {

            if (!checkLabelExistsInSYMTAB(LABEL))
            {
                char address[10];
                sprintf(address, "%04X", LOCCTR);
                SYMTAB[LABEL].address = address;
                SYMTAB[LABEL].type = 'R';
            }

            char address[10];
            sprintf(address, "%04X", (LOCCTR - prevLOCCTR));
            CSECTTAB[currSectName].LOCCTR = address;
            CSECTTAB[currSectName].length = (LOCCTR - prevLOCCTR);
            LOCCTR = prevLOCCTR = 0;
            currSectName = LABEL;
            sectCtr++;
            CSECTTAB[currSectName].sect_num = sectCtr;

            sprintf(address, "%04X", (LOCCTR - prevLOCCTR));
            intermediateFile << setw(10) << lineNum << setw(10) << address << setw(10) << LABEL << setw(10) << OPCODE << setw(15) << OPERAND << endl;
            getline(sourceFile, fileLine);
            lineNum += 5;
            index = 0;
            readNextToken(fileLine, index, LABEL);
            readNextToken(fileLine, index, OPCODE);
        }
        else
        {
            char address[10];
            sprintf(address, "%04X", (LOCCTR - prevLOCCTR));
            CSECTTAB[currSectName].LOCCTR = address;
            CSECTTAB[currSectName].length = (LOCCTR - prevLOCCTR);

            CSECTTAB[currSectName].sect_num = sectCtr;
        }
    }

    if (OPCODE == "END")
    {
        firstExecutableSect = LABEL;
        SYMTAB[firstExecutableSect].address = firstExecutableSect;
    }

    readNextToken(fileLine, index, OPERAND);

    char address[10];
    sprintf(address, "%04X", (LOCCTR - prevLOCCTR));
    intermediateFile << setw(10) << lineNum << setw(10) << address << setw(10) << LABEL << setw(10) << OPCODE << setw(15) << OPERAND << endl;
    programLength = LOCCTR - startAddr;

    sourceFile.close();
    intermediateFile.close();
    errorFile.close();
}
