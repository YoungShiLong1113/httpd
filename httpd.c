#include <stdio.h>

//����ͨ�����ͷ�ļ������ļ�
#include <WinSock2.h>
#pragma comment(lib,"WS2_32.lib")

#define PRINTF(str) printf("[%s - %d]"#str"=%s", __func__, __LINE__-5, str);

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
	int serverSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);//�׽������ͣ������׽��֣��ļ��׽��֣��˴�Ϊ���磬//��������//Э��
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

//��ָ���׽��ֶ�ȡһ�У����浽buff������ʵ�ʶ�ȡ�����ֽ�
int get_line(int sock,char* buff,int size) {
	char c = 0;
	int i = 0;
	while (i<size-1 && c!='\n') {
		int n = recv(sock, &c, 1, 0);
		if (n > 0) {
			if (c == '\r') {
				n = recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n') {
					recv(sock, &c, 1, 0);
				}
				else {
					c == '\n';
				}
			}
			buff[i++] = c;
		}
		else {
			c = '\n';
		}
	}
	buff[i] = 0;
	return i;
}



//�����û���Ϣ���̺߳���
//get���󣺿ͻ��������������̶���Դ
/* �����У�GET / HTTP/1.1\n			������ / URL/�汾\n	Ĭ�Ϸ�����ԴĿ¼��index.html
*  ����ͷ��n�У�Host: 127.0.0.1:8080\n	�ؼ���: ֵ\n		������������ַ
*  ���У�\n
*/
DWORD WINAPI accept_request(LPVOID arg) {
	//��ȡһ�����ݣ�����
	char buff[1024];
	int client = arg;
	int numchars = get_line(client,buff,sizeof(buff));
	PRINTF(buff);
	

	
}

int main() {
	//��ʼ��
	unsigned short port = 8000;//�˿ںţ�0-65535
	int serverSocket = StartUp(&port);
	printf("httpd�������������ڼ����˿�: 127.0.0.1:%d\n", port);

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