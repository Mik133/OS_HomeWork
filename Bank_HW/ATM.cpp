//
// Created by os on 27/07/24.
//
#include "ATM.h"

#include <string.h>

void ATM::parseInput(const string& fileName,bank* globalBank) const {
    const int microSec = 1000;
    //Vars for parsing
    ifstream atmFile(fileName);
    string atmOperation;
    string operation;
    string tmpString;
    int accountId;
    int password;
    int amount;
    int sourceAccount;
    int targetAccount;
    //End of vars for parsing
    if(atmFile.is_open()) {
        while(getline(atmFile,atmOperation)) {
            usleep(100*microSec);//100 milisec sleep
            //Get command type
            stringstream inputStream(atmOperation);
            inputStream >> operation;
            //Continue execution according to given operation
            if(operation == OPEN_ACCOUNT) {
                inputStream >> tmpString;
                accountId = stoi(tmpString);
                inputStream >> tmpString;
                password = stoi(tmpString);
                inputStream >> tmpString;
                amount = stoi(tmpString);
                globalBank->openNewAccount(accountId,password,amount,atmId);
            }
            else if(operation == DEPOSIT) {
                inputStream >> tmpString;
                accountId = stoi(tmpString);
                inputStream >> tmpString;
                password = stoi(tmpString);
                inputStream >> tmpString;
                amount = stoi(tmpString);
                globalBank->depositIntoAccount(accountId,password,amount,atmId);
            }
            else if(operation == WITHDRAWAL) {
                inputStream >> tmpString;
                accountId = stoi(tmpString);
                inputStream >> tmpString;
                password = stoi(tmpString);
                inputStream >> tmpString;
                amount = stoi(tmpString);
                globalBank->withdrawFromAccount(accountId,password,amount,atmId);
            }
            else if(operation == BALANCE_CHECK) {
                inputStream >> tmpString;
                accountId = stoi(tmpString);
                inputStream >> tmpString;
                password = stoi(tmpString);
                globalBank->checkBalance(accountId,password,atmId);
            }
            else if(operation == CLOSE_ACCOUNT){
                inputStream >> tmpString;
                accountId = stoi(tmpString);
                inputStream >> tmpString;
                password = stoi(tmpString);
                globalBank->closeAccount(accountId,password,atmId);
            }
            else if(operation == TRANSFER) {
                inputStream >> tmpString;
                sourceAccount = stoi(tmpString);
                inputStream >> tmpString;
                password = stoi(tmpString);
                inputStream >> tmpString;
                targetAccount = stoi(tmpString);
                inputStream >> tmpString;
                amount = stoi(tmpString);
                globalBank->transferMoney(sourceAccount,password,targetAccount,amount,atmId);
            }
            else {
                cerr << "Bank error: unrecognized operation\n";
                exit(1);
            }
            sleep(1);//SLEEP FOR 1 SECOND
        }
    }
    else {
        cerr << "Bank error: illegal arguments\n";
        exit(1);
    }
    globalBank->atmOpStatusIncrease();
}


