#include <stdio.h>
#include <stdlib.h>

int menu(){
    int esc;

    system("cls");
    printf("\n\n                              CPU SCHEDULING");
    printf("\n-----------------------------------------------------------------------------------------------------------------------------");
    printf("\n     Choose the algorithm you want to use to simulate the scheduler. \n");
    printf("\n                  < 1 > First-Come, First-Serve (FCFS)");
    printf("\n                  < 2 > Shortest-Job-First- (SRTF) ");
    printf("\n                  < 3 > Priority scheduler ");
    printf("\n                  < 4 > Round Robin (RR) ");
    printf("\n                  < 5 > Exit \n");
    printf("\n     Your choice: ");
    scanf("%i", &esc);

    return esc;
}

int painel(){
    int esc;
    system("cls");
    printf("\n\n                              CPU SCHEDULER");
    printf("\n-------------------------------------------------------------------------------\n\n");
    printf("                           Definition of processes.\n\n");
    printf("\n                  < 1 > Used processes established by the program.");
    printf("\n                  < 2 > Introduce new processes.");
    printf("\n\n    Your choice: ");
    scanf("%i", &esc);
    if (esc>2)
        painel();
    else
        return esc;
}

void top(){
    system("cls");
    printf("\n\n                              CPU SCHEDULER");
    printf("\n-------------------------------------------------------------------------------\n\n");
}

struct processes{
    int id;
    int dur;
    int prio;
    int execu;
    int esp;
    struct processes *prox;
    /*
        id - process identification
        dur - process duration
        exe - process execution time
        esp - process wait time
    */
};

struct processes *enter_processes(int id, int dur, int prio){
    struct processes *loc;
    loc = (struct processes*)malloc(sizeof(struct processes));
    if(loc == NULL){
        printf("Allocation error. \n End of execution\n");
        exit(1);
    };
    loc->id = id;
    loc->dur = dur;
    loc->prio = prio;
    loc->execu = 0;
    loc->esp = 0;
    loc->prox = NULL;
}

void list_processes(struct processes *loc){
    struct processes *tmp = loc;
    printf("\n\n\t\t\tList of processes\n\n");
    while(tmp != NULL){
        printf("\tProcess: %d\tPriority: %d\tDuration: %d\n", tmp->id, tmp->prio, tmp->dur);
        tmp = tmp->prox;
    }
    printf("\n");
}

void fcfs(struct processes *loc, int n_proc) {
    int tempo = 0, starting, fim, tmp_esp=0, turnaround_time=0;
    int p[n_proc], i=0;
    struct processes *tmp = loc;
    printf("\n\t\t First-Come, First-Serve (FCFS)\n\n");
    while (tmp != NULL){
        starting = tempo;
        tempo += tmp->dur;
        p[i] = tempo;
        fim = tempo;
        printf("\tProcess: %d\t Duration: %d\t Waiting Time: %d\tProgram finishes: %d\n", tmp->id, tmp->dur, starting, fim);
        tmp_esp += starting;
        tmp = tmp->prox;
        i++;
    }
    i=0;
    printf("\n\t\tAverage Waiting Time= %f\n\n\n",tmp_esp*1.0/n_proc);
    printf("\t    Gantt chart.");
    printf("\n\n\t\t    ");
    for (i=0; i<n_proc; i++){
            printf("| P%d ", i+1);
    }
    printf("|");
    printf("\n\t\t    0  ");
    for (i=0; i<n_proc; i++)
        printf("  %d ", p[i]);
}

