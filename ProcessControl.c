/*模拟使用时间片轮转+动态优先数方法的进程调度程序*/

#include <stdio.h>
#include <stdlib.h>

#define AllProcess 0
#define StateRunning 1
#define StateReady 2
#define StateWaiting 3
#define StateLocked 4

#define Detailed 1
#define Curt 2

struct node {
    int PID;        //进程标识符
    int prio;       //进程优先数
    int priolock;
    int round;      //进程时间片
    int cputime;    //占用cpu时间
    int needtime;   //到完成还需时间
    int count;      //计数器
    int state;      //进程状态
    struct node *next;
};

struct node *AllList = NULL;
int count = 0;
int Memory[10] = {1000};  //内存
int readymax = 5;   //最大就绪数
int round = 2;      //默认时间片
int infoSetting = Detailed;
int IfKill = 1;

struct node* PIDSearch(int pid) {
    struct node *p;
    p = AllList->next;
    while(p){
        if(p->PID == pid) break;
        else p = p->next;
    }
    return p;
}

int main() {
    printf("**************************************\n");
    printf("模拟进程调度系统，启动！(使用动态调整优先数的时间片轮转算法)\n");
    /*进程列表初始化*/
    AllList = (struct node*)malloc(sizeof(struct node));
    AllList->next = NULL;

    while(1) {
        printf("**************************************\n");
        printf("请输入以下序号操作：\n");
        printf("1.创建进程\t\t\t");
        printf("10.批量创建进程\n");
        printf("2.查看进程(当前共%d个进程)\t",count);
        printf("3.挂起进程\n");
        printf("4.唤醒进程\t\t\t");
        printf("5.销毁进程\n");
        printf("6.开始运行\n");
        printf("7.修改默认时间片(当前为%d)\n",round);
        printf("8.修改就绪列表长度(当前为%d)\n",readymax);
        printf("9.关闭系统\n");
        printf("**************************************\n");
        printf("请输入操作序号：");
        int input;
        scanf("%d",&input);
        printf("**************************************\n");

        switch (input) {
        /*创建进程*/
        case 1:{
            int prio = 0;
            int cputime = 0;
            printf("请输入新进程的初始优先级(1~9，数字越小优先级越高)：");
            scanf("%d",&prio);
            if(prio < 1 || prio > 9) {
                printf("使用默认优先级：5\n");
                prio = 5;
            }
            printf("请输入新进程需要运行的时长：");
            scanf("%d",&cputime);
            while(cputime == 0) {
                printf("时长无效，请重新输入：");
                scanf("%d",&cputime);
            }
            CreateAProcess(prio, round, cputime);
            printf("进程已添加。\n");
            break;
        }
        case 10:{
            int num = 0;
            int prio = 0;
            int cputime = 0;
            printf("输入要创建的进程数量：");
            scanf("%d",&num);
            printf("请输入这些进程的初始优先级(1~9，数字越小优先级越高)：");
            scanf("%d",&prio);
            if(prio < 1 || prio > 9) {
                printf("使用默认优先级：5\n");
                prio = 5;
            }
            printf("请输入这些进程需要运行的时长：");
            scanf("%d",&cputime);
            while(cputime == 0) {
                printf("时长无效，请重新输入：");
                scanf("%d",&cputime);
            }

            for(int i = 0; i < num; i++) {
                CreateAProcess(prio, round, cputime);
            }
            printf("已添加%d个进程。\n",num);
            break;
        }
        /*查看进程*/
        case 2:{
            printf("进程列表：\n0.所有进程\t1.运行列表\n2.就绪列表\t3.等待列表\n请输入要查看的列表序号：");
            int a;
            scanf("%d",&a);
            ListOfProcess(a);
            break;
        }
        /*挂起进程*/
        case 3:{
            printf("请输入要挂起进程的ID：");
            int PID;
            struct node *p;
            while(1){
                scanf("%d",&PID);
                p = PIDSearch(PID);
                if(!p){
                    printf("未找到此进程，请重新输入：");
                } else break;
            }
            if(p->state == StateWaiting) printf("操作失败，进程已经处于挂起状态\n");
            else {p->state = StateWaiting; printf("操作成功!\n");}
            break;
        }
        /*唤醒进程*/
        case 4:{
            printf("请输入要唤醒进程的ID：");
            int PID;
            struct node *p;
            while(1){
                scanf("%d",&PID);
                p = PIDSearch(PID);
                if(!p){
                    printf("未找到此进程，请重新输入：");
                } else break;
            }
            if(p->state == StateReady) printf("操作失败，进程已经处于挂起状态\n");
            else {p->state = StateReady; printf("操作成功!\n");}
            break;
        }
        /*销毁进程*/
        case 5:{
            printf("请输入要销毁的进程ID：");
            int PID;
            struct node *p;
            while(1){
                scanf("%d",&PID);
                p = PIDSearch(PID);
                if(!p){
                    printf("未找到此进程，请重新输入：");
                } else break;
            }
            KillProcess(PID);
            printf("销毁成功！\n");
            break;
        }
        /*开始运行*/
        case 6:{
            // printf("选择运行模式：\n");
            // printf("1.动态优先级：\t");
            // printf("2.时间片轮转：(当前时间片：%d)\n",round);
            // int a;
            // scanf("%d",&a);
            // if(a == 1) RunAsPS();
            // else if(a == 2) RunAsRR();
            // else printf("操作符无效！\n");
            printf("1.显示详细信息\t2.仅显示结束信息\n");
            scanf("%d",&infoSetting);
            getchar();
            if(infoSetting != Curt && infoSetting != Detailed) {
                printf("无效，默认显示详细信息!\n");
                infoSetting = Detailed;
            }
            printf("运行结束后是否保留进程并重置剩余运行时间(Y/N)\n");
            char a[3] = "y";
            scanf("%s",&a);
            IfKill = (a[0] == 'Y' || a[0] =='y') ? 0 : ((a[0] == 'N' || a[0] == 'n') ? 1 : printf("无效，默认保留！\n"));
            RunAsRR();
            break;
        }
        /*修改默认时间片参数*/
        case 7:{
            printf("将默认时间片修改为：");
            scanf("%d",&round);
            printf("成功修改为：%d!\n",round);
            break;
        }
        /*修改内存*/
        case 8:{
            printf("将就绪列表长度修改为：(上限为10)");
            scanf("%d",&readymax);
            if( readymax >= 10 ) {
                printf("操作失败！");
                readymax = 5;
                break;
            }
            printf("成功修改为：%d!\n",readymax);
            RefreshReadyList();
            break;
        }
        /*关机*/
        case 9: {
            printf("**************************************\n");
            printf("再见！\n");
            return 0;
        }
        default:
            printf("操作符号无效！\n");
        continue;
        }
    }
}

