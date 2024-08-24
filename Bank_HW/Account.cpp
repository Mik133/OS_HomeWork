//
// Created by os on 27/07/24.
//
#include "Account.h"

#include <cmath>
#include <utility>

Account::Account(int accountId, int password, int balance):
    accountId(accountId),password(password),balance(balance) {
    pthread_mutex_init(&accMutex,NULL);
}

void Account::lockAccount() {
   if(pthread_mutex_lock(&accMutex) != 0) {
       cerr << "Bank error: pthread_mutex_lock failed\n";
       exit(1);
   }
}

void Account::unlockAccount() {
    if(pthread_mutex_unlock(&accMutex) != 0) {
        cerr << "Bank error: pthread_mutex_lock failed\n";
        exit(1);
    }
}

void Account::accountDeposit(int amount) {
    lockAccount();
    this->balance += amount;
    unlockAccount();
}

bool Account::passwordCheck(int password) const {
    if(this->password == password) return true;
    return false;
}

bool Account::accountWithdraw(int amount) {
    lockAccount();
    if(amount > this->balance) {
        unlockAccount();
        return false;
    }
    else {
        this->balance-=amount;
        unlockAccount();
        return true;
    }
}

int Account::getBalance() const {
    return this->balance;
}

void Account::printAccountBalance(){
    cout << "Account " << accountId << ": Balance - " << balance << " $,Account Password - " << password << endl;
}

bool Account::transferOut(int amount) {
    if(balance > amount) {
        balance -= amount;
        return true;
    }
    return false;
}

void Account::transferIn(int amount) {
    balance += amount;
}

int Account::chargeAccCommission(double commission) {
    int oldBalance = balance;
    double balanceDouble = balance;
    balanceDouble -= commission;
    balance = round(balanceDouble);
    return oldBalance - balance;
}

Account::~Account() {
    if(pthread_mutex_destroy(&accMutex) != 0) {
        cerr << "Bank error: pthread_mutex_destroy failed\n";
        exit(1);
    }
}


