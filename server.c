#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct stat {
	int cpuinfo;
	int meminfo;
	int diskinfo;
	char uptime[20];
};

int main() {

	const char *TECHSTAT = "techstat";
	struct sockaddr_in s_addr;
	int listener, sock;
	char buf[100], in[100];


	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		perror("socket");
		return 1;
	}
	
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(7777);
	s_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(listener, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
		perror("bind");
		return 1;
	}
	
	listen(listener, 4);
	
	while(1) {
		sock = accept(listener, NULL, NULL);
		if (sock < 0) {
			perror("accept");
			return 1;
		}
		
		puts("\n-> Клиент подключен");
		puts("-> Введите команду:");
		
		while(1) {
			
			printf(">");
			scanf("%s", in);
			
			if (strcmp(in, TECHSTAT) == 0) {
				send(sock, TECHSTAT, strlen(TECHSTAT) + 1, 0);

				struct stat *techstat = (struct stat *)malloc(sizeof(struct stat));
				recv(sock, techstat, sizeof(struct stat), 0);
				
				puts("----------------------------------------------------------");
				printf("[Загрузка CPU: %d%%] [Загрузка MEM: %d%%] \n[Заполнение диска: %d%%] [Время работы:%s]\n", 
						techstat->cpuinfo, techstat->meminfo, techstat->diskinfo, techstat->uptime);
				puts("----------------------------------------------------------\n");

			}
		}
		
		close(sock);
	}
	
	
	return 0;

}