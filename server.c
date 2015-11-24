#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8080
#define BACKLOG 10

void InitServer()
{
	int ld = socket(AF_INET, SOCK_STREAM, 0);
	if (ld == -1)
	{
		printf("[EXCEPTION]: Socket returns error!\n");
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	socklen_t size_saddr;
	
	int bindId = bind(ld, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (bindId == -1)
	{
		printf("[EXCEPTION]: Bind returns error!\n");
	}
	
	int listenId = listen(ld, BACKLOG);
	if (listenId == -1)
	{
		printf("[EXCEPTION]: Listen returns error!\n");
	}
	
	while (1)
	{
		struct sockaddr_in client_addr;
		socklen_t size_caddr;
		
		int cd = accept(ld, (struct sockaddr *)&client_addr, &size_caddr);
		if (cd == -1)
		{
			printf("[EXCEPTION]: Accept client error!\n");
		}
		printf("workd");
		break;
		//TODO action with client
	}
}

int main()
{
	InitServer();
	return 0;
}
