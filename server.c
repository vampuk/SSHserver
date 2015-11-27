#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define countOfProc 3 //кол-во потоков
pthread_t	proc[countOfProc]; //массив потоков
int		tasks[countOfProc]; //массив заданий, каждое задание соответствует потоку
pthread_mutex_t	locks[countOfProc]; //массив мютексов, каждый мютекс соответствует потоку


void connection(int cd)
{
		printf("Socket[%d]\n", cd);				
		while (1)
		{
			char *mess = "User:\0";
			send(cd, mess, strlen(mess), 0);
			
			// Принятие команды
			char cmd[100];
			memset(cmd, '\0', sizeof(cmd));
			recv(cd, &cmd, sizeof(cmd), 0);
			cmd[strlen(cmd) - 2] = '\0';	// Удалить \r\n
			
			printf("User command: %s\n", cmd); // Печать команды на сервер

			// Перечень команд
			if (strcmp(cmd, "hello\0") == 0)
			{				
				char *mess = "Hello!\r\n\0";
				send(cd, mess, strlen(mess), 0);
			}
			else if (strcmp(cmd, "help\0") == 0)
			{
				char *mess = "My commands:help,hello,exit Standart commands: ls,cat... \r\n\0";
				send(cd, mess, strlen(mess), 0);
				
			}
			else if (strcmp(cmd, "exit\0") == 0) //чтобы отключиться от сервера, а не от терминала 
			{
				char *mess = "By by!\r\n\0";
				send(cd, mess, strlen(mess), 0);				
				close(cd);
				break;
			}
			else // Остальные команды (ls...)
			{				
				FILE *fp = popen(cmd, "r");
				if (fp == NULL) {
				char *mess = "Command error\r\n\0";
				send(cd, mess, strlen(mess), 0);
				}	
				// Чтение, печать в сокет
				char buf[1000];
				while (fgets(buf, sizeof(buf), fp) != NULL) {
				send(cd, buf, strlen(buf), 0);
				}	
				pclose(fp);
			}	
		}		
		printf("Socket [%d] closed\n", cd);
}

void *thrFunc(void *arg)
{
	int id = (int)arg; //Присвоение ID
	while (1)
	{		
		printf("Proc %d waiting\n", id);
		pthread_mutex_lock(&locks[id]);//Лочим мютекс	
		pthread_mutex_lock(&locks[id]);//Режим ожидания(пока логика не разлочит)
		pthread_mutex_unlock(&locks[id]);//Снимаем последний лок		
		printf("Proc %d starting\n", id);
		connection(tasks[id]);	
	}	
	return 0;
}

void newTask(int task)
{
	int i;
	for (i = 0; i < countOfProc; i++)
	{
		if (pthread_mutex_trylock(&locks[i]))//Если закрыт, то не можем залочить
		{
			tasks[i] = task;
			pthread_mutex_unlock(&locks[i]);
			return;
		}
		else{pthread_mutex_unlock(&locks[i]);}
	}	
	close(task);//Закрыть подключение
	printf("All proc are bisy.\n");
}

void start_server()
{
	int ld = socket(AF_INET, SOCK_STREAM, 0);
	if (ld == -1){printf("Listener error\n");}
	struct sockaddr_in servNew;
	servNew.sin_family = AF_INET;
	servNew.sin_port = htons(8080);
	servNew.sin_addr.s_addr = INADDR_ANY;		
	int bindRes = bind(ld, (struct sockaddr *)&servNew, sizeof(servNew));
	socklen_t sizeServ;
	if (bindRes == -1){printf("Bind error\n");}	
	int listenRes = listen(ld, 10);
	if (listenRes == -1){printf("Listen error\n");}	
	while (1)
	{
		struct sockaddr_in client;
		socklen_t sizeClient;		
		int cd = accept(ld, (struct sockaddr *)&client, &sizeClient);
		if (cd == -1){printf("Accept error\n");}		
		newTask(cd);
	}
}

int main()
{	
	int i;
	for(i = 0; i < countOfProc; i++)
	{
		pthread_mutex_init(&locks[i], NULL);
		pthread_create(&proc[i], NULL, thrFunc, (void *)(long long)i);
	}

	start_server();

	return 0;
}
