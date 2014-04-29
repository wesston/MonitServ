#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int meminfo() {
	//MEMLOAD
	FILE *f;
	char buf[100];
	int memTotal, memFree, memUse, buffers, cached;
	
	f = fopen("/proc/meminfo", "r");	
	
	if (!f) {
		perror("open (/proc/meminfo)");
		close(f);
		return 1;
	}

	fgets(buf, sizeof(buf) - 1, f); 
	sscanf(buf, "%*s%d", &memTotal);
	fgets(buf, sizeof(buf) - 1, f);
	sscanf(buf, "%*s%d", &memFree);
	fgets(buf, sizeof(buf) - 1, f); 
	sscanf(buf, "%*s%d", &buffers);
	fgets(buf, sizeof(buf) - 1, f); 
	sscanf(buf, "%*s%d", &cached);
	
	memUse = memTotal - memFree - buffers - cached;
	//Если MemTotal = 100%, тогда MemUse = ....
	memUse = memUse * 100 / memTotal;
	
	fclose(f);
	return memUse;
}

int diskinfo() {
	//DiskUsage
	FILE *f;
	char *cmd = "df -h";
	char buf[200];
	int i = 0;
	int diskState;
	char *p1;

	f = popen (cmd, "r");
	if (!f) {
		perror("popen");
		exit(1);
	}

	for(i = 0; i < 2; i++) {
		fgets(buf, sizeof(buf) - 1, f);
		
		if (i == 1) {
			p1 = strchr(buf, '%');
			*p1 = 0;
			
			while (*p1 != ' ') {
				p1--;
			}
		}
	}
	
	diskState = atoi(p1);
	pclose(f);
	 
	return diskState;
}

char* uptime() {
	FILE *f;
	char *cmd = "uptime";
	char buf[50];
	char *p1, *p2, *uptime;
	int len;
	
	f = popen(cmd, "r");
	if (!f) {
		perror("popen");
		exit(1);
	}
	
	fgets(buf, sizeof(buf) - 1, f);
	p1 = strstr(buf, "up");
	p1 = p1 + 3;
	p2 = strchr(p1, ',');
	len = p2 - p1;
	
	uptime = (char *) malloc(20 * sizeof(char));
	if(uptime == NULL) {
		printf("Требуемая память не выделена.\n");
		exit(1);
	}
	
	strncpy(uptime, p1, len);
    pclose(f);
    
    return uptime;
}

char* techstat() {

	char *tech;
	int i;
	
	tech = (char *)malloc(40 * sizeof(char));
	sprintf(tech, "%d %d %d %s", cpuinfo(), meminfo(), diskinfo(), uptime());
	
	return tech;
}

int main() {
	char const *SWITCHOFF = "off";
	char const *TECHSTAT = "techstat";
	struct sockaddr_in c_addr;
	int sock;
	char buf[100];
	
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
		recv(sock, buf, sizeof(buf), 0);	
		
		if(strcmp(buf, TECHSTAT) == 0) {
			puts("Сервер запросил статистику!");
			char const *STAT = techstat();
			send(sock, STAT, strlen(STAT) + 1, 0);
		}
	}
	
	close(sock);
		
	return 0;	
}