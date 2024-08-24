#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <map>
#include <ctime>
#include <csignal>
#include <cstdlib>
#include <fstream>
//Namespace
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::map;
using std::flush;
using std::string;
using std::getline;
//Const's
const int MAX_LINE_SIZE = 80;
const int MAX_ARGS = 20;
enum {SHOW_PID = 701,PWD = 702,CD = 703,JOBS = 704,KILL = 705,
      FG = 706,BG = 707,DIFF = 708,QUIT = 777,EXTERNAL_CMD = 800,
      DEFAULT = -111};
enum {IS_FG = 501,IS_BG = 500};
enum {STOPPED = 401,NOT_STOPPED = 400};
enum {FG_PROCESS_DEFAULT = 200};
enum {DIFF_FOUND = 1,NO_DIFF = 0};
const size_t MAX_PATH = 80;
const int LAST_DIR_BUFFER = 80;
const char* defaultDir = "NODIR";
//Globals 1
pid_t fgProcess = FG_PROCESS_DEFAULT;
char fgCmd[MAX_LINE_SIZE] = "DEFAULT_FG";
//Functions
int cmdParser(char* cmdType);//will check if the cmd build in and which one or if external
void cmdBuiltInExec(int cmdTypeId,char* cmdArgs[MAX_ARGS],char* lastDir);
void cmdArgsClear(char* cmdArgs[MAX_ARGS]);
void jobsUpdate();
//Signal handling function
void handleSigstop(int signum);
void handleSigkill(int signum);
//Jobs class
class job_c {
private:
    int jobId;
    char* cmdName;
    pid_t processId;
    time_t timeOfStart;
    int isStopped;
public:
    job_c(int jobId,const char* cmdName,pid_t processId) : jobId(jobId),processId(processId) {
        this->cmdName = new char[strlen(cmdName) + 1];
        strcpy(this->cmdName,cmdName);
        timeOfStart = time(NULL);
        isStopped = NOT_STOPPED;
    }
    void printProcess() {
        cout << "[" << jobId << "] " << this->cmdName << " : " << this->processId << " " <<
            difftime(time(NULL),timeOfStart) << " secs ";
        if(isStopped == STOPPED) cout << "(stopped)\n";
        else cout << endl;
    }

    pid_t getProcessId() const{return this->processId;}

    void setStopped(int isStopped){this -> isStopped=isStopped;}

    int getStopped() const{return this->isStopped;}

    char* getCmdName(){return cmdName;}
};

//Globals 2
map<int,job_c> allJobs;//<job id,job data>

