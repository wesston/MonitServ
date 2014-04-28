#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>


int main() {
	struct sockaddr_in s_addr;
	int listener, sock, bytes_read;
	char buf[100];

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
		
		while(1) {
			
			bytes_read = recv(sock, buf, sizeof(buf), 0);
			if(strstr(buf, "exit") != NULL) break;
			if(bytes_read <= 0) break;
			send(sock, buf, bytes_read, 0);
				
		}
		
		close(sock);
	}
	
	
	return 0;

}