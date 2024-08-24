//
// Created by os on 27/07/24.
//

#ifndef LOGGER_H
#define LOGGER_H

//Include
#include <string>
#include <fstream>
#include <iostream>
//NameSpace
using std::string;
using std::ofstream;
using std::cout;
using std::endl;
using std::to_string;
using std::cerr;

class logger {
    const string logFileName = "log.txt";
    ofstream logStream;
    pthread_mutex_t logMutex{};

public:
    logger();

    ~logger();

    void writeOpenAccount(int accountId,int password,int amount,int atmId);

    void openAccourErrAccountExists(int atmId);

    void passwordIncorrect(int atmId,int accoundId);

    void accountDoesNotExist(int atmId,int accountId);

    void depositIntoAccount(int atmId,int accountId,int newBalance,int amount);

    void withdrawBalanceLow(int atmId,int accountId,int amount);

    void withdrawSuccess(int atmId,int accountId,int newBalance, int amount);

    void printBalance(int atmId,int accountId,int balance);

    void closeAccount(int atmId,int accountId,int balance);

    void transactionSuccess(int atmId,int amount,int sourceAcc,int targetAcc,int sourceBalanceNew,int targetBalanceNew);

    void commissionPrint(int commissionPrecentage,int bankGain,int accountID);

    void safeWrite(string toWrite);
};

#endif //LOGGER_H