void CreateAProcess(int prio,int round,int cputime) {
    struct node *p,*p0;
    p = (struct node*)malloc(sizeof(struct node));
    p->prio = prio;
    p->priolock = prio;
    p->round = round;
    p->cputime = cputime;
    p->needtime = cputime;
    count = count + 1;
    p->count = 0;
    p->PID = InitPID();
    p->next = NULL;
    AddToAll(p);
}

int InitPID() {
    int pid = 1001;
    while(PIDSearch(pid)) pid++;
    return pid;
}

void AddToAll(struct node *new) {
    struct node *p,*p0;
    p = p0 = AllList;
    while(p) {
        p0 = p;
        p = p->next;
    }
    p = new;
    p0->next = p;
    p->state = StateWaiting;
    RefreshReadyList();
}

void ListOfProcess(int StateList) {
    struct node *p;
    RefreshReadyList();
    switch (StateList) {
        case StateReady: {
            printf("**************************************\n");
            printf("就绪列表：\n");
            for(int i = 0; i < readymax && Memory[i] != 0; i++) {
                p = PIDSearch(Memory[i]);
                printf("进程ID：%d ",p->PID);
                printf("进程优先级：%d ",p->prio);
                printf("进程时间片：%d ",p->round);
                printf("进程需要运行时间：%d\n",p->cputime);
            }
            printf("End!\n");
            break;
        }
        case StateRunning: {
            p = AllList->next;
            printf("**************************************\n");
            printf("运行列表：\n");
            if (Memory[0] != 0) {
                p = PIDSearch(Memory[0]);
                printf("(已就绪)进程ID：%d ",p->PID);
                printf("进程优先级：%d ",p->prio);
                printf("进程时间片：%d ",p->round);
                printf("进程需要运行时间：%d\n",p->cputime);
            }
            printf("End!\n");
            break;
        }
        case StateWaiting: {
            p = AllList->next;
            printf("**************************************\n");
            printf("等待列表：\n");
            while(p) {
                if (p->state == StateWaiting) {
                    printf("进程ID：%d ",p->PID);
                    printf("进程优先级：%d ",p->prio);
                    printf("进程时间片：%d ",p->round);
                    printf("进程需要运行时间：%d\n",p->cputime);
                }
                p = p->next;
            }
            printf("End!\n");
            break;
        }
        case AllProcess: {
            p = AllList->next;
            printf("**************************************\n");
            printf("所有进程：(总数为%d)\n",count);
            while(p) {
                printf("进程ID：%d ",p->PID);
                printf("进程状态：%d ",p->state);
                printf("进程优先级：%d ",p->prio);
                printf("进程时间片：%d ",p->round);
                printf("进程需要运行时间：%d\n",p->cputime);
                p = p->next;
            }
            printf("End!\n");
            break;
        }
        default: printf("输入的操作符号无效!\n");
    }
}

