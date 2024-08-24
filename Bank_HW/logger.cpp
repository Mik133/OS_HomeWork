//
// Created by os on 27/07/24.
//

#include "logger.h"

logger::logger() {
    pthread_mutex_init(&logMutex,NULL);
    logStream.open(logFileName);
    if(!logStream.is_open()) {
        cerr << "Bank error: fopen failed\n";
        exit(1);
    }
}


void logger::writeOpenAccount(int accountId, int password, int amount, int atmId) {
    string part1 = ":New account id is ";
    string part2 = " with password ";
    string part3 = " and initial balance ";
    string endLine = "\n";
    string toWrite = to_string(atmId) + part1 + to_string(accountId) + part2 + to_string(password) + part3 + to_string(amount) + endLine;
    this->safeWrite(toWrite);
}

void logger::openAccourErrAccountExists(int atmId) {
    string part1 = "Error ";
    string part2 = ": Your transaction failed – account with the same id exists\n";
    string toWrite = part1 + to_string(atmId) + part2;
    this->safeWrite(toWrite);
}

void logger::passwordIncorrect(int atmId, int accoundId) {
    string part1 = "Error ";
    string part2 = ": Your transaction failed – password for account id ";
    string part3 = " is incorrect\n";
    string toWrite = part1 + to_string(atmId) + part2 + to_string(accoundId) + part3;
    this->safeWrite(toWrite);
}

void logger::accountDoesNotExist(int atmId, int accountId) {
    string part1 = "Error ";
    string part2 = ": Your transaction failed – account id ";
    string part3 = " does not exist\n";
    string toWrite = part1 + to_string(atmId) + part2 + to_string(accountId) + part3;
    this->safeWrite(toWrite);
}

void logger::depositIntoAccount(int atmId, int accountId, int newBalance, int amount) {
    string part1 = ": Account ";
    string part2 = " new balance is ";
    string part3 = " after ";
    string part4 = " $ was deposited\n";
    string toWrite = to_string(atmId) + part1 + to_string(accountId) + part2 +
        to_string(newBalance) + part3 + to_string(amount) + part4;
    this->safeWrite(toWrite);;
}

void logger::withdrawBalanceLow(int atmId, int accountId, int amount) {
    string part1 = "Error ";
    string part2 = ": Your transaction failed – account id ";
    string part3 = " balance is lower than ";
    string endLine = "\n";
    string toWrite = part1 + to_string(atmId) + part2 + to_string(accountId) + part3 + to_string(amount) + endLine;
    this->safeWrite(toWrite);
}

void logger::withdrawSuccess(int atmId, int accountId, int newBalance, int amount) {
    string part1 = ": Account ";
    string part2 = " new balance is ";
    string part3 = " after ";
    string part4 = " $ was withdrawn\n";
    string toWrite = to_string(atmId) + part1 + to_string(accountId) + part2 +
        to_string(newBalance) + part3 + to_string(amount) + part4;
    this->safeWrite(toWrite);
}

void logger::printBalance(int atmId, int accountId, int balance) {
    string part1 = ": Account ";
    string part2 = " balance is ";
    string endLine = "\n";
    string toWrite = to_string(atmId) + part1 + to_string(accountId) + part2 + to_string(balance) + endLine;
    this->safeWrite(toWrite);
}

void logger::closeAccount(int atmId, int accountId, int balance) {
    string part1 = ": Account ";
    string part2 = " is now closed. Balance was ";
    string endLine = "\n";
    string toWrite = to_string(atmId) + part1 + to_string(accountId) + part2 + to_string(balance) + endLine;
    this->safeWrite(toWrite);
}

void logger::transactionSuccess(int atmId, int amount, int sourceAcc,
                                int targetAcc, int sourceBalanceNew, int targetBalanceNew) {
    string part1 = ": Transfer ";
    string part2 = " from account ";
    string part3 = " to account ";
    string part4 = " new account balance is ";
    string part5 = " new target account balance is ";
    string endLine = "\n";
    string toWrite = to_string(atmId) + part1 + to_string(amount) + part2 + to_string(sourceAcc)
                    + part3 +to_string(targetAcc) + part4 + to_string(sourceBalanceNew) + part5 +
                        to_string(targetBalanceNew) + endLine;
    this->safeWrite(toWrite);
}

void logger::commissionPrint(int commissionPrecentage, int bankGain, int accountID) {
    string part1 = "Bank: commissions of ";
    string part2 = " % were charged, bank gained ";
    string part3 = " from account ";
    string endLine = "\n";
    string toWrite = part1 + to_string(commissionPrecentage) + part2 + to_string(bankGain) +
                    part3 + to_string(accountID) + endLine;
    this->safeWrite(toWrite);
}

void logger::safeWrite(string toWrite) {
    if(pthread_mutex_lock(&logMutex) != 0) {
        cerr << "Bank error: pthread_mutex_lock failed\n";
        exit(1);
    }
    logStream << toWrite;
    if(pthread_mutex_unlock(&logMutex) != 0) {
        cerr << "Bank error: pthread_mutex_unlock failed\n";
        exit(1);
    }
}

logger::~logger() {
    if(pthread_mutex_destroy(&logMutex) != 0) {
        cerr << "Bank error: pthread_mutex_destroy failed\n";
        exit(1);
    }
}




