#include <stdio.h>
#include<string.h>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include<time.h>
#include <ctime>

#define DEST_PORT 8000
#define DSET_IP_ADDRESS  "127.0.0.1"
using namespace std;

struct calcProtocol {
	uint16_t type;  // What message is this, 1 = server to client, 2 client to server, 3... reserved , conversion needed (for practice)
	uint16_t major_version; // 1, conversion needed (for practice)
	uint16_t minor_version; // 0, conversion needed (for practice)
	uint32_t id; // Server side identification with operation. Client must return the same ID as it got from Server., conversion needed (for practice)
	uint32_t arith; // What operation to perform, see mapping below. 
	int32_t inValue1; // integer value 1, conversion needed (for practice)
	int32_t inValue2; // integer value 2, conversion needed (for practice)
	int32_t inResult; // integer result, conversion needed (for practice)
	double flValue1;  // float value 1,NO NEED TO do host to Network or Network to Host conversion here, we are using equivalent platforms        
	double flValue2;  // float value 2,NO NEED TO do host to Network or Network to Host conversion here, we are using equivalent platforms
	double flResult;  // float result,NO NEED TO do host to Network or Network to Host conversion here, we are using equivalent platforms
};


struct calcMessage {
	uint16_t type;    // See below, conversion needed (for practice)
	uint32_t message; // See below, conversion needed (for practice)
	// Protocol, UDP = 17, TCP = 6, other values are reserved. 
	uint16_t protocol; // conversion needed (for practice)
	uint16_t major_version; // 1, conversion needed (for practice)
	uint16_t minor_version; // 0 , conversion needed (for practice)

};

