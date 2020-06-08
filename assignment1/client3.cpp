#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <string>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <math.h>
#define MAXLINE 4096
using namespace std;
int main(int argc, char** argv){
    int   sockfd, n;
    char  recvline[4096], sendline[4096];
    struct sockaddr_in  servaddr;
	int numberOfData;
	char receive_data[1000] = "just for test";
	
	char client_data[1000] = "OK";
	char calcaulate_result[100];
	int type_use;
	int int1, int2, int_result;
	double double1, double2, double_result;

    if( argc != 2){
        printf("usage: ./client <ipaddress>\n");
        return 0;
    }

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n",argv[1]);
        return 0;
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

   
	int count = 0;
	while (1) {
		if (count == 0 || count == 2) {
			numberOfData = recv(sockfd, receive_data, 999, 0);
		}
		else {
			char receive_data1[1000];
			char receive_data2[1000];
			char receive_data3[1000];
			char receive_data4[1000];

			numberOfData = recv(sockfd, receive_data1, 999, 0);

			printf("test/type_use is: %s\n", receive_data1);
			if (receive_data1[0]!='f') {
				strncpy(receive_data4, receive_data1 + 0 ,3);
				strncpy(receive_data2, receive_data1 + 4, 2);
				strncpy(receive_data3, receive_data1 + 7, 2);

				int1 = atoi(receive_data2);
				int2= atoi(receive_data3);
				if (receive_data1[0] == 'a') {
					int_result = int1 + int2;
				}
				else if (receive_data1[0] == 's') {
					int_result = int1 - int2;
				}
				else if (receive_data1[0] == 'm') {
					int_result = int1 * int2;
				}
				else {
					int_result = int(int1/int2);
				}
				sprintf(calcaulate_result, "%d", int_result);
				printf("test/calculate: %d %s %d = %d\n", int1, receive_data4, int2, int_result);

			}
			else {
				strncpy(receive_data4, receive_data1 + 1, 4);
				strncpy(receive_data2, receive_data1 + 5, 8);
				strncpy(receive_data3, receive_data1 + 14, 7);

				double1 = atof(receive_data2);
				double2 = atof(receive_data3);
				if (receive_data1[1] == 'a') {
					double_result = double1 + double2;
				}
				else if (receive_data1[1] == 's') {
					double_result = double1 - double2;
				}
				else if (receive_data1[1] == 'm') {
					double_result = double1 * double2;
				}
				else {
					double_result = double1/double2;
					double_result = int(double_result*100000.0) / 100000.0;
				}
				double_result = int(double_result*100000.0) / 100000.0;
				sprintf(calcaulate_result, "%9.5f", double_result);
				printf("test/calculate: %8.8g %s %8.8g = %8.8g\n",double1, receive_data4, double2, double_result);
			}

			//printf("\n second data: %s!%s!%s!\n", receive_data1, receive_data2, receive_data3);
		}

		//string receive_data1 = receive_data;
		char result_store[100] = "OK";
		char result_store1[100] = "ERROR";
		int result1 = strcmp(receive_data, result_store);
		int result2 = strcmp(receive_data, result_store1);
		//|| strcmp(receive_data1, "ERROR" == 0£©
		if (result1 == 0 || result2==0){
			printf("calculate result is :%s\ncalculate close,close the connect\n", receive_data);
			break;
		}
		
		if (numberOfData == 0) {
			printf("close the connect\n");
			break;
		}

		else if(numberOfData==-1)
		{
			printf("have wrong\n");
			exit(1);
		}

		
		
		if (count == 1) {
			send(sockfd, calcaulate_result, 100, 0);
			printf("test: sucess send the result %s\n", calcaulate_result);
		}
		else if (count == 0) {
			send(sockfd, client_data, sizeof(client_data), 0);
			printf("test/time:%d  client receive data: %s\n", count, receive_data);
			printf("test: sucess send the result %s\n", calcaulate_result);
		}
		else if (count == 2) {
			char temp[] = "client want to close";
			send(sockfd, temp, sizeof(temp), 0);
			printf("test/time:%d  client receive data: %s\n", count, receive_data);
			printf("test: sucess send the result %s\n", calcaulate_result);
		}
		
		printf("send message successfully\n\n");
		count++;
	}

	close(sockfd);
    return 0;
}