void sjf(struct processes *loc, int n_proc){
    int tempo_exe, shortest, starting, fim, tmp_esp, durc;
    struct processes *copies, *src_tmp, *tmp, *before_shortest;
    printf("\n\t\tShortest-Job-First- (SRTF)\n\n ");
    src_tmp = loc;
    copies = tmp = NULL;
    while (src_tmp != NULL){
        if(copies == NULL){
            copies = enter_processes(src_tmp->id, src_tmp->dur, src_tmp->prio);
            tmp = copies;
        }
        else{
            tmp->prox = enter_processes(src_tmp->id, src_tmp->dur, src_tmp->prio);
            tmp = tmp->prox;
        }
        src_tmp = src_tmp->prox;
    }
    tempo_exe = 0;
    while(copies != NULL){/*search for the new process*/
        before_shortest = NULL;
        shortest = copies->dur;
        tmp = copies->prox;
        src_tmp = copies;
        while(tmp != NULL){
            if(tmp->dur < shortest){
                shortest = tmp->dur;
                before_shortest = src_tmp;
            }
            src_tmp = tmp;
            tmp = tmp->prox;
        }
        if(before_shortest == NULL){/*process is executed and copies and eliminated and allocates the 1st process*/
            starting = tempo_exe;
            tempo_exe += copies->dur;
            durc = copies->dur;
            fim = tempo_exe;
            printf("\tProcess: %d\t Duration: %d\t Waiting: %d\tProgram finishes: %d\n", copies->id, durc, starting, fim);
            tmp_esp += fim;
            src_tmp = copies;
            copies = copies->prox;
            free(src_tmp);
        }
        else{ /*allocates the 1st process if there is no smaller one*/
            tmp = before_shortest->prox;
            starting = tempo_exe;
            tempo_exe += tmp->dur;
            durc = tmp->dur;
            fim = tempo_exe;
            printf("\tProcess: %d\t Duration: %d\t Waiting: %d\tProgram finishes: %d\n", tmp->id, durc, starting, fim);
            before_shortest->prox = tmp->prox;
            free(tmp);
        }
    }
    printf("\n\t\tAverage Waiting Time= %f\n",tmp_esp*1.0/n_proc);
}

void sjf_simulator(struct processes *loc, int n_proc){
    int tempo_exe, shortest, starting, fim, durc, i=0, p[n_proc];
    struct processes *copies, *src_tmp, *tmp, *before_shortest;
    printf("\n\t    Gantt chart\n\n ");
    src_tmp = loc;
    copies = tmp = NULL;
    while (src_tmp != NULL){/*making copies of the processes*/
        if(copies == NULL){
            copies = enter_processes(src_tmp->id, src_tmp->dur, src_tmp->prio);
            tmp = copies;
        }
        else{
            tmp->prox = enter_processes(src_tmp->id, src_tmp->dur, src_tmp->prio);
            tmp = tmp->prox;
        }
        src_tmp = src_tmp->prox;
    }
    tempo_exe = 0;
    printf("\t\t  ");
    while(copies != NULL){/*search for the new process*/
        before_shortest = NULL;
        shortest = copies->dur;
        tmp = copies->prox;
        src_tmp = copies;
        while(tmp != NULL){
            if(tmp->dur < shortest){
                shortest = tmp->dur;
                before_shortest = src_tmp;
            }
            src_tmp = tmp;
            tmp = tmp->prox;
        }
        if(before_shortest == NULL){/*process runs and copies and eliminates and allocates the 1st process s is less*/
            starting = tempo_exe;
            tempo_exe += copies->dur;
            durc = copies->dur;
            fim = tempo_exe;
            p[i]=tempo_exe;
            printf("| P%d ", copies->id);
            src_tmp = copies;
            copies = copies->prox;
            free(src_tmp);
        }
        else{ /*allocates the 1st process if there is no smaller one*/
            tmp = before_shortest->prox;
            starting = tempo_exe;
            tempo_exe += tmp->dur;
            durc = tmp->dur;
            p[i]=tempo_exe;
            fim = tempo_exe;
            printf("| P%d ", tmp->id);
            before_shortest->prox = tmp->prox;
            free(tmp);
        }
        i++;
    }
    printf("|");
    printf("\n\t\t  0  ");
    for (i=0; i<n_proc; i++)
        printf("  %d ", p[i]);
}

