//
// Created by os on 27/07/24.
//
#include "bank.h"

bank::bank() {
    bankName = "The Global OS Bank\n";
    atmOperationsDone = 0;
    pthread_mutex_init(&bankMutex,NULL);
}

bank::~bank() {
    if(pthread_mutex_destroy(&bankMutex) != 0) {
        cerr << "Bank error: pthread_mutex_destroy failed\n";
        exit(1);
    }
}


void bank::openNewAccount(int accountId, int password, int initialAmount,int atmId) {
    if(allAccounts.find(accountId) == allAccounts.end()) {
        if(pthread_mutex_lock(&bankMutex) != 0) {
            cerr << "Bank error: pthread_mutex_lock failed\n";
            exit(1);
        }
        Account* newAccount = new Account(accountId,password,initialAmount);
        allAccounts.insert({accountId,newAccount});
        bankLogger.writeOpenAccount(accountId,password,initialAmount,atmId);
        if(pthread_mutex_unlock(&bankMutex) != 0) {
            cerr << "Bank error: pthread_mutex_unlock failed\n";
            exit(1);
        }
    }
    else {
        bankLogger.openAccourErrAccountExists(atmId);
    }
}

void bank::depositIntoAccount(int accountId, int password, int amount, int atmId) {
    if(allAccounts.find(accountId) != allAccounts.end()) {
        Account* accoutToDeposit = allAccounts[accountId];
        if(accoutToDeposit->passwordCheck(password)) {
            accoutToDeposit->accountDeposit(amount);
            bankLogger.depositIntoAccount(atmId,accountId,accoutToDeposit->getBalance(),amount);
        }
        else {
            bankLogger.passwordIncorrect(atmId,accountId);
        }
    }
    else {
        bankLogger.accountDoesNotExist(atmId,accountId);
    }
}

void bank::withdrawFromAccount(int accountId, int password, int amount, int atmId) {
    if(allAccounts.find(accountId) != allAccounts.end()) {
        Account* accountToWithdraw = allAccounts[accountId];
        if(accountToWithdraw->passwordCheck(password)) {
            if(accountToWithdraw->accountWithdraw(amount)) {
                bankLogger.withdrawSuccess(atmId,accountId,accountToWithdraw->getBalance(),amount);
            }
            else {
                bankLogger.withdrawBalanceLow(atmId,accountId,amount);
            }
        }
        else {
            bankLogger.passwordIncorrect(atmId,accountId);
        }
    }
    else {
        bankLogger.accountDoesNotExist(atmId,accountId);
    }
}

void bank::checkBalance(int accountId, int password, int atmId) {
    if(allAccounts.find(accountId) != allAccounts.end()) {
        Account* accountCheckBalance = allAccounts[accountId];
        if(accountCheckBalance->passwordCheck(password)) {
            bankLogger.printBalance(atmId,accountId,accountCheckBalance->getBalance());
        }
        else {
            bankLogger.passwordIncorrect(atmId,accountId);
        }
    }
    else {
        bankLogger.accountDoesNotExist(atmId,accountId);
    }
}

void bank::closeAccount(int accountId, int password, int atmId) {
    if(allAccounts.find(accountId) != allAccounts.end())  {
        Account* accountToClose = allAccounts[accountId];
        if(accountToClose->passwordCheck(password)) {
            if(pthread_mutex_lock(&bankMutex) != 0) {
                cerr << "Bank error: pthread_mutex_lock failed\n";
                exit(1);
            }
            int balanceOfClosed = accountToClose->getBalance();
            allAccounts.erase(accountId);
            delete accountToClose;
            bankLogger.closeAccount(atmId,accountId,balanceOfClosed);
            if(pthread_mutex_unlock(&bankMutex) != 0) {
                cerr << "Bank error: pthread_mutex_unlock failed\n";
                exit(1);
            }
        }
        else {
            bankLogger.passwordIncorrect(atmId,accountId);
        }
    }
    else {
        bankLogger.accountDoesNotExist(atmId,accountId);
    }
}

