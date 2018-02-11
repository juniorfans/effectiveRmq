#include <stdio.h>
#include <math.h>
#include <string>
using std::string;
#include <ctime>
#include <cassert>
#include "SectionMin.h"

	inline int min(int a ,int b)
	{
		return a < b ? a : b;
	}

	inline static int lowSqrt(int num)
	{
		int i = 0;
		while(0 != (num /= 2))
		{
			++ i;
		}
		return i;
	}

	SectionMin::SectionMin(int *height,int len,int maxQuerySection)
	{
		_sorArray = height;
		_len = len;
		_maxQuerySection = (0 == maxQuerySection) ? _len : maxQuerySection;
		_maxKLevel = lowSqrt(_maxQuerySection);
		init();
	}

	SectionMin::~SectionMin()
	{
		memoryDestroy();
	}

	int SectionMin::query(int start,int end,int *index)
	{
		if(start == end)
		{
			SET_RESULT_IF_NOT_NULL(index,start);
			return INDEX_TO_RESULT(start);
		}
		assert(end - start<= _maxQuerySection);
		int retMin = INT_MAX;
		int curIndex = start;
		int k;
		int curMinIndex = -1;
		while(end != curIndex)
		{
			k = lowSqrt(end - curIndex);//start -> end �൱�ڼ��� start ���� end - start�������ٰ��� start ��Ԫ�ص���Сֵ
			curMinIndex = *twoLevelBucket(curIndex,k);
			if(retMin > INDEX_TO_RESULT(curMinIndex))
			{
				retMin = INDEX_TO_RESULT(curMinIndex);
				SET_RESULT_IF_NOT_NULL(index,curMinIndex);
			}
			curIndex += (1 << k);
		}
		return retMin;
	}

	//��ѯ start(inclusive) ~ end (inclusive)֮�����Сֵ��start -> end �������С��������
	int SectionMin::queryEx_NoLimit(int start,int end,int *index)
	{
		if(end - start <= _maxQuerySection)
			return queryEx(start,end,index);
		if(end - start > 3 * _maxQuerySection)
		{
			printf("end - start is too big : %d\r\n",end - start);
		}
		if(end - start > _maxQuerySection)
		{
			int minmin = INT_MAX;
			int i = start;
			int curEnd = 0;
			int retIndex = -1;
			do
			{
				curEnd = i + _maxQuerySection;
				int curMin = queryEx(i,curEnd > end ? end : curEnd,&retIndex);
				if(curMin < minmin)
				{
					minmin = curMin;
					SET_RESULT_IF_NOT_NULL(index,retIndex);
				}
				i += _maxQuerySection;
			}while (curEnd < end);
			return minmin;
		}
	}
	
	//��ѯ start(inclusive) ~ end (inclusive)֮�����Сֵ��start -> end �������С�� _maxQuerySection ������
	int SectionMin::queryEx(int start,int end,int *index)
	{
		if(start == end)
		{
			SET_RESULT_IF_NOT_NULL(index,start);
			return INDEX_TO_RESULT(start);
		}
		assert(end - start<= _maxQuerySection);//start ~ end ��(end - start + 1���ʵ���ϣ����Դ��� _maxQuerySection + 1 ���򣬹�����ʽ)
		int k = lowSqrt(end - start);
		int retIndex = MIN_INDEX(*twoLevelBucket(start,k),*twoLevelBucket(end - (1<<k),k));
		SET_RESULT_IF_NOT_NULL(index,retIndex);
		return INDEX_TO_RESULT(retIndex);
		//return min(*twoLevelBucket(start,k),*twoLevelBucket(end - (1<<k),k));// rmq[start][k],rmq[end - (1<<k)][k]);
	}
	/************************************************************************/
	/* 
	0 : 0 1 2 ... _sqrt
	1 : 0 1 2 ... _sqrt
	...
	...
	_len-2 : 0 1 2 ... _sqrt
	*/
	/************************************************************************/
	void SectionMin:: memoryConstruct()
	{
		rmq = new int[(_len - 1) * (_maxKLevel + 1)];
		memset(rmq,0,sizeof(int) * (_len - 1) * (_maxKLevel + 1));
	}
	void SectionMin:: memoryDestroy()
	{
		delete[] rmq;
		rmq = NULL;
	}
	inline int * SectionMin ::oneLevelBucket(int element)
	{
		return rmq + (element * (_maxKLevel + 1));
	}
	inline int * SectionMin ::twoLevelBucket(int element,int k)
	{
		return oneLevelBucket(element) + k;
	}

	void SectionMin:: toString()
	{
		for(int i = 0;i < _len;++ i)
		{
			for(int k = 0;k < _maxKLevel;++k)
			{
				//printf("%d ",*(*(rmq + i) + k));
				printf("%d ",*twoLevelBucket(i,k));
			}
			printf("\r\n");
		}
	}
	
	//��i��Ԫ�غͺ��� 2^k ��Ԫ�ص���Сֵ
	void SectionMin::init()
	{
		memoryConstruct();
		int k = 0;
		for(int i = 0;i < _len - 1;++ i)
		{
			*twoLevelBucket(i,k) = MIN_INDEX(i,i+1);
			//*twoLevelBucket(i,k) = _sorArray[i] <= _sorArray[i+1] ? i : i+1;//min(_sorArray[i],_sorArray[i + 1]);
		}

		for(k = 1;k <= _maxKLevel;++k)
		{
			int sec = 1 << k;
			for(int i = 0;i < _len - 1;++ i)
			{
				if(i + sec >= _len)
				{
					break;
				}
				else
				{
					//�� M[i,k] ��ʾ i ���� 2^k ��Ԫ����ֵ
					//������ M[i,k-1] �� M[i+2^(k-1), k-1] ����ֵ
					int index1 = *twoLevelBucket(i,k-1);
					int index2 = *twoLevelBucket(i + (sec>>1),k - 1);
					*twoLevelBucket(i,k) = MIN_INDEX(index1,index2);
					//*twoLevelBucket(i,k) = _sorArray[index1] <= _sorArray[index2] ? index1 : index2;//min(*twoLevelBucket(i,k-1),*twoLevelBucket(i + (sec>>1),k - 1));//i�ͺ���2^k��Ԫ����Сֵ����i����2^(k-1)��Ԫ����Сֵ��i+2^(k-1)����2^(k-1)��Ԫ�ص���Сֵ����Сֵ
				}
			}
		}
	}
	void SectionMin::showRMQ()
	{
		printf("the rmq data : \r\n");
		for(int i = 0;i < _len - 1;++ i)
		{
			for(int k = 0;k <= _maxKLevel;++ k)
			{
				printf("%d ",*twoLevelBucket(i,k));
			}printf("\r\n");
		}
	}