void scheduling_priority(struct processes *loc, int n_proc){
    int tempo_exe, starting, major, fim, tmp_esp, durc;
    struct processes *copies, * src_tmp, *tmp, *major_prio;
    printf("\n\t\tPriority scheduler \n\n");
    src_tmp = loc;
    copies = tmp = NULL;
    while (src_tmp != NULL){/*making copies of the processes*/
        if(copies == NULL){
            copies = enter_processes(src_tmp->id, src_tmp->dur, src_tmp->prio);
            tmp = copies;
        }
        else{
            tmp->prox = enter_processes(src_tmp->id, src_tmp->dur, src_tmp->prio);
            tmp = tmp->prox;
        }
        src_tmp = src_tmp->prox;
    }
    tempo_exe = 0;
    while(copies != NULL){/*check next process*/
        major_prio = NULL;
        major = copies->prio;
        tmp = copies->prox;
        src_tmp = copies;
        while(tmp != NULL){
            if(tmp->prio < major){
                major = tmp->prio;
                major_prio = src_tmp;
            }
            src_tmp = tmp;
            tmp = tmp->prox;
        }
        if(major_prio == NULL){/*if 1st has major priority*/
            starting = tempo_exe;
            tempo_exe += copies->dur;
            durc = copies->dur;
            fim = tempo_exe;
            printf("\tProcess: %d\t Duration: %d\t Waiting: %d\tProgram finishes: %d\n", copies->id, durc, starting, fim);
            tmp_esp += fim;
            src_tmp = copies->prox;
            free(copies);
            copies = src_tmp;
        }
        else {/* if 1st has no major priority*/
            tmp = major_prio->prox;
            starting = tempo_exe;
            tempo_exe += tmp->dur;
            durc = tmp->dur;
            fim = tempo_exe;
            printf("\tProcess: %d\t Duration: %d\t Waiting: %d\tProgram finishes: %d\n", tmp->id, durc, starting, fim);
            major_prio->prox = tmp->prox;
            free(tmp);
        }
    }
    printf("\n\t\tAverage Waiting Time= %f\n",tmp_esp*1.0/n_proc);
}

void scheduling_priority_simulator(struct processes *loc, int n_proc){
    int tempo_exe, starting, major, fim, durc, i=0, p[n_proc];
    struct processes *copies, * src_tmp, *tmp, *major_prio;
    printf("\n\t    Gantt chart\n\n ");
    src_tmp = loc;
    copies = tmp = NULL;
    while (src_tmp != NULL){/*making copies of the processes*/
        if(copies == NULL){
            copies = enter_processes(src_tmp->id, src_tmp->dur, src_tmp->prio);
            tmp = copies;
        }
        else{
            tmp->prox = enter_processes(src_tmp->id, src_tmp->dur, src_tmp->prio);
            tmp = tmp->prox;
        }
        src_tmp = src_tmp->prox;
    }
    tempo_exe = 0;
    printf("\t\t  ");
    while(copies != NULL){/*check next process*/
        major_prio = NULL;
        major = copies->prio;
        tmp = copies->prox;
        src_tmp = copies;
        while(tmp != NULL){
            if(tmp->prio < major){
                major = tmp->prio;
                major_prio = src_tmp;
            }
            src_tmp = tmp;
            tmp = tmp->prox;
        }
        if(major_prio == NULL){/*if 1st has a higher priority*/
            starting = tempo_exe;
            tempo_exe += copies->dur;
            p[i]=tempo_exe;
            durc = copies->dur;
            fim = tempo_exe;
            printf("| P%d ", copies->id);
            src_tmp = copies->prox;
            free(copies);
            copies = src_tmp;
        }
        else {/* if 1st has no higher priority*/
            tmp = major_prio->prox;
            starting = tempo_exe;
            tempo_exe += tmp->dur;
            p[i]=tempo_exe;
            durc = tmp->dur;
            fim = tempo_exe;
            printf("| P%d ", tmp->id);
            major_prio->prox = tmp->prox;
            free(tmp);
        }
        i++;
    }
    printf("|");
    printf("\n\t\t  0  ");
    for (i=0; i<n_proc; i++)
        printf("  %d ", p[i]);
}

