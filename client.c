#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

int cpuinfo() {
	//CPULOAD
	FILE *f;
	char buf[100];
	float cpuInfo;
	
	f = fopen("/proc/loadavg", "r");
	if (!f) {
		perror("open (/proc/loadavg)");
		close(f);
		return 1;
	}
	
	fscanf(f, "%s", buf);
	sscanf(buf, "%f", &cpuInfo);
	cpuInfo = cpuInfo * 100;
	fclose(f);
	
	return cpuInfo;
}

int main() {
	struct sockaddr_in c_addr;
	int sock;
	char buf[100], word[20];
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		return 1;
	}
	
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(7777);
	c_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(sock, (struct sockaddr *)&c_addr, sizeof(c_addr)) < 0) {
		perror("connect");
		return 1;	
	}
	
	while(1) {
		printf("Введите слово для отправки\n");
		scanf("%s", word);
		send(sock, word, sizeof(word), 0);
		recv(sock, buf, sizeof(word), 0);
		printf("%s\n\n", buf);
	}
	
	close(sock);
	return 0;	
}