int main() {
    //Base variables
    char inputCmd[MAX_LINE_SIZE];
    char inputCmdCopy[MAX_LINE_SIZE];
    char* cmdNameJob;
    char* cmdArgs[MAX_ARGS];
    char* cmdToken;
    int indexCmd = 1;
    pid_t pid = -1; //LATER
    int cmdId = DEFAULT;
    char lastDir[LAST_DIR_BUFFER] = "NODIR";
    int isCmdFgBg = DEFAULT;
    int newJobId;
    int setgrpVal;
    int killVal;
    fgProcess = FG_PROCESS_DEFAULT;
    //Signal handling 1:SIGSTOP
    struct sigaction saSigstop;
    saSigstop.sa_handler = &handleSigstop;
    sigemptyset(&saSigstop.sa_mask);
    saSigstop.sa_flags = 0;
    //Signal handling 2:SIGKILL
    struct sigaction saSigkill;
    saSigkill.sa_handler = &handleSigkill;
    sigemptyset(&saSigkill.sa_mask);
    saSigkill.sa_flags = 0;
    while(true) {
        sigaction(SIGTSTP,&saSigstop,NULL);
        sigaction(SIGINT,&saSigkill,NULL);
        cmdArgsClear(cmdArgs);
        jobsUpdate();
        cout << "smash > ";
        fgets(inputCmd,MAX_LINE_SIZE,stdin);
        strcpy(inputCmdCopy,inputCmd);
        if((strcmp(inputCmd,"\n")) == 0) continue;
        cmdToken = strtok(inputCmd," \n\t");
        char* cmdType;
        if(cmdToken != NULL)
            cmdType = new char[strlen(cmdToken) - 1];
        else continue;
        strcpy(cmdType,cmdToken);
        cmdArgs[0] = cmdType;
        while(cmdToken != NULL) {
            cmdArgs[indexCmd] = strtok(NULL," \t\n");
            if(cmdArgs[indexCmd] == NULL) break;
            indexCmd+=1;
        }
        cmdId = cmdParser(cmdType);
        if(cmdId != EXTERNAL_CMD) {
            cmdBuiltInExec(cmdId,cmdArgs,lastDir);
        }
        else {
            char* lastArg = new char[strlen((cmdArgs[indexCmd - 1])) -1];
            strcpy(lastArg,cmdArgs[indexCmd - 1]);
            if(lastArg[strlen(lastArg) - 1] == '&') {
                cmdArgs[indexCmd - 1] = strtok(cmdArgs[indexCmd - 1],"&");
                isCmdFgBg = IS_BG;
            }
            else if(strcmp(cmdArgs[indexCmd - 1],"&") == 0) {
                isCmdFgBg = IS_BG;
                cmdArgs[indexCmd - 1] = NULL;
            }
            else {
                isCmdFgBg = IS_FG;
            }
            delete lastArg;
            pid = fork();
            if(pid == -1) {
                cerr << "smash error: fork failed\n";
                delete cmdType;
                return 1;
            }
            if(pid == 0) {
                setgrpVal = setpgrp();
                if(setgrpVal == -1) {
                    cerr << "smash error: setpgrp failed\n";
                }
                execvp(cmdType,cmdArgs);
                cerr << "smash error: exec failed\n";
                delete cmdType;
                killVal = kill(getpid(),SIGKILL);
                if(killVal == -1) {
                    cerr << "smash error: kill failed\n";
                }
            }
            else{
                cmdNameJob = strtok(inputCmdCopy,"\n");
                if(isCmdFgBg == IS_FG) {
                    fgProcess = pid;
                    strcpy(fgCmd,cmdNameJob);
                    if(waitpid(pid,NULL,0) == -1) {
                        cerr << "smash error: waitpid failed\n";
                    }
                    fgProcess = FG_PROCESS_DEFAULT;
                    strcpy(inputCmd,"\n");
                }
                else {
                    jobsUpdate();
                    cmdNameJob = strtok(inputCmdCopy,"\n");
                    if(allJobs.empty()) {
                        job_c new_job(1,cmdNameJob,pid);
                        allJobs.insert({1,new_job});
                    }
                    else {
                        newJobId = allJobs.rbegin()->first + 1;
                        job_c new_job(newJobId,cmdNameJob,pid);
                        allJobs.insert({newJobId,new_job});
                    }
                }
                delete cmdType;
            }
        }

        indexCmd = 1;
    }
    return 0;
}

int cmdParser(char* cmdType) {
    if(strcmp(cmdType,"showpid") == 0) return SHOW_PID;
    else if(strcmp(cmdType,"pwd") == 0) return PWD;
    else if(strcmp(cmdType,"cd") == 0) return CD;
    else if(strcmp(cmdType,"jobs") == 0) return JOBS;
    else if(strcmp(cmdType,"kill") == 0) return KILL;
    else if(strcmp(cmdType,"fg") == 0) return FG;
    else if(strcmp(cmdType,"bg") == 0) return BG;
    else if(strcmp(cmdType,"quit") == 0) return QUIT;
    else if(strcmp(cmdType,"diff") == 0) return DIFF;
    else return EXTERNAL_CMD;
}

