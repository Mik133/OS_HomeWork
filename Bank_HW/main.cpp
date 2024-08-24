#include <cmath>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "bank.h"
#include "Account.h"
#include "ATM.h"
#include "logger.h"
//Atm Struct
typedef struct {
    ATM atm;
    int atmId{};
    string fileName;
    bank* bankGlobal{};
}atmStruct;

typedef struct {
    bank* globalBank;
    int numOfAtm;
}bankAndAtmNum;

void* bankCommissionCharge(void *passedBankNUm) {
    bankAndAtmNum threadBankAtm = *(bankAndAtmNum*)passedBankNUm;
    int numOfAtm = threadBankAtm.numOfAtm;
    while(threadBankAtm.globalBank->getAtmOpStatus() != numOfAtm) {
        sleep(3);
        threadBankAtm.globalBank->chargeCommission();
    }
    return 0;
}

void* bankStatusPrinting(void *passedBankNUm) {
    bankAndAtmNum threadBankAtm = *(bankAndAtmNum*)passedBankNUm;
    int numOfAtm = threadBankAtm.numOfAtm;
    int miliSec = 1000;
    while (threadBankAtm.globalBank->getAtmOpStatus() != numOfAtm) {
        threadBankAtm.globalBank->bankStatusPrint();
        usleep(500*miliSec);
    }
    return 0;
}
//Funcs
void* runAtm(void* atmToRun) {
    atmStruct localAtm = *(atmStruct*) atmToRun;
    localAtm.atm.parseInput(localAtm.fileName,localAtm.bankGlobal);
    return 0;
}

int main(int argc, char* argv[]) {
    //Check args correctness:
    //1.Check number of args
    if(argc == 1) {
        cerr << "Bank error:illegal arguments\n";
        return 1;
    }
    //2.Check if atms exists
    else {
        FILE* atmFile;
        for(int i = 1;i < argc;i++) {
            atmFile = fopen(argv[i],"r");
            if(atmFile == NULL) {
                cerr << "Bank error:illegal arguments\n";
                return 1;
            }
            else {
                if(fclose(atmFile) != 0) {
                    cerr << "Bank error: fclose failed\n";
                    exit(1);
                }
            }
        }
    }
    //If we got here the input is legal
    //Let's Init the bank;
    bank* bankOfOs;
    bank bank;
    bankOfOs = &bank;
    //Now were sending all the data to the atm to execute
    int atmNumber = argc - 1;
    ATM* atms = new ATM[3];
    atmStruct* allAtms = new atmStruct[atmNumber];
    for(int i = 0;i < atmNumber;i++) {
        atms[i].atmInit(i + 1);
        atmStruct newAtm;
        newAtm.atm = atms[i];
        newAtm.atmId = i + 1;
        string tmpFileName(argv[i + 1]);
        newAtm.fileName = tmpFileName;
        newAtm.bankGlobal = bankOfOs;
        allAtms[i] = newAtm;
    }
    bankAndAtmNum localBankAndNum = {&bank,atmNumber};
    pthread_t* atmThreads = new pthread_t[atmNumber];
    pthread_t bankPrintTh;
    pthread_t bankCommissionTh;
    for(int i = 0;i < atmNumber;i++) {
        if(pthread_create(&atmThreads[i],NULL,runAtm,&allAtms[i]) != 0) {
            cerr << "Bank error: pthread_create failed\n";
            exit(1);
        }
    }
    if(pthread_create(&bankPrintTh, NULL, bankStatusPrinting, &localBankAndNum) != 0) {
        cerr << "Bank error: pthread_create failed\n";
        exit(1);
    }
    if(pthread_create(&bankCommissionTh,NULL,bankCommissionCharge,&localBankAndNum) != 0) {
        cerr << "Bank error: pthread_create failed\n";
        exit(1);
    }
    for(int i = 0;i < atmNumber;i++) {
        if(pthread_join(atmThreads[i],NULL) != 0) {
            cerr << "Bank error: pthread_create failed\n";
            exit(1);
        }
    }
    if(pthread_join(bankPrintTh, NULL) != 0) {
        cerr << "Bank error: pthread_create failed\n";
        exit(1);
    }
    if(pthread_join(bankCommissionTh,NULL) != 0) {
        cerr << "Bank error: pthread_create failed\n";
        exit(1);
    }
    delete[] atmThreads;
    delete[] allAtms;
    delete[] atms;
    return 0;
}
