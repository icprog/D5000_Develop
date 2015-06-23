#include<iostream>
#include "dms_tcp.h"

using namespace std;
int fd = -1;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "参数太少！\r\n用法:test_dms [filename]" << endl;
		cout << "filename:要发送给服务器的文件的名字" << endl;
		return -1;
	}

	sigignore(SIGPIPE);

	TCPClass *tcp = new TCPClass();

	int ret = tcp->Cli_Init(SERVER_PORT, "127.0.0.1");

	if (ret != -1)
	{
		FILE *fp = fopen(argv[1], "r");

		char buf[4096] = { 0 };

		if (fp != NULL)
		{
			cout << "===========发送给服务器的数据=============" << endl;
			while (fgets(buf, 4096, fp) > 0)
			{
				cout << buf << endl;
				if (tcp->Send(tcp->GetSocketfd(), buf, strlen(buf)) <= 0)
				{
					cout << "发送数据失败，" << strerror(errno) << endl;
					delete tcp;
					return 1;
				}
			}

			cout << "===========从服务器接收到的数据=============" << endl;
			while (true)
			{
				if (tcp->ReadLine(tcp->GetSocketfd(), buf, 4096) <= 0)
				{
					cout << "接收数据失败，" << strerror(errno) << endl;
					delete tcp;
					return 2;
				}
				cout << buf << endl;

				if (strstr(buf, "</Message>") != NULL)
					break;
			}
		}
		else
		{
			cout << "打开文件" << argv[1] << "失败，" << strerror(errno) << endl;
			delete tcp;
			return 3;
		}
	}

	delete tcp;

	return 0;
}
