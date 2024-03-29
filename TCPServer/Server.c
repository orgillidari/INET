#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

//网络连接头文件和库
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "ws2_32.lib")
#endif

int main(int argc, char** argv)
{
	int ret;

	//初始化网络环境
#ifdef _WIN32
	WSADATA oWsaData;
	WSAStartup(MAKEWORD(2, 2), &oWsaData);
#endif

	//创建服务器Socket
	SOCKET oServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (oServer == INVALID_SOCKET) {
		goto FAILED;
	}

	//绑定端口
	struct sockaddr_in oServerAddr;
	oServerAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	oServerAddr.sin_family = AF_INET;
	oServerAddr.sin_port = htons(12345);
	int nServerAddrLen = sizeof(oServerAddr);

	ret = bind(oServer, (const struct sockaddr*)&oServerAddr, nServerAddrLen);
	if (ret != 0) {
		goto FAILED;
	}

	//开启监听
	ret = listen(oServer, 1);

	while (1) {

		//接收客户端连接
		struct sockaddr_in oClientAddr;
		int nClientAddrLen = sizeof(oClientAddr);
		SOCKET oClient = accept(oServer, (struct sockaddr*)&oClientAddr, &nClientAddrLen);
		printf("New client %s：%d\n", inet_ntoa(oClientAddr.sin_addr), ntohs(oClientAddr.sin_port));

		//收发数据
        char pBuffer[128];
        memset(pBuffer, 0, 128);
		int nRecvLen = recv(oClient, pBuffer, 128, 0);
		printf("nRecvLen = %d; pBuffer = %s\n", nRecvLen, pBuffer);

		int nSendLen = send(oClient, pBuffer, nRecvLen, 0);
		printf("nSendLen = %d\n", nSendLen);

		//关闭Socket
		closesocket(oClient);
	}

FAILED:
	//关闭Socket
	if (oServer != INVALID_SOCKET) {
		closesocket(oServer);
	}

	//销毁网络环境
#ifdef _WIN32
	WSACleanup();
#endif

	system("pause");
	return 0;
}