/*模拟使用银行家算法进行资源分配的程序*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NumOfResource 3
#define MaxProcessNum 20

struct ProcessControlBlock {
    int PID;        //进程标识符
    int prio;       //进程优先数
    int state;      //进程状态
    struct ProcessControlBlock *next;
};

struct ProcessControlBlock *ProcessList = NULL;     //创建进程链表
int AVAILABLE[NumOfResource] = {0};                 //每个资源可用实例数量
int MAX[MaxProcessNum][NumOfResource] = {0};        //每个进程最大资源需求
int ALLOCATION[MaxProcessNum][NumOfResource] = {0}; //进程已经占用的资源的实例数量
int NEED[MaxProcessNum][NumOfResource] = {0};       //进程还需要的资源实例数量
int ProcessNum = 0;

int FINISH[MaxProcessNum] = {0};
int WORK[NumOfResource] = {0};

struct ProcessControlBlock* PIDSearch(int pid);
int GetProcessID();
int SecurityLock(int pid, int Res,int Num);
int CreateNewProcess();
void AddProcessToList(struct ProcessControlBlock *new);
void PrintList(int div);
void InitResource();
void InitProcessResource(int PID,int ResNo,int Num);
void PrintList(int div);
void UseRes(int pid, int Res,int Num);
void IfCompelete(int pid);
void KillProcess(int PID);

void help() {
        printf("************************************\n");
        printf("\033[33m*用法：[可省略]{可以有多个接在后面}*\n\n");
        printf("创建一个新的进程并声明它将要占用的最大资源数量：\n");
        printf("cp {[占用资源编号] [占用资源数量]} \n\n");
        printf("查看当前所有进程最高占用|正在占用|资源剩余数量：\n");
        printf("list (max|all|res)\n\n");
        printf("为存在的进程补充新的将要占用的最大资源数量：\n");
        printf("cpr (进程ID) {(占用资源编号) (占用资源数量)}\n\n");
        printf("让活动中的程序开始使用资源：\n");
        printf("use (进程ID) {(资源编号) (资源数量)}\033[0m\n");
    }

int main() {
    InitResource();
    printf("************************************\n");
    printf("*模拟银行家算法预防死锁程序，启动！*\n");
    help();
    printf("\n你可以随时输入help来查看这些用法\n");
    printf("************************************\n");
    PrintList(3);

    while(1){
    printf("************************************\n/");

    char input[50] = {'\0'};
    scanf("%s",&input);
    
    /*创建进程*/
    if (!strcmp(input,"cp")) {
        system("cls");
        int pid,ResNo,Num;
        bool crea = true;
        while(getchar() != '\n') {
            scanf("%d %d",&ResNo,&Num);
            // printf("资源%d 数量%d\n",ResNo,Num);
            if (crea) {
                pid = CreateNewProcess();
                crea = false;
            }
            InitProcessResource(pid,ResNo,Num);
        }
        printf("完成,新进程的ID为%d\n",pid);
        PrintList(1);
        PrintList(2);
    }

    /*查看进程和占用资源*/
    else if (!strcmp(input,"list")) {
        system("cls");
        char div[5];
        scanf("%s",&div);
        // printf("输入%s\n",div);
        if (!strcmp(div,"max")) PrintList(1);
        else if (!strcmp(div,"all")) PrintList(2);
        else if (!strcmp(div,"res")) PrintList(3);
    }

    /*设定新的最大占用*/
    else if (!strcmp(input,"cpr")) {
        system("cls");
        int pid,ResNo,Num;
        while(getchar() != '\n') {
            scanf("%d %d %d",&pid,&ResNo,&Num);
            // printf("进程id%d 资源%d 数量%d\n",pid,ResNo,Num);
            InitProcessResource(pid,ResNo,Num);
        }
        // printf("完成\n");
        PrintList(1);
        PrintList(2);
    }

    /*开始占用*/
    else if (!strcmp(input,"use")) {
        system("cls");
        int pid,ResNo,Num;
        scanf("%d",&pid);
        while(getchar() != '\n') {
            scanf("%d %d",&ResNo,&Num);
            // printf("进程id%d 资源%d 数量%d\n",pid,ResNo,Num);
            UseRes(pid,ResNo,Num);
        }
        // printf("完成\n");
        PrintList(1);
        PrintList(2);
        PrintList(3);
    }

    else if (!strcmp(input,"clear")) {
        printf("GoodProcess,LoveFromXiao\n");
        system("cls");
    }

    /*帮助*/
    else if (!strcmp(input,"help")) help();
    }
    system("pause");
}

