#include <bits/stdc++.h>
#include "essentials.h"
using namespace std;

void writeHeaderRecord(ofstream &objectFile, string LABEL, string address, int programLength)
{
    LABEL.resize(6, ' ');
    address.insert(0, 2, '0');
    char tempPL[10];
    sprintf(tempPL, "%06X", programLength);
    objectFile << "H^" << LABEL << '^' << address << '^' << tempPL << endl;
}

void writeTextRecord(ofstream &objectFile, string address, string OBJECTCODE, string &currRec, bool lastRec = false)
{
    if (lastRec)
    {
        if (currRec.length() - count(currRec.begin(), currRec.end(), '^') > 0)
        {
            char recLength[10];
            sprintf(recLength, "%02X", (currRec.length() - count(currRec.begin(), currRec.end(), '^')) / 2);
            currRec.pop_back();
            objectFile << recLength << "^" << currRec << endl;
            currRec = "";
        }
    }
    else
    {
        if (OBJECTCODE.length() != 0)
        {
            if (currRec.length() - count(currRec.begin(), currRec.end(), '^') == 0)
            {
                char tempAddr[10];
                sprintf(tempAddr, "%06X", address);
                address.insert(0, 2, '0');
                objectFile << "T^" << address << "^";
            }

            if (currRec.length() + OBJECTCODE.length() - count(currRec.begin(), currRec.end(), '^') > 60)
            {
                char recLength[10];
                sprintf(recLength, "%02X", (currRec.length() - count(currRec.begin(), currRec.end(), '^')) / 2);
                currRec.pop_back();
                objectFile << recLength << "^" << currRec << endl;
                currRec = "";

                char tempAddr[10];
                sprintf(tempAddr, "%06X", address);
                address.insert(0, 2, '0');
                objectFile << "T^" << address << "^";
            }
            currRec += OBJECTCODE;
            currRec += "^";
        }
        else
        {
            if (OPCODE != "START" && OPCODE != "END" && OPCODE != "BASE" && currRec.length() - count(currRec.begin(), currRec.end(), '^') > 0)
            {
                char temp[10];
                sprintf(temp, "%02X", (currRec.length() - count(currRec.begin(), currRec.end(), '^')) / 2);
                currRec.pop_back();
                objectFile << temp << "^" << currRec << endl;
                currRec = "";
            }
        }
    }
}

void writeEndRecord(ofstream &objectFile, string startAddr)
{
    startAddr.insert(0, 2, '0');
    objectFile << "E^" << startAddr << endl;
}

