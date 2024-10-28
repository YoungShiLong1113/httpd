#include <stdio.h>

//网络通信相关头文件、库文件
#include <WinSock2.h>
#pragma comment(lib,"WS2_32.lib")

void ErrorDie(const char* str) {
	perror(str);
	exit(1);
}

//实现网络初始化
//返回值：套接字（服务器端）
//端口port：传0自动分配可用端口
int StartUp(unsigned short* port) {
	/*
	* 1.网络通信初始化 Windows系统需要初始化，Linux不需要
	* 2.创建套接字
	* 3.设置套接字属性，设置端口可复用（可忽略）
	* 4.绑定套接字和网络地址
	* 5.动态分配端口号（可忽略）
	* 6.创建监听队列
	*/
	
	//1.网络通信初始化
	WSADATA data;
	int ret = WSAStartup(MAKEWORD(1, 1), &data);//返回0为成功
	if (ret) { //返回不为0的值为失败
		ErrorDie("WSAStartup");
	}

	//2.创建套接字
	int serverSocket = socket(PF_INET,//套接字类型：网络套接字，文件套接字，此处为网络
		SOCK_STREAM,//数据流
		IPPROTO_TCP);
	if (serverSocket < 0) {
		//提示并结束
		ErrorDie("serverSocket");
	}

	//3.设置套接字属性，设置端口可复用
	int opt = 1;
	ret = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) & opt, sizeof(opt));
	if (ret < 0) {
		ErrorDie("setsocketopt");
	}



	//4.绑定套接字和网络地址
	  //配置服务器网络地址
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverSocket));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(*port);//host to net short
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);//host to net long

	
	ret = bind(serverSocket, (struct socketaddr*) & serverAddr, sizeof(serverAddr));
	if (ret < 0) {
		ErrorDie("bind");
	}

	//动态分配端口
	int nameLen = sizeof(serverAddr);
	if (*port == 0) {
		ret = getsockname(serverSocket, (struct sockaddr*)&serverAddr, &nameLen);
		if (ret < 0) {
			ErrorDie("getsockname");
		}
		*port = serverAddr.sin_port;
	}

	//6.创建监听队列
	ret = listen(serverSocket, 5);
	if (ret < 0) {
		ErrorDie("listen");
	}

	return serverSocket;
}

//处理用户信息的线程函数
DWORD WINAPI accept_request(LPVOID arg) {

}

int main() {
	//初始化
	unsigned short port = 0;//端口号：0-65535
	int serverSocket = StartUp(&port);
	printf("httpd服务启动，正在监听端口: %d ", port);

	struct sockaddr_in clint_addr;
	int client_addr_len = sizeof(clint_addr);
	while (1) {
		int client_sock = accept(serverSocket, &clint_addr, &client_addr_len);//accept负责只监听，有访问时创建新的套接字，accept继续监听
		if (client_sock < 0) {
			ErrorDie("accept");
		}

		//创建线程
		DWORD threadId = 0;
		CreateThread(0, 0, accept_request, client_sock, 0, &threadId);

	}

	closesocket(serverSocket);
	return 0;


}