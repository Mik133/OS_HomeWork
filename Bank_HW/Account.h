//
// Created by os on 27/07/24.
//

#ifndef ACCOUNT_H
#define ACCOUNT_H
//Libs
#include <string>
#include <iostream>
//NamsSpaces
using std::string;
using std::cout;
using std::endl;
using std::cerr;
//Class Account
class Account {
private:
    int accountId;
    int password;
    int balance;
    pthread_mutex_t accMutex;

public:
    Account(int accountId,int password,int balance);

    ~Account();

    void lockAccount();

    void unlockAccount();

    void accountDeposit(int amount);

    bool passwordCheck(int password) const;

    bool accountWithdraw(int amount);

    int getBalance() const;

    void printAccountBalance();

    bool transferOut(int amount);

    void transferIn(int amount);

    int chargeAccCommission(double commission);

};
#endif //ACCOUNT_H
