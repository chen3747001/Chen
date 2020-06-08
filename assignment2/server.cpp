#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include<string.h>
#include <string>
#include <stdlib.h>
#include<time.h>
#include"sstream"

#define random(x) (rand()%x)
#define SERV_PORT   8000
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
	//传值时不能直接调用，要用指针
	calcProtocol protocol_calc;
	calcProtocol *point_protocol_calc=&protocol_calc;
	char get_protocol[500];
	
	//printf("--begin protocol--%d %d %d %d %d %d %d %d %.2f %.2f %.2f\n", point_protocol_calc->type, point_protocol_calc->major_version, point_protocol_calc->minor_version, point_protocol_calc->id, 
	//	point_protocol_calc->arith, point_protocol_calc->inValue1,point_protocol_calc->inValue2, point_protocol_calc->inResult, point_protocol_calc->flValue1, point_protocol_calc->flValue2, point_protocol_calc->flResult);


	calcMessage message_calc;
	calcMessage *point_message_calc = &message_calc;
	char get_message[500];
	
	//printf("----%d %d %d %d %d\n", point_message_calc->type, point_message_calc->message,point_message_calc->protocol,point_message_calc->major_version, point_message_calc->minor_version);
	/* sock_fd --- socket文件描述符 创建udp套接字*/
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd < 0)
	{
		perror("socket");
		exit(1);
	}
	
	struct sockaddr_in addr_serv;
	int len;
	memset(&addr_serv, 0, sizeof(struct sockaddr_in));  
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port = htons(SERV_PORT);
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);  
	len = sizeof(addr_serv);

	
	if (bind(sock_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0)
	{
		perror("bind error:");
		exit(1);
	}


	int recv_num;
	int send_num;
	int count = 0;
	int type_use;
	int int1, int2, int_result;
	double double1, double2, double_result;
	char double1_temp[100];
	char double2_temp[100];
	char double_result_temp[100];
	struct sockaddr_in addr_client;
	string type[8] = { "iadd","isub","imul","idiv","fadd","fsub","fmul","fdiv" }; 
	int ip_use=80;

	while (1)
	{
		printf("\n\n=====server wait======\n");
		count = 0;

		//初始化对应calc_message,calc_protocol
		protocol_calc.type = 1;
		protocol_calc.major_version = 1;
		protocol_calc.minor_version = 2;
		protocol_calc.id = 80;
		protocol_calc.arith = 1;
		protocol_calc.inValue1 = 0;
		protocol_calc.inValue2 = 0;
		protocol_calc.inResult = 0;
		protocol_calc.flValue1 = 0;
		protocol_calc.flValue2 = 0;
		protocol_calc.flResult = 0;
		printf("--begin protocol--%d %d %d %d %d %d %d %d %.2f %.2f %.2f\n", protocol_calc.type, protocol_calc.major_version, protocol_calc.minor_version, protocol_calc.id, protocol_calc.arith,
			protocol_calc.inValue1, protocol_calc.inValue2, protocol_calc.inResult, protocol_calc.flValue1, protocol_calc.flValue2, protocol_calc.flResult);

		message_calc.type = 22;
		message_calc.message = 0;
		message_calc.protocol = 17;
		message_calc.major_version = 1;
		message_calc.minor_version = 0;
		//memset(&message_calc, 0, sizeof(calcMessage));

		printf("--begin message--%d %d %d %d %d\n", message_calc.type, message_calc.message, message_calc.protocol, message_calc.major_version, message_calc.minor_version);

		//创建随机数，进行计算
		srand((int)time(0));
		type_use = random(8);
		protocol_calc.arith = type_use;
		if (type_use < 4) {
			int1 = random(100);
			int2 = random(100);
			protocol_calc.inValue1 = int1;
			protocol_calc.inValue2 = int2;
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
		else if (type_use < 8) {
			double1 = (double)rand() / (double)(RAND_MAX / 100.0);
			double2 = (double)rand() / (double)(RAND_MAX / 100.0);
			
			sprintf(double1_temp, "% 8.8f", double1);
			sprintf(double2_temp, "% 8.8f", double2);
			double1 = atof(double1_temp);
			double2 = atof(double2_temp);
			//printf("%8.8g %8.8g\n", double1, double2);
			
			protocol_calc.flValue1 = double1;
			protocol_calc.flValue2 = double2;
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
			}
			sprintf(double_result_temp, "% 8.8f", double_result);
			double_result = atof(double_result_temp);
			printf("test/calculate's result is: %8.8g\n", double_result);
		}

		printf("\ncreate new values to calculate:\n");
		char type_temp[100];
		strcpy(type_temp, type[type_use].c_str());
		printf("type: %s (%d) || %d  %d \n     or %8.8g %8.8g\n", type_temp,type_use, int1, int2, double1, double2);

		while (1)
		{
			//根据count控制，判断发生次数
			//第一次收发,接受连接请求，发送任务
			if (count == 0) {

				
				recv_num = recvfrom(sock_fd, point_message_calc, sizeof(message_calc), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);
				printf("get message: %d %d %d %d %d\n", message_calc.type, message_calc.message, message_calc.protocol, message_calc.major_version, message_calc.minor_version);
				if (recv_num < 0)
				{
					perror("recvfrom error:");
					exit(1);
				}

				send_num = sendto(sock_fd, point_protocol_calc, sizeof(protocol_calc), 0, (struct sockaddr *)&addr_client, len);
				printf("send protocol: %d %d %d %d %d %d %d %d %.2f %.2f %.2f\n", protocol_calc.type, protocol_calc.major_version, protocol_calc.minor_version, protocol_calc.id, protocol_calc.arith,
					protocol_calc.inValue1, protocol_calc.inValue2, protocol_calc.inResult, protocol_calc.flValue1, protocol_calc.flValue2, protocol_calc.flResult);
				
				count++;
			}
			//第二次收发，确认工作结果，放回结果。
			else if(count==1) {
				printf("\n----sencond time is begin:\n");
				recv_num = recvfrom(sock_fd, point_protocol_calc, sizeof(protocol_calc), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);
				printf("get protocol: %d %d %d %d %d %d %d %d %.2f %.2f %.2f\n", protocol_calc.type, protocol_calc.major_version, protocol_calc.minor_version, protocol_calc.id, protocol_calc.arith,
					protocol_calc.inValue1, protocol_calc.inValue2, protocol_calc.inResult, protocol_calc.flValue1, protocol_calc.flValue2, protocol_calc.flResult);

				//如果返回的对应ip正确
				if (ip_use == protocol_calc.id) {
					if ((type_use < 4 && int_result == protocol_calc.inResult) || (type_use > 3 && double_result == protocol_calc.flResult)) {
						message_calc.message = 1;
					}
					else {
						message_calc.message = 2;
					}
				}
				else {//返回对应ip不正确
					message_calc.type = 3;
					message_calc.message = 2;
				}

				
				send_num = sendto(sock_fd, point_message_calc, sizeof(message_calc), 0, (struct sockaddr *)&addr_client, len);
				printf("send message: %d %d %d %d %d\n", message_calc.type, message_calc.message, message_calc.protocol, message_calc.major_version, message_calc.minor_version);
				//printf("-send message--%d %d %d %d %d\n", point_message_calc->type, point_message_calc->message, point_message_calc->protocol, point_message_calc->major_version, point_message_calc->minor_version);
				
				count++;
			}
			else {
				printf("break the connect\n");
				break;
			}
		
		}
	}

	close(sock_fd);

	return 0;
}