void cmdBuiltInExec(int cmdTypeId,char* cmdArgs[MAX_ARGS],char* lastDir) {
    if(cmdTypeId == SHOW_PID) {
        cerr << "smash pid is " << getpid() << endl;
    }
    else if(cmdTypeId == PWD) {
        char* cwdPath = new char[sizeof(char) * MAX_PATH];
        getcwd(cwdPath,MAX_PATH);
        if(cwdPath == NULL)
            cerr << "smash error: getcwd failed\n";
        cout << cwdPath << endl;
        delete cwdPath;
    }
    else if(cmdTypeId == CD) {
        int argsNum = 0;
        while(cmdArgs[argsNum] != NULL) {
            argsNum++;
        }
        if(argsNum > 2) {
            cerr << "smash error: cd: too many arguments\n";
            return ;
        }
        else if(argsNum == 1) {
            getcwd(lastDir,MAX_PATH);
        }
        else if(strcmp(cmdArgs[1],"-") == 0) {
            if(strcmp(lastDir,defaultDir) == 0) cerr << "smash: error: cd: OLDPWD not set\n";
            else {
                if(chdir(lastDir) == -1) {
                    cerr << "smash error: chdir failed\n";
                    return;
                }
            }
        }
        else {
            char* cwdPath = new char[sizeof(char) * MAX_PATH];
            getcwd(cwdPath,MAX_PATH);
            strcpy(lastDir,cwdPath);
            if(chdir(cmdArgs[1]) == -1) {
                cerr << "smash error: chdir failed\n";
            }
            delete cwdPath;
        }
    }
    else if(cmdTypeId == JOBS) {
        jobsUpdate();
        for(auto jobItr = allJobs.begin();jobItr != allJobs.end();jobItr++) {
            jobItr->second.printProcess();
        }
    }
    else if(cmdTypeId == KILL) {
        jobsUpdate();
        if(cmdArgs[1] == NULL || strcmp(cmdArgs[1],"-") == 0 || cmdArgs[2] == NULL) {
            cerr << "smash error: kill: invalid arguments\n";
            return;
        }
        char* sigNum = strtok(cmdArgs[1],"-");
        int argsNum = 0;
        while(cmdArgs[argsNum] != NULL) argsNum++;
        int jobId = atoi(cmdArgs[2]);
        if(strcmp(sigNum,cmdArgs[1]) == 0 || argsNum > 3 || jobId < 1) {
            cerr << "smash error: kill: invalid arguments\n";
            return;
        }
        int signalNum = atoi(sigNum);
        auto processToSendSig = allJobs.find(jobId);
        if(processToSendSig == allJobs.end()) {
            cerr << "smash error: kill: job-id " << jobId << " does not exist\n";
            return;
        }
        pid_t processId = processToSendSig->second.getProcessId();
        if(kill(processId,signalNum) == -1) {
            cout << "smash error: kill failed\n";
            return;
        }
    }
    else if(cmdTypeId == FG) {
        int argsNum = 0;
        while(cmdArgs[argsNum] != NULL) argsNum++;
        if(argsNum == 1) {
            if(allJobs.empty()) {
                cerr << "smash error: fg: job list is empty\n";
                return;
            }
            else {
                auto maxJobId = allJobs.rbegin();
                pid_t maxJobPid = maxJobId->second.getProcessId();
                fgProcess = maxJobId->second.getProcessId();
                strcpy(fgCmd,maxJobId->second.getCmdName());
                cout << fgCmd << endl;
                allJobs.erase(maxJobId->first);
                if(kill(maxJobPid,SIGCONT) == -1) {
                    cout << "smash error: kill failed\n";
                    return;
                }
                if(waitpid(maxJobPid,NULL,0) == -1) {
                    cout << "smash error: waitpid failed\n";
                    return;
                }
            }
        }
        else if(argsNum > 2){
            cerr << "smash error: fg: invalid arguments\n";
            return;
        }
        else {
            int jobId = atoi(cmdArgs[1]);
            auto jobToRm = allJobs.find(jobId);
            if(jobToRm == allJobs.end()) {
                cerr << "smash error: fg: job-id " << jobId << " does not exist\n";
                return;
            }
            pid_t jobToRmPid = jobToRm->second.getProcessId();
            allJobs.erase(jobId);
            fgProcess = jobToRmPid;
            strcpy(fgCmd,jobToRm->second.getCmdName());
            cout << fgCmd << endl;
            if(kill(jobToRmPid,SIGCONT) == -1) {
                cout << "smash error: kill failed\n";
                return;
            }
            if(waitpid(jobToRmPid,NULL,0) == -1) {
                cout << "smash error: waitpid failed\n";
                return;
            }
        }
    }
    else if(cmdTypeId == BG) {
        int argsNum = 0;
        while(cmdArgs[argsNum] != NULL) argsNum++;
        if(argsNum == 1) {
            if(allJobs.empty()) {
                cerr << "smash error: bg: there are no stopped jobs to resume\n";
                return;
            }
            for(auto jobItr = allJobs.rbegin();jobItr != allJobs.rend();jobItr++) {
                if(jobItr->second.getStopped() == NOT_STOPPED) {
                    if(jobItr->first == allJobs.begin()->first) {
                        cerr << "smash error: bg: there are no stopped jobs to resume\n";
                        return;
                    }
                }
                else {
                    pid_t pidToBg = jobItr->second.getProcessId();
                    if(kill(pidToBg,SIGCONT) == -1) {
                        cout << "smash error: kill failed\n";
                        return;
                    }
                    jobItr->second.setStopped(NOT_STOPPED);
                    cout << jobItr->second.getCmdName() << " : " << pidToBg << endl;
                }
            }
        }
        else if(argsNum == 2){
            int jobId = atoi(cmdArgs[1]);
            auto jobToBg = allJobs.find(jobId);
            if(jobToBg == allJobs.end()) {
                cerr << "smash error: bg: " << jobId << " does not exist\n";
                return;
            }
            if(jobToBg->second.getStopped() == NOT_STOPPED) {
                cerr << "smash error: bg: job-id " << jobToBg->first << " is already running in the background\n";
                return;
            }
            else if(jobId < 1 || jobId > 100) {
                cout << "smash error: bg: invalid arguments\n";
                return;
            }
            else {
                pid_t pidToBg = jobToBg->second.getProcessId();
                if(kill(pidToBg,SIGCONT) == -1) {
                    cout << "smash error: kill failed\n";
                    return;
                }
                allJobs.find(jobId)->second.setStopped(NOT_STOPPED);
                cout << jobToBg->second.getCmdName() << " : " << pidToBg << endl;
            }
        }
        else {
            cout << "smash error: bg: invalid arguments\n";
            return;
        }

    }
    else if(cmdTypeId == QUIT) {
        if(cmdArgs[1] == NULL) exit(0);
        else if(strcmp(cmdArgs[1],"kill") != 0) exit(0);
        else if(strcmp(cmdArgs[1],"kill") == 0) {
            pid_t jobPid;
            for(auto jobItr = allJobs.begin();jobItr != allJobs.end();jobItr++) {//Step 1:send sigterm
                jobPid = jobItr->second.getProcessId();
                cout << jobItr->second.getCmdName() << " - Sending SIGTERM..." << flush;
                if(kill(jobPid,SIGTERM) == -1) {
                    cout << "smash error: kill failed\n";
                    return;
                }
                sleep(5);
                jobsUpdate();
                if(allJobs.find(jobItr->first) == allJobs.end()) {
                    cout << " Done\n";
                }
                else {
                    signal(SIGKILL,SIG_DFL);
                    if(kill(jobPid,SIGKILL) == -1) {
                        cout << "smash error: kill failed\n";
                        return;
                    }
                    cout << "(5 sec passed) Sending SIGKILL... Done\n";
                }
            }
        exit(0);
        }
        else exit(0);
    }
    else if(cmdTypeId == DIFF) {
        int argsNum = 0;
        while(cmdArgs[argsNum] != NULL) argsNum++;
        if(argsNum > 3) {
            cerr << "smash error: diff: invalid arguments\n";
            return;
        }
        else {
            std::ifstream file1(cmdArgs[1]);
            std::ifstream file2(cmdArgs[2]);
            int diffCheck = NO_DIFF;
            if(!file1.is_open() || !file2.is_open()) {
                cerr << "smash error: diff: invalid arguments\n";
                return;
            }
            string file1data,file2data;
            while(getline(file1,file1data) && getline(file2,file2data)) {
                if(file1data != file2data) {
                    diffCheck = DIFF_FOUND;
                    cout << diffCheck << endl;
                    return;
                }
            }
            if(getline(file1,file1data) || getline(file2,file2data)){
                diffCheck = DIFF_FOUND;
            }
            cout << diffCheck << endl;
        }
    }
    else {
        cerr << "An error has occured\n";
        _exit(1);
    }
}

