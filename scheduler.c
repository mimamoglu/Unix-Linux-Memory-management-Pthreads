#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sched.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#define MEMORY 1000 * 512
#define FRAME_SIZE 512
#define MAX_MSG_SIZE 200000

typedef struct {
    int is_allocated;
    char content[FRAME_SIZE];
} Frame;

typedef struct {
    long msg_type;    
    char msg_text[MAX_MSG_SIZE];
    char stringProcess[9];
} message;

int msgid;
Frame memory[1000];
long processes[10];


void initialize_memory() {
    int i;
    for (i = 0; i < 1000; i++) {
        memory[i].is_allocated = 0;
    }
}

int find_free_frame() {
    int i, k;
    for (i = 0; i < 1000; i++) {
        if (memory[i].is_allocated == 0) {
            return i;
        }
    }
    return -1;
}


int allocate_memory(char* temp) {
    int i, allocated_frames = 0;
    
    int frame_index = find_free_frame();
    if (!(frame_index == -1)) {
      memory[frame_index].is_allocated = 1;
      strcpy(memory[frame_index].content, temp);
      allocated_frames++;
    }
    return frame_index;
}

void* child_func(void* args) {

    message* msg1 = (message*)args;
    
    char* text = strdup(msg1->msg_text);
    long process_name = msg1->msg_type;
    char* stringProcess = strdup(msg1->stringProcess);
    
    int karakter_sayisi = strlen(text);
    double sonuc = (double)karakter_sayisi / 512;
    int sayfa_sayisi = ceil(sonuc);
    
    int page_table[sayfa_sayisi];
    int i, sn;
    char temp[513]; 
    for (i = 0 ; i < sayfa_sayisi; i++) {
      strncpy (temp, text, 512);
      temp[512] = '\0';
      strncpy (text, text+512, karakter_sayisi);
      sn = allocate_memory(temp);
      page_table[i] = sn;
    }
    free(args);
 
    while(1) {
       
      message pn;
      int page_number;
       
       if(sn == -1) {
            printf("memory dolu istemci %s reddedildi, process sonlandırılıp kaynakları geri alındı\n", stringProcess);
            strcpy(pn.msg_text, "KAPAN");
            pn.msg_type = process_name;
            msgsnd(msgid, &pn, strlen(pn.msg_text) + 1, 0);
            int k;
            for (k = 0; k < sayfa_sayisi; k++) {
              if(page_table[k] == -1) { break;}
              memory[page_table[k]].is_allocated = 0;
            }
            break;
        }
        
       fflush(stdout);
        if (msgrcv(msgid, (void*)&pn, MAX_MSG_SIZE, process_name, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        
        sscanf(pn.msg_text, "%d", &page_number);
        if(!(page_number >= 0 && page_number < sayfa_sayisi)) {
          printf("%s isimli istemciden gelen %d numaralı sayfaya erişim istemi reddedildi, process sonlandırılıp kaynakları geri alındı\n", stringProcess, page_number);
          strcpy(pn.msg_text, "KAPAN");
          pn.msg_type = process_name;
          msgsnd(msgid, &pn, strlen(pn.msg_text) + 1, 0);
          int k;
          for (k = 0; k < sayfa_sayisi; k++) {
            memory[page_table[k]].is_allocated = 0;
          }
          break;
        }
        else {
         printf("İstemci %s tarafından istenen sayfa %d :\n %s\n", stringProcess, page_number, memory[page_table[page_number]].content);
         strcpy(pn.msg_text, memory[page_table[page_number]].content);
         pn.msg_type = process_name;
         msgsnd(msgid, &pn, strlen(pn.msg_text) + 1, 0);
        }
       
    }
    
}

pthread_t worker_threads[10];

void create_client_thread(long process_name, char* text, char* sp) {
    pthread_t thread;
    message* args = (message*)malloc(sizeof(message));
    args->msg_type = process_name;
    strncpy(args->stringProcess, sp, 9);
    strncpy(args->msg_text, text, MAX_MSG_SIZE);
    
    if (pthread_create(&thread, NULL, child_func, args) != 0) {
        perror("pthread_create");
        exit(1);
    }
   
    for (int i = 0; i < 10; i++) {
        if (worker_threads[i] == 0) {
            worker_threads[i] = thread;
            break;
        }
    }
   
    if (pthread_detach(thread) != 0) {// Detach the thread to clean up its resources automatically after it finishes(thread) != 0) {
        perror("pthread_detach");
        exit(1);
    }
}

int main() {
  key_t key;
  message msg_received;
    
  // Özel bir anahtar oluştur
  key = ftok(".", 'T');

  // Mesaj kuyruğu oluştur
  msgid = msgget(key, IPC_CREAT | 0666);
  if (msgid == -1) {
      perror("msgget");
      exit(1);
  }
  memset(worker_threads, 0, sizeof(worker_threads));
  initialize_memory();  
  printf("Scheduler başlatıldı.\n");
  int k;
  for(k = 0; k < 10; k++) { processes[k] = 0; }
  
  while (1) {
    if (msgrcv(msgid, (void*)&msg_received, MAX_MSG_SIZE, 0, 0) == -1) {
      perror("msgrcv");
      exit(1);
    }
    int fl = 0;
    for(k = 0; k < 10; k++) {
      if(processes[k] == msg_received.msg_type) {
        fl = 1;
      }
     }
     if (fl == 0) {
        for (k = 0; k < 10; k++) {
          if(processes[k] == 0)  {
              processes[k] = msg_received.msg_type;
              break;
          }
        }
        int ln;
        char stringProcess[9];
        char test[1];
        strncpy(test, msg_received.msg_text, 1);
        ln = strtol(test, NULL, 10);
        strncpy(stringProcess, msg_received.msg_text+1, ln);
        int text_len = strlen(msg_received.msg_text);
        int plen = strlen(stringProcess);
        strncpy(msg_received.msg_text, msg_received.msg_text+(1+plen), text_len-1-plen);
        printf("Sunucu: Mesaj kimliği %s client için worker thread oluşturuldu\n", stringProcess);
        create_client_thread(msg_received.msg_type, msg_received.msg_text, stringProcess);
      }
  }
  for (int i = 0; i < 10; i++) {
    if (worker_threads[i] != 0) {
      if (pthread_join(worker_threads[i], NULL) != 0) {
        perror("pthread_join");
        exit(1);
      }
    }
  }
  return 0;
}