int main()
{
	calcProtocol protocol_calc;
	calcProtocol *point_protocol_calc = &protocol_calc;
	//memset(&protocol_calc, 8, sizeof(calcProtocol));
	protocol_calc.type = 0;
	protocol_calc.major_version = 0;
	protocol_calc.minor_version = 0;
	protocol_calc.id = 10;
	protocol_calc.arith = 2;
	protocol_calc.inValue1 = 1;
	protocol_calc.inValue2 = 2;
	protocol_calc.inResult = 3;
	protocol_calc.flValue1 = 1.1;
	protocol_calc.flValue2 = 1.2;
	protocol_calc.flResult = 1.3;
	char get_protocol[500];

	calcMessage client_message_calc;
	calcMessage *point_client_message_calc = &client_message_calc;
	char get_message[500];
	client_message_calc.type = 22;
	client_message_calc.message = 0;
	client_message_calc.protocol = 17;
	client_message_calc.major_version = 1;
	client_message_calc.minor_version = 0;

	//memset(&client_message_calc, 0, sizeof(calcMessage));
	/*client_message_calc.type = 99;
	client_message_calc.message = 99;
	client_message_calc.protocol = 99;
	client_message_calc.major_version = 99;
	client_message_calc.minor_version = 99;*/
	
	//printf("--begin message--%d %d %d %d %d\n", point_client_message_calc->type, point_client_message_calc->message, point_client_message_calc->protocol, point_client_message_calc->major_version, point_client_message_calc->minor_version);

	int sock_fd;

	
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd < 0)
	{
		perror("socket");
		exit(1);
	}


	struct sockaddr_in addr_serv;
	int len;
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_addr.s_addr = inet_addr(DSET_IP_ADDRESS);
	addr_serv.sin_port = htons(DEST_PORT);
	len = sizeof(addr_serv);


	int send_num;
	int recv_num;
	int count = 0;
	int incount = 0;
	int type_use;
	int int1, int2, int_result;
	double double1, double2, double_result;
	string type[8] = { "iadd","isub","imul","idiv","fadd","fsub","fmul","fdiv" };
	clock_t start, now;//调用clock_t计算运行时间

	while (true)
	{
		if (count == 0) 
		{
			printf("----first time begin:\n");
			send_num = sendto(sock_fd, point_client_message_calc, sizeof(client_message_calc), 0, (struct sockaddr *)&addr_serv, len);
			printf("send message: %d %d %d %d %d\n", client_message_calc.type, client_message_calc.message, client_message_calc.protocol, client_message_calc.major_version, client_message_calc.minor_version);
			recv_num = recvfrom(sock_fd, point_protocol_calc, sizeof(protocol_calc), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);

			//进行判定，是否传收有效
			start = clock();
			while (incount < 5)
			{

				//表示收到数据（数据位长大于0）
				if (recv_num > 0) {
					break;
				}
				//表示未收到数据
				else {
					//处理方式1：如果重传次数在3次及以内
					if (incount < 4) {
						now = clock();
						if ((now - start) / CLOCKS_PER_SEC > (2 + 2 * incount)) {
							incount++;
							//重传请求
							if (incount < 3) {//还是二次重传之内
								send_num = sendto(sock_fd, point_client_message_calc, sizeof(client_message_calc), 0, (struct sockaddr *)&addr_serv, len);
								printf("client have resend the calc_message\n");
							}
							//测试代码
							/*if(incount==2){
								recv_num = recvfrom(sock_fd, point_protocol_calc, sizeof(protocol_calc), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);
							}*/
						}
						//正常用时不用注释
						recv_num = recvfrom(sock_fd, point_protocol_calc, sizeof(protocol_calc), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);
					}
				}
			}

			//incount==5表示已经重传了二次
			if (incount == 5) {
				printf("client first send calc_mesaage have wrong,close the connect\n");
				close(sock_fd);
				break;
			}
			//memcpy(point_protocol_calc, get_protocol, sizeof(point_protocol_calc) + 1);
			/*printf("--%d %d %d--\n", message_calc_point->type, message_calc_point->message, message_calc_point->major_version);
			printf("--%d\n", message_calc_point->major_version);*/
			printf("get protocol: %d %d %d %d %d %d %d %d %.2f %.2f %.2f\n", protocol_calc.type, protocol_calc.major_version, protocol_calc.minor_version, protocol_calc.id, protocol_calc.arith, protocol_calc.inValue1,
				protocol_calc.inValue2,protocol_calc.inResult, protocol_calc.flValue1, protocol_calc.flValue2, protocol_calc.flResult);
			
			//printf("--get protocol--%d %d %d %d %d %d %d %d %.2f %.2f %.2f\n", point_protocol_calc->type, point_protocol_calc->major_version, point_protocol_calc->minor_version, point_protocol_calc->id, point_protocol_calc->arith, 
			//	point_protocol_calc->inValue1,point_protocol_calc->inValue2, point_protocol_calc->inResult, point_protocol_calc->flValue1, point_protocol_calc->flValue2, point_protocol_calc->flResult);
			//recv_buf[recv_num] = '\0';
			count++;
		}
		else if (count == 1) 
		{
			printf("\n----second time begin: \n");
			type_use = protocol_calc.arith;
			//printf("%d", type_use);
			char type_temp[100];
			strcpy(type_temp, type[type_use].c_str());
			if (type_use < 4) {
				int1 = protocol_calc.inValue1;
				int2 = protocol_calc.inValue2;
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
					int_result = (int)(int1 / int2);
				}
				protocol_calc.inResult = int_result;
				
				printf("test/calculate %d %s %d = %d\n", int1, type_temp, int2, type_temp,int_result);//出现 int_result==-14832 的原因是 int_result==0；
			}
			else {
				double1 = protocol_calc.flValue1;
				double2 = protocol_calc.flValue2;
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
					char double_result_temp[100];
					sprintf(double_result_temp, "% 8.8f", double_result);
					double_result = atof(double_result_temp);
				}
				protocol_calc.flResult = double_result;
				printf("test/calculate's result is: %8.8g\n", double_result);
				printf("test/calculate %8.8g %s %8.8g = %8.8g\n", double1, type_temp, double2,  double_result);
			}

			//测试ip是否对应的代码,平时该注释
			//protocol_calc.id = 81;

			send_num = sendto(sock_fd, point_protocol_calc, sizeof(protocol_calc), 0, (struct sockaddr *)&addr_serv, len);
			printf("send protocol: %d %d %d %d %d %d %d %d %.2f %.2f %.2f\n", protocol_calc.type, protocol_calc.major_version, protocol_calc.minor_version, protocol_calc.id, protocol_calc.arith, protocol_calc.inValue1,
				protocol_calc.inValue2, protocol_calc.inResult, protocol_calc.flValue1, protocol_calc.flValue2, protocol_calc.flResult);
			//printf("-send protocol result-%d %d %d %d %d %d %d %d %.2f %.2f %.2f\n", point_protocol_calc->type, point_protocol_calc->major_version, point_protocol_calc->minor_version, point_protocol_calc->id, point_protocol_calc->arith,
			//	point_protocol_calc->inValue1, point_protocol_calc->inValue2, point_protocol_calc->inResult, point_protocol_calc->flValue1, point_protocol_calc->flValue2, point_protocol_calc->flResult);
			if (send_num < 0)
			{
				perror("sendto error:");
				exit(1);
			}

			//测试接受数据代码
			recv_num = 0;
			recv_num = recvfrom(sock_fd, point_client_message_calc, sizeof(client_message_calc), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);
			start = clock();
			incount = 0;
			while (incount < 6)
			{

				//表示收到数据（数据位长大于0）
				if (recv_num > 0) {
					break;
				}
				//表示未收到数据
				else {
					//处理方式1：如果重传次数在3次及以内且等待时间小于10秒
					if (incount < 6) {
						now = clock();
						if ((now - start) / CLOCKS_PER_SEC > (2 + 2 * incount)) {
							incount++;
							//重传请求
							if (incount < 3) {//还是二次重传之内
								send_num = sendto(sock_fd, point_protocol_calc, sizeof(protocol_calc), 0, (struct sockaddr *)&addr_serv, len);
								printf("client have resend the calc_protocol\n");
							}
							else {
								printf("waiting\n");
							}
							//测试代码
							/*if(incount==4){
								recv_num = recvfrom(sock_fd, point_client_message_calc, sizeof(client_message_calc), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);
							}*/
						}
						//正常用时不用注释
						recv_num = recvfrom(sock_fd, point_client_message_calc, sizeof(client_message_calc), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);
					}
				}
			}

			//incount==6表示已经等待10秒且没有结果
			if (incount == 6) {
				printf("client second send calc_protocol have wrong,close the connect\n");
				close(sock_fd);
				break;
			}
			
			printf("get message: %d %d %d %d %d\n", client_message_calc.type, client_message_calc.message, client_message_calc.protocol, client_message_calc.major_version, client_message_calc.minor_version);
			//printf("--get message--%d %d %d %d %d\n", point_client_message_calc->type, point_client_message_calc->message, point_client_message_calc->protocol, point_client_message_calc->major_version, point_client_message_calc->minor_version);
			if (client_message_calc.type != 3) {
				if (client_message_calc.message == 1) {
					printf("OK\n");
				}
				else if (client_message_calc.message == 2) {
					printf("ERROR\n");
				}
				else {
					printf("Not applicable/availible\n");
				}
			}
			else {
				printf("connect ip is wrong\n");
			}

			count++;
		}
		else {
			printf("break the connect");
			close(sock_fd);
			break;
		}
		
	}
	
	return 0;
}