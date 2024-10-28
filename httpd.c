#include <stdio.h>

//����ͨ�����ͷ�ļ������ļ�
#include <WinSock2.h>
#pragma comment(lib,"WS2_32.lib")

void ErrorDie(const char* str) {
	perror(str);
	exit(1);
}

//ʵ�������ʼ��
//����ֵ���׽��֣��������ˣ�
//�˿�port����0�Զ�������ö˿�
int StartUp(unsigned short* port) {
	/*
	* 1.����ͨ�ų�ʼ�� Windowsϵͳ��Ҫ��ʼ����Linux����Ҫ
	* 2.�����׽���
	* 3.�����׽������ԣ����ö˿ڿɸ��ã��ɺ��ԣ�
	* 4.���׽��ֺ������ַ
	* 5.��̬����˿ںţ��ɺ��ԣ�
	* 6.������������
	*/
	
	//1.����ͨ�ų�ʼ��
	WSADATA data;
	int ret = WSAStartup(MAKEWORD(1, 1), &data);//����0Ϊ�ɹ�
	if (ret) { //���ز�Ϊ0��ֵΪʧ��
		ErrorDie("WSAStartup");
	}

	//2.�����׽���
	int serverSocket = socket(PF_INET,//�׽������ͣ������׽��֣��ļ��׽��֣��˴�Ϊ����
		SOCK_STREAM,//������
		IPPROTO_TCP);
	if (serverSocket < 0) {
		//��ʾ������
		ErrorDie("serverSocket");
	}

	//3.�����׽������ԣ����ö˿ڿɸ���
	int opt = 1;
	ret = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) & opt, sizeof(opt));
	if (ret < 0) {
		ErrorDie("setsocketopt");
	}



	//4.���׽��ֺ������ַ
	  //���÷����������ַ
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverSocket));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(*port);//host to net short
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);//host to net long

	
	ret = bind(serverSocket, (struct socketaddr*) & serverAddr, sizeof(serverAddr));
	if (ret < 0) {
		ErrorDie("bind");
	}

	//��̬����˿�
	int nameLen = sizeof(serverAddr);
	if (*port == 0) {
		ret = getsockname(serverSocket, (struct sockaddr*)&serverAddr, &nameLen);
		if (ret < 0) {
			ErrorDie("getsockname");
		}
		*port = serverAddr.sin_port;
	}

	//6.������������
	ret = listen(serverSocket, 5);
	if (ret < 0) {
		ErrorDie("listen");
	}

	return serverSocket;
}

//�����û���Ϣ���̺߳���
DWORD WINAPI accept_request(LPVOID arg) {

}

int main() {
	//��ʼ��
	unsigned short port = 0;//�˿ںţ�0-65535
	int serverSocket = StartUp(&port);
	printf("httpd�������������ڼ����˿�: %d ", port);

	struct sockaddr_in clint_addr;
	int client_addr_len = sizeof(clint_addr);
	while (1) {
		int client_sock = accept(serverSocket, &clint_addr, &client_addr_len);//accept����ֻ�������з���ʱ�����µ��׽��֣�accept��������
		if (client_sock < 0) {
			ErrorDie("accept");
		}

		//�����߳�
		DWORD threadId = 0;
		CreateThread(0, 0, accept_request, client_sock, 0, &threadId);

	}

	closesocket(serverSocket);
	return 0;


}