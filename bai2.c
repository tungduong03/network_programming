#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#define BUFFER_SIZE 256
int main() {
    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }

    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);

    // Gan socket voi cau truc dia chi
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    char buf[256];

    for (int i = 0; i < 8; i++) {
        if (fork() == 0) {
            while (1) {
                int client = accept(listener, NULL, NULL);
                printf("New client accepted: %d\n", client);
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                    continue;
                
                buf[ret] = 0;
                printf("Received from %d: %s\n", client, buf);
                

                char response[BUFFER_SIZE];
                // Kiểm tra lệnh GET_TIME và format
                if (strncmp(buf, "GET_TIME", 8) == 0) {
                    char *format = buf + 9; // Bỏ qua "GET_TIME "
                    time_t rawtime;
                    struct tm *timeinfo;
                    time(&rawtime);
                    timeinfo = localtime(&rawtime);

                    if (strncmp(format, "dd/mm/yyyy", 10) == 0) {
                        strftime(response, BUFFER_SIZE, "%d/%m/%Y", timeinfo);
                    } else if (strncmp(format, "dd/mm/yy", 8) == 0) {
                        strftime(response, BUFFER_SIZE, "%d/%m/%y", timeinfo);
                    } else if (strncmp(format, "mm/dd/yyyy", 10) == 0) {
                        strftime(response, BUFFER_SIZE, "%m/%d/%Y", timeinfo);
                    } else if (strncmp(format, "mm/dd/yy", 8) == 0) {
                        strftime(response, BUFFER_SIZE, "%m/%d/%y", timeinfo);
                    } 
                    else {
                        strcpy(response, "Invalid format\n");
                    }
                    // Gửi thời gian hoặc thông báo lỗi
                    send(client, response, strlen(response), 0);
                }
                close(client);
            }
            
            exit(0);
        }
    }

    getchar();
    killpg(0, SIGKILL);
    return 0;
}