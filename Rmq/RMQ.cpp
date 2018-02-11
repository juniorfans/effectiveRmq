#include <iostream>
using namespace::std;
#include <memory>
#include <limits>
#include <cassert>

#include "RMQ+1-1.h"
#include "CartesianTree.h"
#include "RMQ.h"

RMQ::RMQ(int *theArray,int theLen)
:sorArray(theArray),arrayLen(theLen)
{
	init();
}

RMQ::~RMQ(void)
{
	if(NULL != theRmq1_1)
	{
		delete theRmq1_1;
		theRmq1_1 = NULL;
	}
	if(NULL != eulerPath)
	{
		delete[] eulerPath;
		eulerPath = NULL;
	}
	if(NULL != indexPath)
	{
		delete[] indexPath;
		indexPath = NULL;
	}

	if(NULL != firstOccInEulerPath)
	{
		delete[] firstOccInEulerPath;
		firstOccInEulerPath = NULL;
	}
}

int RMQ::queryMin(int s,int e,int *index)
{
	int sInEulerPath = firstOccInEulerPath[s];
	int eInEulerpath = firstOccInEulerPath[e];
	if(sInEulerPath > eInEulerpath)
	{
		int tmp = sInEulerPath;
		sInEulerPath = eInEulerpath;
		eInEulerpath = tmp;
	}
	int eulerIndex = -1;
	theRmq1_1->queryMin(sInEulerPath,eInEulerpath,&eulerIndex);
	SET_RESULT_IF_NOT_NULL(index,indexPath[eulerIndex]);
	return sorArray[indexPath[eulerIndex]];
}

void RMQ::init()
{
	CartesianTree ct(sorArray,arrayLen);
	eulerPath = ct.getEulerPath();
	indexPath = ct.getIndexArray();
	firstOccInEulerPath = ct.getFirstOccInEulerPath();
	theRmq1_1 = new RMQ1_1(eulerPath,(arrayLen << 1) - 1);	//欧拉序列长度为 2*n - 1
}