void cmdArgsClear(char* cmdArgs[MAX_ARGS]) {
    for(int i = 0;i < MAX_ARGS;i++)
        cmdArgs[i] = NULL;
}

void jobsUpdate() {
    pid_t jobStatus;
    for(auto jobItr = allJobs.begin();jobItr != allJobs.end();jobItr++) {
        jobStatus = waitpid(jobItr->second.getProcessId(),&jobStatus,WNOHANG);
        if(jobStatus > 0) allJobs.erase(jobItr->first);
    }
}

void handleSigstop(int signum) {
    cout << "smash: caught ctrl-Z\n";
    if(fgProcess != FG_PROCESS_DEFAULT) {
        cout << "smash: process " << fgProcess << " was stopped\n";
        if(allJobs.empty()) {
            job_c jobStopped(1,fgCmd,fgProcess);
            jobStopped.setStopped(STOPPED);
            allJobs.insert({1,jobStopped});
        }
        else {
            int newJobId = allJobs.rbegin()->first + 1;
            job_c jobStopped(newJobId,fgCmd,fgProcess);
            jobStopped.setStopped(STOPPED);
            allJobs.insert({newJobId,jobStopped});
        }
        if(kill(fgProcess,SIGSTOP) == -1) {
            cout << "smash error: kill failed\n";
            return;
        }
    }
}

void handleSigkill(int signum) {
    cout << "smash: caught ctrl-C\n";
    if(fgProcess != FG_PROCESS_DEFAULT) {
        cout << "smash: process " << fgProcess << " was killed\n";
        if(kill(fgProcess,SIGKILL) == -1) {
            cout << "smash error: kill failed\n";
            return;
        }
    }
}