/*初始化资源数量和进程链表*/
void InitResource() {
    /*默认第i个资源拥有i个实例*/
    for (int i = 0; i < NumOfResource; i++) {
        AVAILABLE[i] = i+1;
    }
    ProcessList = (struct ProcessControlBlock*)malloc(sizeof(struct ProcessControlBlock));
    ProcessList->next = NULL;
}

/*初始化进程所需资源*/
void InitProcessResource(int PID,int ResNo,int Num) {
    MAX[PID-1][ResNo-1] = Num;
    NEED[PID-1][ResNo-1] = MAX[PID-1][ResNo-1];
}
// void InitProcessResource(struct ProcessControlBlock *Process,int ResNo,int Num) {
//     MAX[Process->PID][ResNo] = Num;
// }

/*创建进程*/
int CreateNewProcess() {
    struct ProcessControlBlock *p,*p0;
    p = (struct ProcessControlBlock*)malloc(sizeof(struct ProcessControlBlock));
    p->PID = GetProcessID();
    p->next = NULL;
    AddProcessToList(p);
    ProcessNum += 1;
    return p->PID;
}

/*为进程分配id*/
int GetProcessID() {
    int pid = 1;
    while(PIDSearch(pid)) pid++;
    return pid;
}

/*将进程添加至进程列表*/
void AddProcessToList(struct ProcessControlBlock *new) {
    struct ProcessControlBlock *p,*p0;
    p = p0 = ProcessList;
    while(p) {
        p0 = p;
        p = p->next;
    }
    p = new;
    p0->next = p;
}

/*通过pid搜索进程*/
struct ProcessControlBlock* PIDSearch(int pid) {
    struct ProcessControlBlock *p;
    p = ProcessList->next;
    while(p){
        if(p->PID == pid) break;
        else p = p->next;
    }
    return p;
}

void PrintList(int div) {
    struct ProcessControlBlock *p;
    switch (div) {
        /*max*/
        case 1:{
            printf("************************************\n");
            printf("进程最大占用：\n");
            p = ProcessList->next;
            printf("进程ID\t");
            for(int i = 1; i < NumOfResource+1; i++) printf("资源%d\t",i);
            printf("\n");
            while(p) {
                printf("  %d\t",p->PID);
                for(int i = 0; i < NumOfResource; i++) printf("  %d\t",MAX[p->PID-1][i]);
                printf("\n");
                p = p->next;
            }
            break;
        }
        /*all*/
        case 2:{
            printf("************************************\n");
            printf("进程已经占用：\n");
            p = ProcessList->next;
            printf("进程ID\t");
            for(int i = 1; i < NumOfResource+1; i++) printf("资源%d\t",i);
            printf("\n");
            while(p) {
                printf("  %d\t",p->PID);
                for(int i = 0; i < NumOfResource; i++) printf("  %d\t",ALLOCATION[p->PID-1][i]);
                printf("\n");
                p = p->next;
            }
            break;
        }
        /*res*/
        case 3:{
            printf("************************************\n");
            printf("当前资源剩余量：\n");
            for(int i = 1; i < NumOfResource+1; i++) printf("资源%d\t",i);
            printf("\n");
            for(int i = 0; i < NumOfResource; i++) printf("  %d\t",AVAILABLE[i]);
            printf("\n");
            break;
        }
    }
}

