#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <string>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<time.h>
#include"sstream"

#define random(x) (rand()%x)

#define MAXLINE 1000

using namespace std;

int main(int argc, char** argv){
    int  listenfd, connfd;
    struct sockaddr_in  servaddr;
	struct sockaddr_in clientAddress;
	int clientAddressLength;
    char  buff[1000];
	char receive_data[1000];
    int  n;
	int pid;
	char server_data[1000] = "TEXT TCP 1.0";
	string type[8] = { "add","sub","mul","div","fadd","fsub","fmul","fdiv" };//0 add,1 subtract,2 multiply,3 divide,4 float add,5 float subtract6 float multiply, 7 float divide
	int type_use;
	int int1, int2,int_result;
	double double1, double2,double_result;

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

    if( listen(listenfd, 10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }


	
	
	while (1)
	{
		printf("======waiting for client's request======\n");
		clientAddressLength = sizeof(clientAddress);
		clientAddressLength = sizeof(clientAddress);
		connfd = accept(listenfd, (struct sockaddr*)&clientAddress, &clientAddressLength);

		int count = 3;
		//create data to calculate
		srand((int)time(0));
		type_use = random(8);
		if (type_use < 4) {
			int1 = random(100);
			int2 = random(100);
			if (type_use == 0) {
				int_result = int1 + int2;
			}
			else if (type_use == 1) {
				int_result = int1 - int2;
			}
			else if (type_use == 2) {
				int_result = int1 * int2;
			}
			else {
				int_result = int(int1 / int2);
			}
			printf("test/calculate's result is: %d\n", int_result);
		}
		else if(type_use<8) {
			double1= (double)rand() / (double)(RAND_MAX / 100.0);
			double2 = (double)rand() / (double)(RAND_MAX / 100.0);
			double1= int(double1*100000.0) / 100000.0;
			double2 = int(double2*100000.0) / 100000.0;
			if (type_use == 4) {
				double_result = double1 + double2;
			}
			else if (type_use == 5) {
				double_result = double1 - double2;
			}
			else if (type_use == 6) {
				double_result = double1 * double2;
			}
			else {
				double_result = double1 / double2;
				double_result = int(double_result*100000.0) / 100000.0;
			}
			printf("test/calculate's result is: %8.8g\n", double_result);
		}

		printf("\ncreate new values to calculate:");
		printf("type: %d || %d  %d \n     or %8.8g %8.8g\n",type_use, int1,int2,double1, double2);

		pid = fork();

		//printf("\n the big while have been workng--\n");
		
		if (pid == 0) {
			while (1) {
				//printf("the small while have been workng--\n");
				if (count == 3) {
					
					send(connfd, server_data, 500, 0);
					count--;
					n = recv(connfd, buff, MAXLINE, 0);
					buff[n] = '\0';
					printf("test/time:%d--recv msg from client: %s\n", count, buff);
					sleep(1);
				}

				else if (count == 2) {
					//printf("start!!!!\n");
					char send_type[100];
					char send_value1[20];
					char send_value2[20];
					sprintf(send_type, "");
					sprintf(send_value1, "");
					sprintf(send_value1, "");
					/*itoa(type_use, senf_type, 10);*/

					strcpy(send_type, type[type_use].c_str());
					printf("test:start-----\n");
					if (type_use < 4) {
						/*send_value1 = to_string(int1);
						send_value2 = to_string(int2);*/
						sprintf(send_value1, "% 3d", int1);
						sprintf(send_value2, "% 3d", int2);
					}
					else {
						sprintf(send_value1, "% 9.5f", double1);
						sprintf(send_value2, "% 9.5f", double2);
					}
					printf("test:start-1----\n");

					strcat(send_type, send_value1);
					strcat(send_type, send_value2);
					send(connfd, send_type, 500, 0);
					sleep(0.5);

					/*send(connfd, send_type, 500, 0);
					sleep(0.5);
					send(connfd, send_value1, 500, 0);
					sleep(0.5);
					send(connfd, send_value2, 500, 0);*/
					printf("test:star-2------%s\n", send_type);
					/*char server_data_1[4096] = "have begined";
					send(connfd, server_data_1, 500, 0);*/
					count--;
					/*n = recv(connfd, buff, MAXLINE, 0);
					buff[n] = '\0';
					printf("--%d--recv msg from client: %s\n", count, buff);*/
					sleep(1);
				}

				else if (count == 1) {
					printf("test:last-1----!!!!!!\n");
					n = recv(connfd, receive_data, 1000, 0);
					printf("test:last-2----!!!!!!\n");
					char server_data2[6];
					if (type_use < 4) {
						int receive_result = atof(receive_data);
						if (receive_result == int_result) {
							/*server_data2 = "OK";*/
							sprintf(server_data2, "%s", "OK");
							
						}
						else {
							/*server_data2 = "ERROR";*/
							sprintf(server_data2, "%s", "ERROR");
						}
					}
					else {
						double receive_result = atof(receive_data);
						if ((receive_result - double_result) > -0.01 && (receive_result - double_result) < 0.01) {
							sprintf(server_data2, "%s", "OK");
						}
						else {
							sprintf(server_data2, "%s", "ERROR");
						}
						
					}
					printf("test:last-3----!!!!!!\n");
					send(connfd, server_data2, 500, 0);
					
					
					buff[n] = '\0';
					printf("test/time:%d  recv msg from client: %s\n", count, buff);
					count--;
					sleep(1);
				}

				/*else if(count==0)
				{

					char server_data3[] = "";
					send(connfd, server_data3, 0, 0);
					printf("close the connect");
					break;
				}*/
			}
			exit(0);
		}
		else {
			printf("Parent, close connfd().\n");
			close(connfd);//sock is closed BY PARENT
		}
    }
    
    return 0;
}