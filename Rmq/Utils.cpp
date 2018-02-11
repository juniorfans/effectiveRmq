#include <fstream>
using std::ifstream;
using std::ios_base;
#include <string>
using std::string;
#include <windows.h>

#include <iostream>
using namespace::std;
#include <memory>
#include <limits>
#include <cassert>
#include "Utils.h"

/************************************************************************/
/* 
数字以空格或\n区分开
1 3 4 9 
4 5 6 2 
*/
/************************************************************************/
void parseIntStream(char *buf,int bufLen,int *numbers,int maxNumLen,int *realNumLen)
{
	int i;
	numbers[i=0] = 0;
	for (char *p=buf;*p && p - buf < bufLen;p++)
	{
		if (*p == ' ' || *p == '\n')
		{
			++ i;
			if(i == maxNumLen)
			{
				-- i;
				break;
			}

			numbers[i] = 0;
		}
		else
		{
			if(*p != '\r')
				numbers[i] = numbers[i] * 10 + *p - '0';
		}
	}
	if(NULL != realNumLen)
		*realNumLen = i + 1;
}


/************************************************************************/
/* 
从文件 fileName 中读入数字，一行数字表示一个事务；
返回：文件中的数字。各个事务以 FS_GROUP_END分隔

realIntsCounts: 数组真实的大小
*/
/************************************************************************/
void fileIntsEx(const string& fileName,int *ints,int readMaxInts,int *realIntsCounts)
{
	HANDLE hFile = 
		CreateFileA((fileName.c_str())
		, GENERIC_READ
		, FILE_SHARE_READ
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{   
		printf("Could not open file (error %d)\n", GetLastError()); 
	}

	int size = GetFileSize(hFile, NULL);

	HANDLE hMapFile =
		CreateFileMapping(hFile
		, NULL
		, PAGE_READONLY 
		, 0
		, size
		, NULL);



	char* pBuff
		= (char*)MapViewOfFile(hMapFile
		, FILE_MAP_READ 
		, 0
		, 0
		, 0);

	parseIntStream(pBuff,size,ints,readMaxInts,realIntsCounts);
	UnmapViewOfFile(pBuff);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
}

void intsToFile(int *ints,int len,const string &fileName)
{
	ofstream file(fileName.c_str());
	if(file.is_open())
	{
		for(int i = 0;i < len;++ i)
		{
			file << ints[i] << '\n';
		}
		file.close();
	}
}



int radix(int num)
{
	if(num < 0)
	{
		num = -num;
	}
	int retDix = 1;
	long long ret = 1;//这里必须要定义多于 32 位的整数，如对于 INT_MAX 来说，ret 不断左移直到小于0，再到小于0.
	while (ret <= num)
	{
		ret <<= 1;
		++ retDix;
	}
	return --retDix;//ret >> 1;
}