void UseRes(int pid, int Res,int Num) {
    if (Num <= NEED[pid-1][Res-1]) {
        if (Num <= AVAILABLE[Res-1]) {
            ALLOCATION[pid-1][Res-1] += Num;
            AVAILABLE[Res-1] -= Num;
            NEED[pid-1][Res-1] = MAX[pid-1][Res-1] - ALLOCATION[pid-1][Res-1];
        } else {
            printf("资源余量不足，资源将在充足时分配给改进程\n");
            return;
        }
        
        if (!SecurityLock(pid,Res,Num)) {
                printf("申请不通过，不安全！\n");
                return;
            } else printf("资源请求成功！\n");
        IfCompelete(pid);
    }
    else {
        printf("请求量不得大于声明的最大需求量！\n");
    }
}

int SecurityLock(int pid, int Res,int Num) {
    int seculist[ProcessNum];
    int seculist_i;
    for (int i = 0; i < NumOfResource; i++) WORK[i] = AVAILABLE[i];
    for (int i = 0; i < MaxProcessNum; i++) FINISH[i] = 0;
    int sec = 1;
    // int rem[NumOfResource];
    int rem;
    for (seculist_i = 0; seculist_i < ProcessNum; seculist_i++) {
        for (int i = 0; i < ProcessNum; i++) {
            sec = 1;
            // rem_i = 0;
            for (int j = 0; j < NumOfResource; j++) {
                if (FINISH[i] == 0 && NEED[i][j] <= WORK[j]) {
                    WORK[j] += ALLOCATION[i][j];
                    // rem[rem_i] = j;
                    rem = j;
                    // rem_i += 1;
                    printf("\033[2;33m进程:%d 资源:%d,需求:%d,余量:%d\033[0m\n",i+1,j+1,NEED[i][j],WORK[j]);
                } else if (NEED[i][j] > WORK[j]) {
                    sec = 0;
                    printf("\033[2;33m[无法运行]进程:%d 资源:%d,需求:%d,余量:%d\033[0m\n",i+1,j+1,NEED[i][j],WORK[j]);
                    // for (rem_i = 0; rem_i < NumOfResource && rem[rem_i] < NumOfResource; rem_i++) WORK[rem[rem_i]] -= ALLOCATION[i][rem[rem_i]];
                    WORK[rem] -= ALLOCATION[i][rem];
                    break;
                } else if (FINISH[i] == 1) {
                    sec = 0;
                    printf("\033[2;33m[跳过]进程:%d 资源:%d,需求:%d,余量:%d\033[0m\n",i+1,j+1,NEED[i][j],WORK[j]);
                }
            }
            if(sec) {
                FINISH[i] = 1;
                seculist[seculist_i] = i+1;
                break;
            }
        }
    }
    
    for (int i = 0; i < ProcessNum; i++) {
        if (FINISH[i] == 0) sec = 0;
    }

    if (sec) {
        printf("当前安全序列：");
        int i = 0;
        for (seculist_i = 0; seculist_i < ProcessNum; seculist_i++) {
            if(i) printf("->");
            printf("%d",seculist[seculist_i]);
            if(!i) i = 1;
        }
        printf("\n");
        return 1;
    } else {
        NEED[pid-1][Res-1] += Num;
        ALLOCATION[pid-1][Res-1] -= Num;
        AVAILABLE[Res-1] += Num;
        return 0;
    }
}

void IfCompelete(int pid) {
    bool comp = true;
    for (int j = 0; j < NumOfResource; j++) {
        if (MAX[pid-1][j] != ALLOCATION[pid-1][j]) comp = false;
    }
    if (comp) {
        printf("该进程已取得所有应得的资源，运行完成并释放资源\n");
        KillProcess(pid);
    }
}

void KillProcess(int PID) {
    // struct ProcessControlBlock *p,*p0;
    // p0 = ProcessList;
    // p = PIDSearch(PID);
    // while(p0->next != p) p0 = p0->next;
    // p0->next = p->next;
    for (int j = 0; j < NumOfResource; j++) {
        AVAILABLE[j] += ALLOCATION[PID-1][j];
        MAX[PID-1][j] = 0;
        ALLOCATION[PID-1][j] = 0;
        NEED[PID-1][j] = 0;
    }
    // free(p);
    // ProcessNum -= 1;
}