void pass2()
{
    ifstream intermediateFile;
    ofstream errorFile, objectFile, listingFile;
    string fileLine, address, LABEL, OPCODE, OPERAND, OBJECTCODE, startAddr, operand1, operand2, tempOPERAND, tempOPCODE, currRec, endRec;
    int lineNum, totalBits, baseRegVal, operandAddr, immValue, pc, relativeAddr;
    bool format4Flag, base;

    intermediateFile.open("intermediate_file");
    if (!intermediateFile)
    {
        cout << "Unable to open file: intermediate_file" << endl;
        exit(1);
    }

    objectFile.open("object_file");
    if (!objectFile)
    {
        cout << "Unable to open file: object_file" << endl;
        exit(1);
    }

    listingFile.open("listing_file");
    if (!listingFile)
    {
        cout << "Unable to open file: listing_file" << endl;
        exit(1);
    }
    listingFile << setw(10) << "Line" << setw(10) << "Address" << setw(10) << "Label" << setw(10) << "Opcode" << setw(15) << "Operand" << setw(15) << "Object Code" << endl;
    listingFile << "======================================================================" << endl;

    errorFile.open("error_file", fstream::app);
    if (!errorFile)
    {
        cout << "Unable to open file: error_file" << endl;
        exit(1);
    }

    getline(intermediateFile, fileLine);
    getline(intermediateFile, fileLine);
    getline(intermediateFile, fileLine);
    processIntermediateFileLine(fileLine, lineNum, address, LABEL, OPCODE, OPERAND);

    if (OPCODE == "START")
    {
        startAddr = address;
        listingFile << setw(10) << lineNum << setw(10) << address << setw(10) << LABEL << setw(10) << OPCODE << setw(15) << OPERAND << setw(15) << OBJECTCODE << endl;

        getline(intermediateFile, fileLine);
    }

    writeHeaderRecord(objectFile, LABEL, address, programLength);
    processIntermediateFileLine(fileLine, lineNum, address, LABEL, OPCODE, OPERAND);

    while (OPCODE != "END")
    {
        if (!checkCommentLine(fileLine))
        {
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
                if (OPTAB[tempOPCODE].format == 1)
                {
                    OBJECTCODE = tempOPCODE;
                }
                else if (OPTAB[tempOPCODE].format == 2)
                {
                    operand1 = OPERAND.substr(0, OPERAND.find(','));
                    operand2 = OPERAND.substr(OPERAND.find(',') + 1, OPERAND.length() - OPERAND.find(',') - 1);
                }
                else if (OPTAB[tempOPCODE].format == 3)
                {
                    if (tempOPCODE == "RSUB")
                    {
                        OBJECTCODE = OPTAB[tempOPCODE].opcode + (format4Flag ? "000000" : "0000");
                    }
                    else
                    {
                        totalBits = format4Flag ? 20 : 12;
                        if (OPERAND[0] == '#') // Immediate Addressing
                        {
                            if (OPERAND.substr(OPERAND.length() - 2, 2) == ",X")
                            {
                                errorFile << "Line " << lineNum << " : Index based addressing not supported with Immediate addressing" << endl;
                            }
                            else
                            {
                                tempOPERAND = OPERAND.substr(1, OPERAND.length() - 1);
                                if (checkIfStringIsNumeric(tempOPERAND))
                                {
                                    immValue = stoi(tempOPERAND);
                                    if (immValue >= (1 << totalBits))
                                    {
                                        errorFile << "Line " << lineNum << " : Immediate value exceeds format limit" << endl;
                                    }
                                    else
                                    {
                                        char temp[20];
                                        sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 1);
                                        OBJECTCODE = temp;
                                        OBJECTCODE += (format4Flag ? '1' : '0');
                                        if (format4Flag)
                                        {
                                            sprintf(temp, "%05X", immValue);
                                        }
                                        else
                                        {
                                            sprintf(temp, "%03X", immValue);
                                        }
                                        OBJECTCODE += temp;
                                    }
                                }
                                else if (checkLabelExistsInSYMTAB(tempOPERAND) && SYMTAB[tempOPERAND].type == 'A')
                                {
                                    immValue = stoi(tempOPERAND, nullptr, 16);
                                    if (immValue >= (1 << totalBits))
                                    {
                                        errorFile << "Line " << lineNum << " : Immediate value exceeds format limit" << endl;
                                    }
                                    else
                                    {
                                        char temp[20];
                                        sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 1);
                                        OBJECTCODE = temp;
                                        OBJECTCODE += (format4Flag ? '1' : '0');
                                        if (format4Flag)
                                        {
                                            sprintf(temp, "%05X", immValue);
                                        }
                                        else
                                        {
                                            sprintf(temp, "%03X", immValue);
                                        }
                                        OBJECTCODE += temp;
                                    }
                                }
                                else
                                {
                                    operandAddr = stoi(SYMTAB[tempOPERAND].address, nullptr, 16);
                                    if (format4Flag)
                                    {
                                        char temp[20];
                                        sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 1);
                                        OBJECTCODE = temp;
                                        OBJECTCODE += '1'; // x = 0, b = 0, p = 0, e = 1
                                        sprintf(temp, "%05X", operandAddr);
                                        OBJECTCODE += temp;
                                    }
                                    else
                                    {
                                        pc = stoi(address, nullptr, 16);
                                        pc += (format4Flag ? 4 : 3);
                                        relativeAddr = operandAddr - pc;
                                        if (relativeAddr <= 524287 && relativeAddr >= -524288)
                                        {
                                            char temp[20];
                                            sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 1);
                                            OBJECTCODE = temp;
                                            OBJECTCODE += '2'; // x = 0, b = 0, p = 1, e = 0
                                            relativeAddr &= 0xfffff;
                                            sprintf(temp, "%05X", relativeAddr);
                                            OBJECTCODE += temp;
                                        }
                                        else if (base)
                                        {
                                            relativeAddr = operandAddr - baseRegVal;
                                            if (relativeAddr <= 1048575 && relativeAddr >= 0)
                                            {
                                                char temp[20];
                                                sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 1);
                                                OBJECTCODE = temp;
                                                OBJECTCODE += '4'; // x = 0, b = 1, p = 0, e = 0
                                                relativeAddr &= 0xfffff;
                                                sprintf(temp, "%05X", relativeAddr);
                                                OBJECTCODE += temp;
                                            }
                                        }
                                        else if (operandAddr <= 1048575)
                                        {
                                            char temp[20];
                                            sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 1);
                                            OBJECTCODE = temp;
                                            OBJECTCODE += '0'; // x = 0, b = 0, p = 0, e = 0
                                            sprintf(temp, "%05X", operandAddr);
                                            OBJECTCODE += temp;
                                        }
                                    }
                                }
                            }
                        }
                        else if (OPERAND[0] == '@')
                        {
                        }
                        else if (OPERAND.substr(OPERAND.length() - 2, 2) == ",X")
                        {
                            tempOPERAND = OPERAND.substr(0, OPERAND.length() - 2);
                            operandAddr = stoi(SYMTAB[tempOPERAND].address, nullptr, 16);
                            pc = stoi(address, nullptr, 16);
                            pc += (format4Flag ? 4 : 3);
                            relativeAddr = operandAddr - pc;
                            if ((!format4Flag && relativeAddr <= 2047 && relativeAddr >= -2048) || (format4Flag && relativeAddr <= 524287 && relativeAddr >= -524288))
                            {
                                char temp[20];
                                sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 3); // n = 1, i = 1
                                OBJECTCODE = temp;
                                OBJECTCODE += (format4Flag ? 'B' : 'A'); // x = 1, b = 0, p = 1, e = 0/1
                                if (format4Flag)
                                {
                                    relativeAddr &= 0xfffff;
                                    sprintf(temp, "%05X", relativeAddr);
                                }
                                else
                                {
                                    relativeAddr &= 0xfff;
                                    sprintf(temp, "%03X", relativeAddr);
                                }
                                OBJECTCODE += temp;
                            }
                            else if (base)
                            {
                                relativeAddr = operandAddr - baseRegVal;
                                if ((!format4Flag && relativeAddr <= 4095 && relativeAddr >= 0) || (format4Flag && relativeAddr <= 1048575 && relativeAddr >= 0))
                                {
                                    char temp[20];
                                    sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 3); // n = 1, i = 1
                                    OBJECTCODE = temp;
                                    OBJECTCODE += (format4Flag ? 'D' : 'C'); // x = 1, b = 1, p = 0, e = 0/1
                                    if (format4Flag)
                                    {
                                        relativeAddr &= 0xfffff;
                                        sprintf(temp, "%05X", relativeAddr);
                                    }
                                    else
                                    {
                                        relativeAddr &= 0xfff;
                                        sprintf(temp, "%03X", relativeAddr);
                                    }
                                    OBJECTCODE += temp;
                                }
                            }
                            else if ((!format4Flag && operandAddr <= 4095) || (format4Flag && operandAddr <= 1048575))
                            {
                                char temp[20];
                                sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 3); // n = 1, i = 1
                                OBJECTCODE = temp;
                                OBJECTCODE += (format4Flag ? '9' : '8'); // x = 1, b = 0, p = 0, e = 0/1
                                if (format4Flag)
                                    sprintf(temp, "%05X", operandAddr);
                                else
                                    sprintf(temp, "%03X", operandAddr);
                                OBJECTCODE += temp;
                            }
                        }
                        else
                        {
                            operandAddr = stoi(SYMTAB[OPERAND].address, nullptr, 16);
                            pc = stoi(address, nullptr, 16);
                            pc += (format4Flag ? 4 : 3);
                            relativeAddr = operandAddr - pc;
                            if (!format4Flag && relativeAddr <= 2047 && relativeAddr >= -2048)
                            {
                                char temp[20];
                                sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 3); // n = 1, i = 1
                                OBJECTCODE = temp;
                                OBJECTCODE += (format4Flag ? '3' : '2'); // x = 0, b = 0, p = 1, e = 0/1
                                if (format4Flag)
                                {
                                    relativeAddr &= 0xfffff;
                                    sprintf(temp, "%05X", relativeAddr);
                                }
                                else
                                {
                                    relativeAddr &= 0xfff;
                                    sprintf(temp, "%03X", relativeAddr);
                                }
                                OBJECTCODE += temp;
                            }
                            else if (!format4Flag && base)
                            {
                                relativeAddr = operandAddr - baseRegVal;
                                if ((!format4Flag && relativeAddr <= 4095 && relativeAddr >= 0) || (format4Flag && relativeAddr <= 1048575 && relativeAddr >= 0))
                                {
                                    char temp[20];
                                    sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 3); // n = 1, i = 1
                                    OBJECTCODE = temp;
                                    OBJECTCODE += (format4Flag ? '5' : '4'); // x = 0, b = 1, p = 0, e = 0/1
                                    if (format4Flag)
                                    {
                                        relativeAddr &= 0xfffff;
                                        sprintf(temp, "%05X", relativeAddr);
                                    }
                                    else
                                    {
                                        relativeAddr &= 0xfff;
                                        sprintf(temp, "%03X", relativeAddr);
                                    }
                                    OBJECTCODE += temp;
                                }
                            }
                            else if ((!format4Flag && operandAddr <= 4095) || (format4Flag && operandAddr <= 1048575))
                            {
                                char temp[20];
                                sprintf(temp, "%02X", stoi(OPTAB[tempOPCODE].opcode, nullptr, 16) + 3); // n = 1, i = 1
                                OBJECTCODE = temp;
                                OBJECTCODE += (format4Flag ? '1' : '0'); // x = 0, b = 0, p = 0, e = 0/1
                                if (format4Flag)
                                    sprintf(temp, "%05X", operandAddr);
                                else
                                    sprintf(temp, "%03X", operandAddr);
                                OBJECTCODE += temp;
                            }
                        }
                    }
                }
            }
            else if (tempOPCODE == "BYTE")
            {
            }
            else if (tempOPCODE == "WORD")
            {
                char temp[20];
                sprintf(temp, "%06X", stoi(OPERAND, nullptr, 16));
                OBJECTCODE = temp;
            }
            else if (tempOPCODE == "BASE")
            {
                if (checkLabelExistsInSYMTAB(OPERAND))
                {
                    baseRegVal = stoi(SYMTAB[OPERAND].address, nullptr, 16);
                    base = true;
                }
                else
                {
                    errorFile << "Line " << lineNum << " : Symbol not found" << endl;
                }
            }

            writeTextRecord(objectFile, address, OBJECTCODE, currRec);
        }

        listingFile << setw(10) << lineNum << setw(10) << address << setw(10) << LABEL << setw(10) << OPCODE << setw(15) << OPERAND << setw(15) << OBJECTCODE << endl;
        getline(intermediateFile, fileLine);
        processIntermediateFileLine(fileLine, lineNum, address, LABEL, OPCODE, OPERAND);
        OBJECTCODE = "";
    }

    writeTextRecord(objectFile, address, OBJECTCODE, currRec, true);
    writeEndRecord(objectFile, startAddr);
    listingFile << setw(10) << lineNum << setw(10) << address << setw(10) << LABEL << setw(10) << OPCODE << setw(15) << OPERAND << setw(15) << OBJECTCODE << endl;

    intermediateFile.close();
    errorFile.close();
    listingFile.close();
    objectFile.close();
}