void bank::transferMoney(int sourceAcc, int password, int targetAcc, int amount, int atmId) {
    if(allAccounts.find(sourceAcc) != allAccounts.end()&&
        allAccounts.find(targetAcc) != allAccounts.end()) {
        Account* sourceAccB = allAccounts[sourceAcc];
        if(sourceAccB->passwordCheck(password)) {
            Account* targetAccB = allAccounts[targetAcc];
            if(sourceAcc < targetAcc) {
                sourceAccB->lockAccount();
                targetAccB->lockAccount();
            }
            else {
                targetAccB->lockAccount();
                sourceAccB->lockAccount();
            }
            if(sourceAccB->transferOut(amount)) {
                targetAccB->transferIn(amount);
                if(sourceAcc < targetAcc) {
                    targetAccB->unlockAccount();
                    sourceAccB->unlockAccount();
                }
                else {
                    targetAccB->unlockAccount();
                    sourceAccB->unlockAccount();
                }
                bankLogger.transactionSuccess(atmId,amount,sourceAcc,targetAcc,sourceAccB->getBalance(),targetAccB->getBalance());
                return;
            }
            bankLogger.withdrawBalanceLow(atmId,sourceAcc,amount);
            if(sourceAcc < targetAcc) {
                sourceAccB->unlockAccount();
                targetAccB->unlockAccount();
            }
            else {
                targetAccB->unlockAccount();
                sourceAccB->unlockAccount();
            }
        }
        else {
            bankLogger.passwordIncorrect(atmId,sourceAcc);
        }
    }
    else {
        if(allAccounts.find(sourceAcc) == allAccounts.end()) {
            bankLogger.accountDoesNotExist(atmId,sourceAcc);
       }
        if(allAccounts.find(targetAcc) == allAccounts.end()){
            bankLogger.accountDoesNotExist(atmId,targetAcc);
        }
    }
}

void bank::bankStatusPrint() {
    if(pthread_mutex_lock(&bankMutex) != 0) {
        cerr << "Bank error: pthread_mutex_lock failed\n";
        exit(1);
    }
    printf("\033[2J");
    printf("\033[1;1H");
    for(auto itrAccounts = allAccounts.begin();itrAccounts != allAccounts.end();itrAccounts++) {
        itrAccounts->second->lockAccount();
        itrAccounts->second->printAccountBalance();
        itrAccounts->second->unlockAccount();
    }
    if(pthread_mutex_unlock(&bankMutex) != 0) {
        cerr << "Bank error: pthread_mutex_unlock failed\n";
        exit(1);
    }
}


void bank::chargeCommission() {
    if(pthread_mutex_lock(&bankMutex) != 0) {
        cerr << "Bank error: pthread_mutex_lock failed\n";
        exit(1);
    }
    int commission = rand();
    commission %= 5;
    commission++;
    double commisionPr = commission * 0.01;
    double currentBalance;
    double commissionAmount;
    int bankGain;
    for(auto itrAccounts = allAccounts.begin();itrAccounts != allAccounts.end();itrAccounts++) {
        itrAccounts->second->lockAccount();
        currentBalance = itrAccounts->second->getBalance();
        commissionAmount = commisionPr * currentBalance;
        bankGain = itrAccounts->second->chargeAccCommission(commissionAmount);
        bankLogger.commissionPrint(commission,bankGain,itrAccounts->first);
        itrAccounts->second->unlockAccount();
    }
    if(pthread_mutex_unlock(&bankMutex) != 0) {
        cerr << "Bank error: pthread_mutex_unlock failed\n";
        exit(1);
    }
}

void bank::atmSetDone() {
    atmOperationsDone = true;
}

int bank::getAtmOpStatus() const {
    return atmOperationsDone;
}

void bank::atmOpStatusIncrease() {
    if(pthread_mutex_lock(&bankMutex) != 0) {
        cerr << "Bank error: pthread_mutex_lock failed\n";
        exit(1);
    }
    atmOperationsDone++;
    if(pthread_mutex_unlock(&bankMutex) != 0) {
        cerr << "Bank error: pthread_mutex_unlock failed\n";
        exit(1);
    }
}