void RefreshWaitingList() {
    struct node *p = AllList->next;
    while(p) {
        if(p->state != StateLocked) p->state = StateWaiting;
        p = p->next;
    }
}

void RefreshReadyList() {
    /*刷新内存*/
    for(int i = 0; i < 10; i++) {
        if( i < readymax ) {
            Memory[i] = 0;
        } else if ( i >= readymax ) {
            Memory[i] = 1000;
        }
    }
    /*刷新进程*/
    RefreshWaitingList();
    /*加入内存*/
    struct node *p = AllList->next;
    for(int i = 0; i < readymax && p; i++) {
        p = AllList->next;
        for(int prio = 1; prio < 10; p = p->next) {
            if(!p) {
                p = AllList->next;
                prio = prio + 1;
            }
            if( p->prio == prio && p->state == StateWaiting) {
                Memory[i] = p->PID;
                p->state = StateReady;
                break;
            }
        }
    }
}

void KillProcess(int PID) {
    struct node *p,*p0;
    p0 = AllList;
    p = PIDSearch(PID);
    while(p0->next != p) p0 = p0->next;
    p0->next = p->next;
    free(p);
    count = count - 1;
    RefreshReadyList();
}

// void RunAsPS() {
//     printf("**************************************\n");
//     RefreshReadyList();
//     struct node *p;
//     int time = 0;
//     while(Memory[0] != 0) {
//         p = PIDSearch(Memory[0]);
//         printf("%d...开始：%d 优先级：%d 剩余时间：%d\n",time,p->PID,p->prio,p->needtime);
//         p->state = StateRunning;
//         if(p->needtime <= round) {
//             time = time + p->needtime;
//             printf("%d...结束：%d 优先级：%d ，已销毁\n",time,p->PID,p->prio);
//             KillProcess(p->PID);
//         } else {
//             p->needtime = p->needtime - round;
//             p->prio = p->prio + 2/p->needtime;
//             printf("%d...暂停：%d 优先级：%d 剩余时间：%d\n",time,p->PID,p->prio,p->needtime);
//             RefreshReadyList();
//         }
//     }
//     printf("运行完成！\n");
// }

void RunAsRR() {
    printf("**************************************\n");
    RefreshReadyList();
    struct node *p = AllList->next;
    if(!p) {
        printf("无可运行的进程，请先创建进程！\n");
        return 0;
    }
    int time = 0;
    while(Memory[0] != 0) {
        p = PIDSearch(Memory[0]);
        if(infoSetting == Detailed) printf("%.3d---开始：%d 优先级：%d 剩余运行时间：%d\n",time,p->PID,p->prio,p->needtime);
        p->state = StateRunning;
        p->count = p->count +1;
        if(p->needtime <= round) {
            time = time + p->needtime;
            if (IfKill == 1) {
                printf("%.3d---结束：%d 优先级：%d ，共运行%d次，已销毁\n\n",time,p->PID,p->prio,p->count);
                KillProcess(p->PID);
            } else if (IfKill == 0) {
                printf("%.3d---结束：%d 优先级：%d，共运行%d次\n\n",time,p->PID,p->prio,p->count);
                p->needtime = p->cputime;
                p->prio = p->priolock;
                p->state = StateLocked;
                RefreshReadyList();
            }
            
        } else {
            p->needtime = p->needtime - round;
            p->state = StateReady;
            time = time + round;
            p->prio = p->prio + 4/p->needtime;
            if(p->prio > 9) p->prio = 9;
            if(infoSetting == Detailed) printf("%.3d---暂停：%d 优先级：%d 剩余时间：%d\n\n",time,p->PID,p->prio,p->needtime);
            RefreshReadyList();
        }
    }
    printf("运行完成！\n");
    UnlockP();
}

void UnlockP() {
    struct node *p = AllList->next;
    while(p) {
        if(p->state == StateLocked) p->state = StateWaiting;
        p = p->next;
    }
    RefreshReadyList();
}
