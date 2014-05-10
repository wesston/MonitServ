#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>
#include <netdb.h>

struct stat {
	int cpuinfo;
	int meminfo;
	int diskinfo;
	char uptime[20];
};

void ping(struct sockaddr_in c_addr) {
	FILE *f;
	time_t second;
	struct tm *tm;
	struct hostent *c_addrinfo;
	char cmd[40] = "ping -c 4 ";
	char c_ip[16], c_hostname[50];
	char buf[100];
	int transmit, recv;
	char *p, *work_status;

	strcpy(c_ip, inet_ntoa(c_addr.sin_addr));
	strcat(cmd, c_ip);
	c_addrinfo = gethostbyaddr((char *)&c_addr.sin_addr.s_addr, sizeof(c_addr.sin_addr.s_addr), AF_INET);
	strcpy(c_hostname, c_addrinfo->h_name);

	f = popen(cmd, "r");
	if(!f) {
		perror("open");
		exit(1);
	}

	second = time(NULL);
	tm = localtime(&second);

	while(1) {					
		fgets(buf, sizeof(buf) - 1, f);
		if (strstr(buf, "transmitted"))
			break;
	}

	sscanf(buf, "%d", &transmit);
	sscanf(buf, "%d", &recv);

	if(transmit == recv) {
		work_status = "Работает";
	} else {
		work_status = "Не работает";
	}

	puts("---------------------------------------------------------------------------------");
	printf("%s | Hostname: %s | IP: %s | Дата проверки: %d.%d.%d, %d:%d\n", work_status, c_hostname, c_ip, tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min);
	puts("---------------------------------------------------------------------------------\n");
}

int main() {

	const char *TECHSTAT = "techstat";
	const char *PING = "ping";
	struct sockaddr_in s_addr, c_addr;
	int listener, sock;
	char buf[100], in[100];
	int c_addr_len;


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
		c_addr_len = sizeof(c_addr);
		sock = accept(listener, (struct sockaddr *)&c_addr, &c_addr_len);
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
				printf("[Загрузка CPU: %d%%] [Загрузка MEM: %d%%] \n[Заполнение диска: %d%%] [Время работы: %s]\n", 
						techstat->cpuinfo, techstat->meminfo, techstat->diskinfo, techstat->uptime);
				puts("----------------------------------------------------------\n");

			}

			if(strcmp(in, PING) == 0) {
				ping(c_addr);
			}
		}
		
		close(sock);
	}
	
	
	return 0;

}