void robbin_round(struct processes *loc, int quantum, int n_proc){
    int count, j, time, remain, flag=0;
    int tmp_esp=0, turnaround_time=0, at[10], bt[10], rt[10];
    remain=n_proc;
    struct processes *copies, *src_tmp, *tmp, *slot;
    printf("\n\t\tRound Robin (RR) - Quantum: %d\n", quantum);
    src_tmp = loc;
    for(count=0; count < n_proc; count++){
            bt[count] = src_tmp->dur;
            rt[count] = bt[count];
            src_tmp = src_tmp->prox;
    }
    for(time = 0, count = 0; remain !=0;){
        at[count]=time;
        if(rt[count] <= quantum && rt[count] > 0){
            time+=rt[count];
            rt[count]=0;
            flag=1;
        }
        else if(rt[count]>0){
            rt[count]-=quantum;
            time+=quantum;
        }
        if(rt[count]==0 && flag==1){
            remain--;
            printf("\tProcess: %d\t Waiting: %d\tProgram finishes: %d\n", count+1, time-bt[count], time);
            tmp_esp+=time-bt[count];
            turnaround_time+=time;
            flag=0;
        }
        if(count == n_proc-1)
            count=0;
        else if(at[count]<=time)
            count++;
        else
            count=0;
    }
    printf("\n\t      Average Waiting Time= %f\n",tmp_esp*1.0/n_proc);
    printf("\t   Average Turnaround Time = %f",turnaround_time*1.0/n_proc);
    free(src_tmp);
}

int main(){
    int esc, es_proc, n_proc, i, id, dur, prio, posi=0;
    do{
    if (posi==0){
        es_proc=painel();
    }
    else
        esc = menu();
    struct processes *list_proc, *tmp_proc;
    if(es_proc == 1){
        n_proc = 4;
        list_proc = enter_processes(1, 12, 2);
        list_proc->prox = enter_processes(2, 2, 1); tmp_proc = list_proc->prox;
        tmp_proc->prox  = enter_processes(3,  8, 4); tmp_proc = tmp_proc->prox;
        tmp_proc->prox  = enter_processes(4,  10, 3);
        esc = menu();
    }
    else if(es_proc==2){
        top();
        printf("\n\n  OBS: Preferably choose for more than 3 processes.");
        printf("\n\n   Introduce the number of processes: ");
        scanf("%d", &n_proc);
        for(i=0; i<n_proc;i++){
            system("cls");
            top();
            printf("\n             Process %d: ", i+1);
            printf("\n\nEnter the duration (Burst time) P[%d]: ", i+1);
            scanf("%d", &dur);
            printf("Introduce your execution priority P[%d]: ", i+1);
            scanf("%d", &prio);
            if(i==0)
                list_proc = enter_processes(i+1, dur, prio);
            else if(i==1){
                list_proc->prox = enter_processes(i+1, dur, prio); tmp_proc = list_proc->prox;
            }
            else if(i==n_proc-1){
                tmp_proc->prox  = enter_processes(i+1,  dur, prio);
            }
            else{
                tmp_proc->prox  = enter_processes(i+1,  dur, prio); tmp_proc = tmp_proc->prox;
            }
        }
        posi=1;
        esc = menu();
    }

    if(esc == 1){
        system("cls");
        list_processes(list_proc);
        fcfs(list_proc, n_proc);
        printf("\n\n\t< 1 > Go back\nChoice: ");
        int i;
        scanf("%d", &i);
    }
    else if ( esc == 2){
        system("cls");
        list_processes(list_proc);
        sjf(list_proc, n_proc);
        sjf_simulator(list_proc, n_proc);
        printf("\n\n\t< 1 > Go back\nChoice: ");
        int i;
        scanf("%d", &i);
    }
    else if (esc == 3){
        system("cls");
        list_processes(list_proc);
        scheduling_priority(list_proc, n_proc);
        scheduling_priority_simulator(list_proc, n_proc);
        printf("\n\n\t< 1 > Go back\nChoice: ");
        int i;
        scanf("%d", &i);
    }
    else if( esc == 4){
        system("cls");
        int quantum;
        list_processes(list_proc);
        printf("\nTime quantum: ");
        scanf("%d", &quantum);
        robbin_round(list_proc, quantum, n_proc);
        printf("\n\n\t< 1 > Go back\n Your choice: ");
        int i;
        scanf("%d", &i);
    }
    else if( esc == 5){
        printf("\n\nProgram terminates.\n\n");
        exit(2);
    }
    }while(esc <= 6);
    return 0;
}
