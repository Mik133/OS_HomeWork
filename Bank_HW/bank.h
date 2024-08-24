//
// Created by os on 27/07/24.
//

#ifndef BANK_H
#define BANK_H

//Libraries
#include <iostream>
#include <map>
#include "Account.h"
#include "logger.h"

//Namespaces
using std::cout;
using std::endl;
using std::map;
//Bank class
class bank {
private:
    map<int,Account*> allAccounts;//ID,Acc
    string bankName;
    pthread_mutex_t bankMutex;
    logger bankLogger;
    int atmOperationsDone;
public:
    bank();

    ~bank();

    void openNewAccount(int accountId,int password,int initialAmount,int atmId);

    void depositIntoAccount(int accountId,int password,int amount,int atmID);

    void withdrawFromAccount(int accountId,int password,int amount, int atmId);

    void checkBalance(int accountId,int password,int atmId);

    void closeAccount(int accountId, int password,int atmId);

    void transferMoney(int sourceAcc,int password,int targetAcc,int amount,int atmId);

    void bankStatusPrint();

    void atmSetDone();

    int getAtmOpStatus() const;

    void atmOpStatusIncrease();

    void chargeCommission();
};


#endif //BANK_H
