#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>

#define MAX_MSG_SIZE 200000

typedef struct {
    long msg_type;
    char msg_text[MAX_MSG_SIZE];
} message;

long process_name;
char filename[50];
int msgid;
int m;

void receive_messages(long process_name) {
  message msg;
  
  if (msgrcv(msgid, (void*)&msg, MAX_MSG_SIZE, process_name, 0) == -1) {
    perror("msgrcv");
    exit(1);
  }
    
  // Trim trailing spaces and other whitespace characters from the received message
  int len = strlen(msg.msg_text);
  while (len > 0 && isspace((unsigned char)msg.msg_text[len - 1])) {
    msg.msg_text[--len] = '\0';
  }
        
  if (strncmp(msg.msg_text, "KAPAN", 5) == 0) {
    printf("user process kapanıyor...");
    fflush(stdout); 
    exit(1);
  }
  else {
    printf("İstenen sayfa :\n%s\n", msg.msg_text);
  }
  m = 0;
}

void send_messages(long process_name) {
  message msg;
  char send_msg[MAX_MSG_SIZE];            
  msg.msg_type = process_name;
  printf("Enter the page number you want to access (-1 to terminate): ");
  fgets(send_msg, MAX_MSG_SIZE, stdin);
  // Page number i sunucuya gonder
  strncpy(msg.msg_text, send_msg, MAX_MSG_SIZE);
  msgsnd(msgid, (void*)&msg, strlen(msg.msg_text)+1, 0);
  msgsnd(msgid, (void*)&msg, strlen(msg.msg_text)+1, 0);
  m = 1;
}

int main(int argc, char *argv[]) {
  key_t key;
  char stringProcess[9];
  if (argc != 3) {
    printf("Usage: %s <Process name> <File name>\n", argv[0]);
    return 1;
  }
  
  strncpy(stringProcess, argv[1], 9);
  strcpy(filename, argv[2]);

  // Istenilen dosyayi oku texti ve process name i scheduler e gonder
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    printf("Error: File not found.\n");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  int fsize = ftell(file);
  fseek(file, 0, SEEK_SET);  
  char text[fsize];
  fread(text, fsize, 1, file);
  fclose(file);
    
  key = ftok(".", 'T');
  msgid = msgget(key, 0);
  if (msgid == -1) {
      perror("msgget");
      exit(1);
  }
  
  srand(time(NULL));
  long int range = (99999 + 1) - 10000;
  long int process_id = 10000 + (rand() % range);  
  
  message msg;
  char lth[fsize+10];
  int processName_length = strlen(stringProcess);
  sprintf(lth, "%d", processName_length);
  strcat(lth, stringProcess);
  strcat(lth, text);
  strncpy(msg.msg_text, lth, MAX_MSG_SIZE);
  msg.msg_type = process_id;
  msgsnd(msgid, &msg, strlen(msg.msg_text) + 1, 0);
    
  // user processes dongusu
  m = 0;
  while(1) {
    if(m == 0) {
      send_messages(process_id);
    }
    else{ 
      receive_messages(process_id);
    }
  }
  return 0;
}
