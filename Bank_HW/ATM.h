//
// Created by os on 27/07/24.
//

#ifndef ATM_H
#define ATM_H
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "bank.h"
#include <sstream>
//NameSpace
using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::cerr;
using std::stringstream;
using std::stoi;
//Const's
const string OPEN_ACCOUNT = "O";
const string DEPOSIT = "D";
const string WITHDRAWAL = "W";
const string BALANCE_CHECK = "B";
const string CLOSE_ACCOUNT = "Q";
const string TRANSFER = "T";
//ATM CLASS
class ATM {
private:
    int atmId = -1;


public:
    void atmInit(int atmId) {
        this->atmId = atmId;
    }

    void parseInput(const string& fileName,bank* globalBank) const;

    const int getNextVal(string &operatioSting);

};

#endif //ATM_H
