#include <iostream>
using namespace::std;
#include <memory>
#include <limits>
#include <cassert>
#define BLOCK_SIZE 20
#include "SectionMin.h"
#include "NormalRMQ.h"

extern int minValue(int *sorArray,int start,int end,int *index = NULL);

NormalRMQ::~NormalRMQ(void)
{
	delete[] blockMinIndexes;
	delete[] blockMins;
	delete sm;
}

NormalRMQ::NormalRMQ(int *sorArray,int len)
:_sorArray(sorArray),_len(len)
{
	init();
}

void NormalRMQ::init()
{
	blockCounts  = _len / BLOCK_SIZE + ((0 != (_len % BLOCK_SIZE)) ? 1 : 0);
	blockMinIndexes = new int[blockCounts];
	memset(blockMinIndexes,-1,sizeof(int) * blockCounts);
	blockMins = new int[blockCounts];
	memset(blockMins,0,sizeof(int) * blockCounts);


	int minIndex = -1;
	int i ;
	for(i = 0;i < _len;i += BLOCK_SIZE)
	{
		blockMins[i / BLOCK_SIZE] = minValue(_sorArray,i,i+BLOCK_SIZE - 1,&minIndex);
		blockMinIndexes[i / BLOCK_SIZE] = minIndex;
	}
	if(_len != (i -= BLOCK_SIZE))
	{
		assert(blockCounts-1 == i / BLOCK_SIZE);
		blockMins[blockCounts - 1] = minValue(_sorArray,i,_len - 1,&minIndex);
		blockMinIndexes[blockCounts - 1] = minIndex;
	}
	sm = new SectionMin(blockMins,blockCounts,blockCounts - 1);
}

#ifndef SET_RESULT_IF_NOT_NULL
#define SET_RESULT_IF_NOT_NULL(theIndexPointer,theIndex) if(NULL!=(theIndexPointer)) *(theIndexPointer)=(theIndex)
#endif

int NormalRMQ::queryMin(int s,int e,int *index)
{
	assert(s <= e);
	int sBlock = s / BLOCK_SIZE;
	int eBlock = e / BLOCK_SIZE;
	if(sBlock == eBlock)
	{
		int theMin = _sorArray[s],theIndex = s;
		for(int i = s + 1;i <= e;++ i)
		{
			if(_sorArray[i] < theMin)
			{
				theIndex = i;
				theMin = _sorArray[i];
			}
		}
		SET_RESULT_IF_NOT_NULL(index,theIndex);
		return theMin;
	}
	int min1,min2,min3;
	min1 = min2 = min3 = INT_MAX;
	int index1,index2,index3;
	//s -> 当前块最末
	int toEnd = (sBlock + 1) * BLOCK_SIZE;
	index1 = s;
	min1 = _sorArray[index1];
	for(int i = s +1;i < toEnd;++ i)
	{
		if(_sorArray[i] < min1)
		{
			min1 = _sorArray[i];
			index1 = i;
		}
	}
	//sBlock + 1 -> eBlock - 1
	if(sBlock + 1 <= eBlock - 1)
	{
		min2 = sm->queryEx(sBlock+1,eBlock-1,&index2);
		//查询第 index2 块中最小值的索引
		int ss = index2 * BLOCK_SIZE;
		int ee = ss + BLOCK_SIZE;
		index2 = ss;
		int curMin = _sorArray[ss];
		for (int i = ss + 1;i < ee;++ i)
		{
			if(_sorArray[i] < curMin)
			{
				curMin = _sorArray[i];
				index2 = i;
			}
		}
		assert(curMin == min2);
	}
	//eBlock 首 -> e
	int sStartPos = eBlock * BLOCK_SIZE;
	toEnd = e + 1;
	index3 = sStartPos;
	min3 = _sorArray[index3];
	for(int i = sStartPos +1;i < toEnd;++ i)
	{
		if(_sorArray[i] < min3)
		{
			min3 = _sorArray[i];
			index3 = i;
		}
	}

	SET_RESULT_IF_NOT_NULL(index,min1 <= min2 ? (min1 <= min3 ? index1 : index3) : (min2 <= min3 ? index2 : index3));
	return min1 <= min2 ? (min1 <= min3 ? min1 : min3) : (min2 <= min3 ? min2 : min3);
}

