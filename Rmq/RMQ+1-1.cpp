
#include <iostream>
using namespace::std;
#include <memory>
#include "RMQ+1-1.h"
#include <limits>
#include <cassert>
#include "Utils.h"

Model1_1_MinValue::Model1_1_MinValue(int theL)
{
	L = theL;
	maxOdd = (1 << L) - 1; //最大奇数
	int minValueSize = maxOdd + 1;//(maxOdd - (-maxOdd)) / 2 + 1 正负各一半
	theMinValues = new int[minValueSize];
	for(int i = 0;i < minValueSize;++ i)
	{
		theMinValues[i] = INT_MAX;
	}
	theSubs = new int[minValueSize];
	memset(theSubs,0,sizeof(int) * minValueSize);

	theMinIndexes = new int[minValueSize];
	memset(theMinIndexes,-1,sizeof(int) * minValueSize);

#ifdef VERIFY_MODEL1_1_CREATE
	theMinValues_directyly = new int[minValueSize];
	memset(theMinValues_directyly,0,sizeof(int) * minValueSize);
	theMinIndexs_directyly = new int[minValueSize];
	memset(theMinIndexs_directyly,-1,sizeof(int) * minValueSize);
#endif // BENCHMARK_CMP
	init();
}

Model1_1_MinValue::~Model1_1_MinValue()
{	
	if (NULL != theMinValues)
	{
		delete[] theMinValues;
		theMinValues = NULL;
	}
#ifdef VERIFY_MODEL1_1_CREATE
	if(NULL != theMinValues_directyly)
	{
		delete[] theMinValues_directyly;
		theMinValues_directyly = NULL;
	}
#endif
	if (NULL!= theSubs)
	{
		delete[] theSubs;
		theSubs = NULL;
	}
	if(NULL != theMinIndexes)
	{
		delete[] theMinIndexes;
		theMinIndexes = NULL;
	}
}

void Model1_1_MinValue::init()
{
#ifdef CHECK_RMQ1_1_INIT
	assert(1 == (maxOdd & 1));
#endif // CHECK_INIT
	//5 = 4+2 - 1 则分为两个集合：{4,2} ， {1} 前一个集合表示二进制位为1，后一个值表示为-1.
	//positive 表示前面一个集合的所有值的或，negative 表示后一个集合的或
	int positive = 1,negative = 0;
	addMin(1,0);
	addMin(0,1);	//-1
	int lastNegative2N = 0;
	for(int i = 3;i <= maxOdd;i += 2) 
	{
		//当前奇数比上一个奇数大2，我们利用上一个奇数的二进制得到当前奇数的二进制
		//使用的方法是：将 negative 中的 1 转移到 positive中，此时相当于原来的二进制值加 2，
		//若 negative 中不存在 1，则将negative 中的 2 转移到 positive，将 positive 中的 1 转移到 negative
		//若 negative 中不存在 2，则将negative 中的 4 转移到 positive，将 positive 中的 2,1 转移到 negative
		//...
		//依据的原理是 ：2^n - (2^n-1 + 2 ^n-2 + ... + 2^1 + 2^0) = 1；只需要将 negative 中的2^n转移到 positive，再将
		//positive 中的2^n-1 + 2 ^n-2 + ... + 2^1 + 2^0) 转移到 negative ，就能保证前后两个奇数的值相差 2.

		//引理：从 3 开始的递增2的奇数序列，若 2^n 在 negative 中，则 1,2,4,... 2^(n-1) 一定在 positive 当中。反之亦然。

		//这是基本的思路，有一种特殊情况需要处理：当 negative 为 0 时：
		//设此时 positive 集合的和（外在表现为 positive 的值）为 x (x必定为奇数，因为1，2，4，8...都在 positive 中)，我们先引入一个更高位的数字放入 positive，此值为 x + 1
		//因为现在所有的位都为1，由于 2^n - (2^n-1 + 2 ^n-2 + ... + 2^1 + 2^0) = 1 ，故更高位的数字表示 x + 1
		//设从 positive 转移 b 到 negative，positive 中还剩下 a，此时对应的二进制数字应该是 x + 2（比上一次奇数大2），列下方程：
		//a + b = x+1 + x，a - b = x + 2，解方程：b = (x-1)/2，a = (3x+3)/2
		//即：positive = (3x+3)/2，negative = (x-1)/2。
		//实际上，设 positive = 2^n + 2^(n-1) + ... + 2 + 1; negative = 0; 此时只需要令 positve = 2^(n+1) + 2^n；negative = 2^(n-1) + ... + 2 + 1 即可

		//先处理特殊情况： negative 为空
		if(0 == negative)
		{
			#ifdef CHECK_RMQ1_1_INIT
			assert(0 == ((positive + 1) & positive));//所有位全为 1
			#endif
			int x = positive;
			positive = (x+1)*3 / 2;
			negative = (x-1) / 2;
			lastNegative2N = 0;
			addMin(positive,negative);	
			addMin(negative,positive);	//- positive
		}
		else
		{
			//找到 negative中最低的值为 1 的位 如 negative = {4,16} 则找到 4（第3位）
			int curAdd = 1;
			int n = 0;
			while(curAdd != (negative & curAdd))
			{
				curAdd <<= 1;
				++ n;
			}
			
			//将 2^n 从 negative 转移到 positive
			#ifdef CHECK_RMQ1_1_INIT
			assert(0 == (positive & curAdd));//2^n 若在negative 之中，就必定不能在 positive 集合中。因为一个数字不可能在第 n 位上同时为 1 或 -1
			#endif
			positive |= curAdd;
			negative -= curAdd;

			//将 2^n-1 + 2 ^n-2 + ... + 2^1 + 2^0 从 positive 转移到 negative
			#ifdef CHECK_RMQ1_1_INIT
			assert(curAdd - 1 == ((curAdd - 1) & positive));//验证 2^n-1 ， 2 ^n-2 ， ... ，2^1 ， 2^0都在 positive 之中；
			assert(0 == (negative & (curAdd - 1)));//验证 2^n-1 ， 2 ^n-2 ， ... ，2^1 ， 2^0都不在 negative 之中；
			#endif
			positive -= (curAdd - 1);
			negative |= (curAdd - 1);
			addMin(positive,negative);
			addMin(negative,positive);
		}
		//	printf("%d = %d,%d\r\n",i,positive,negative);
			//cout << i << " = " << positive << " - " << negative<< endl;
			int a = 0;
			++a;
	}
#ifdef VERIFY_MODEL1_1_CREATE
	verify();
#endif
}

#ifdef VERIFY_MODEL1_1_CREATE
void Model1_1_MinValue::depose(int v,int *rpositive,int *rnegative)
{
	bool isNegative = v < 0;
	if(isNegative)
	{
		v = -v;
	}
	int num = lastMin2n(v);
	int positive = num,negative = 0;
	int curSub = positive;
	while (true)
	{
		curSub = positive - negative;
		if(curSub < v)
		{
			positive += (num >>= 1);
		}
		else if(curSub > v)
		{
			negative += (num >>= 1);
		}
		else
		{
			if(1 == num)
			{
				break;
			}
			else
			{
				cout << "error" << endl;
			}
		}
	}
	if(!isNegative)
	{
		*rpositive = positive;
		*rnegative = negative;
	}
	else
	{
		*rpositive = negative;
		*rnegative = positive;
	}
}
#endif

void Model1_1_MinValue:: addMin(int positive,int negative)
{
	minValue(positive,negative);
#ifdef VERIFY_MODEL1_1_CREATE
	minValueDirectly(positive,negative);
#endif
}

//最后一个小于 num 的 2^n
int Model1_1_MinValue:: lastMin2n(int num)
{
	long long ret = 1;//这里必须要定义多于 32 位的整数，如对于 INT_MAX 来说，ret 不断左移直到小于0，再到小于0.
	while (ret <= num)
	{
		ret <<= 1;
	}
	return ret >> 1;
}

/************************************************************************/
/* 
	设 v = positive - negative;	lastOodV = (positive-negative)/2;
	lastOodV 相当于把 positive 和 negative 分别都除以 2。
	若 1 在 positve 中，则 positive / 2 相当于把 positive 中的 1 移除掉。
		举例：
		设 positive = {16,4,1},negative={8,2}，表示：1 -1 1 -1 1，
		分别除以2后变成：{8,2} 和 {4,1}，表示 1 -1 1 -1
		对格局的影响仅在于少了最末尾一个 1,即表示的序列之和比原来小 1
	若 1 在 negative 中，则表示序列之和比原来大 1

	所以 v 表示的序列之和比 v/2 表示的序列之和：大 1(当 1 在 positve 中)；小 1 (当 1 在 negative 中)

	另一方面，由于 odd 和 lastOdd 分别除以 2 对序列的顺序无影响，只是最后少了一个 1 或 -1，则这两个序列表示的序列的最值
	有如下变化：
		[1].最大值 : {1 在 positive 中，则 odd 的最值比 lastOdd 大1；1 在 negative 中，最大值不会加上这个负1，则最值不变
		[2].最小值 : {1 在 positive 中，最小值不会考虑这个正1，则 odd 的最值不变；1 在 negative 中，odd 最值比 lastOdd 小1}
*/
/************************************************************************/
void Model1_1_MinValue::minValue(int positive,int negative)
{
	int v = positive - negative;
	int index = MIN_VALUES_INDEX(v);
	if(1 == v)
	{
		theSubs[index] = 1;
		theMinValues[index] = 1;
		theMinIndexes[index] = 0;
		return;
	}
	else if(-1 == v)
	{
		theSubs[index] = -1;
		theMinValues[index] = -1;
		theMinIndexes[index] = 0;
		return;
	}
	int halfV = (positive >> 1) - (negative >> 1);//注意不能是 (positive-negative)>>1
	
	int lowLevelIndex = MIN_VALUES_INDEX(halfV);
	//先算出 1 在 positive 还是在 negative 里面
	//目前的 +1 -1 序列相对于原来的序列在末尾加了一个 +1 或 -1
	if(1 == (positive & 1))
	{
		theSubs[index] = theSubs[lowLevelIndex] + 1;
	}
	else
	{
		theSubs[index] = theSubs[lowLevelIndex] - 1;
	}

	if(theSubs[index] < theMinValues[lowLevelIndex])
	{
		theMinValues[index] = theSubs[index];
		int theVRadix = radix(v);
		theMinIndexes[index] = theVRadix - 1;
	}
	else
	{
		theMinValues[index] = theMinValues[lowLevelIndex];
		theMinIndexes[index] = theMinIndexes[lowLevelIndex];
	}
}




/*
void Model1_1_MinValue::minValue(int positive,int negative,int theSubValue)
{
	int v = positive - negative;
	int index = MIN_VALUES_INDEX(v);
	if(1 == v)
	{
		theMinValues[index] = 1;
		return;
	}
	else if(-1 == v)
	{
		theMinValues[index] = -1;
		return;
	}
	int halfV = (positive >> 1) - (negative >> 1);//注意不能是 (positive-negative)>>1

	int lowLevelIndex = MIN_VALUES_INDEX(halfV);
	//先算出 1 在 positive 还是在 negative 里面
	//目前的 +1 -1 序列相对于原来的序列在末尾加了一个 +1 或 -1
	if(1 == (positive & 1))
	{
		theSubs[index] = theSubs[lowLevelIndex] + 1;
	}
	else
	{
		theSubs[index] = theSubs[lowLevelIndex] - 1;

	}

	cout << "theSubs : " << theSubs[index] << endl;

	if(theSubs[index] < theMinValues[lowLevelIndex])
	{
		theMinValues[index] = theSubs[index];
	}
	else
	{
		theMinValues[index] = theMinValues[lowLevelIndex];
	}
}

*/


#ifdef VERIFY_MODEL1_1_CREATE
int Model1_1_MinValue::minValueDirectly(int positive,int negative)
{
	int v = positive - negative;
	int index = MIN_VALUES_INDEX(v);
	//特殊情况
	if(0 == negative)//positive 上所有的位都为 1，最小值为1
	{
		theMinValues_directyly[index] = 1;
		theMinIndexs_directyly[index] = 0;
		return 1;
	}
	//negative 上所有位都为 1 时则要循环计算有多少个 1才能决定最小值，转入下面的处理即可
	
	//正常处理
	int theMin = INT_MAX;
	int maxV = positive > negative ? positive : negative;
	int lm2n = lastMin2n(maxV);
	int sub = 0;
	int occ = 0;
	while (0 != lm2n)
	{
		if(0 != (positive & lm2n))
		{
			sub += 1;
		}
		else if(0 != (negative & lm2n))
		{
			sub -= 1;
		}
		else
		{
			cout << "error " << endl;
		}
		
		if(theMin > sub)
		{
			theMinIndexs_directyly[index] = occ;
			theMin = sub;
		}
		lm2n >>= 1;
		++ occ;
	}
	theMinValues_directyly[index] = theMin;
	return theMin;
}

#endif

#ifdef VERIFY_MODEL1_1_CREATE
void Model1_1_MinValue::verify()
{
	for(int i = - maxOdd;i < maxOdd;i += 2)
	{
		int index = MIN_VALUES_INDEX(i);
		int m1 = theMinValues[index];
		int m2 = theMinValues_directyly[index];
		int index1 = theMinIndexes[index] ,index2 = theMinIndexs_directyly[index];
		int positive,negative;
		depose(i,&positive,&negative);
	//	int m3 = minValueDirectly(positive,negative);
		if(m1 == m2 && index1 == index2/* && m2 == m3*/)
		{
			
		}
		else
		{
			cout << "error" << endl;
			cout << i << ",min : " << m1 << "," << m2 << "," <</* m3 << " = " << */positive << "," << negative << endl;
			system("pause");
		}
	
	}
	cout << "verify model1_1_create ok" << endl;
}

#endif
int Model1_1_MinValue::queryMin(int num,int *index)
{
	assert(1 == (num & 1));
	int theIndex = MIN_VALUES_INDEX(num);
	SET_RESULT_IF_NOT_NULL(index,theMinIndexes[theIndex]);
	return theMinValues[MIN_VALUES_INDEX(num)];
}


#undef MIN_VALUES_INDEX
//end of RMQ1_1Impl
//////////////////////////////////////////////////////////////////////////


void RMQ1_1_SMALL::init()
{
	theV = 0;
	int positive = 0,negative = 0;
	for(int i = 1;i < len;++ i)
	{
		if(sorArray[i] > sorArray[i - 1])	//递增
		{
			positive = (positive << 1) + 1;
			negative <<= 1;
		}
		else
		{
			negative = (negative << 1) + 1;
			positive <<= 1;
		}
	}
	theV = positive - negative;
	vRadix = radix(abs(theV));
	thePositive = positive;
	theNegative = negative;
//	show1_1();
}

void RMQ1_1_SMALL:: show1_1()
{
	int bigger = thePositive > theNegative ? thePositive : theNegative;
	int theRadix = radix(bigger);
	int cpm = 1 << (theRadix - 1);
	while (0 != cpm)
	{
		if(0 != (cpm & thePositive))
		{
			cout << 1 << " ";
		}
		else
		{
			cout << -1 << " ";
		}
		cpm >>= 1;
	}
	cout << endl;
}

//查询区间 s(inclusive) -> e(exclusive)最小值
//index 表示 s 到 e 这个区间的最小值的索引
int RMQ1_1_SMALL::queryMin(int s,int e,int *index)
{
	if(e >= len)
	{
		e = len - 1;
	}
	if(s >= len)
	{
		return -1;
	}
	if(s == e)
	{
		SET_RESULT_IF_NOT_NULL(index,s);
		return sorArray[s];
	}
	if(2 == len)
	{
		SET_RESULT_IF_NOT_NULL(index,(sorArray[0] < sorArray[1]) ? 0 : 1);
		return sorArray[0] < sorArray[1] ? sorArray[0] : sorArray[1];
	}
	//查询 [s,e]最小值相当于查询 theV 的第 [s,e-1] 区间的最小值综合 sorArray[s]的结果。
	//因此需要得到区间 [s,e - 1]对应的数字，再使用RMQ_IMPL查询这个数字对应的最小值
	//而 theV 最简表达式可能没有 s 位或 e-1 位，将其扩展为更多位的做法是添加 (1 -1)这样的序列
	//如 5 = (4) + 2 - 1 = (8 - 4) + 2 -1 = (16 - 8 - 4) + 2 -1 = (32 - 16 - 8 - 4) + 2 - 1
	//即： 5 = (1 1 -1) = (1 -1 1 -1) = (1 -1 -1 1 -1) = (1 -1 -1 -1 1 -1) ...
	//规律就是：将最高位的数字 x 换成 2x - x
	//对于负数：-5 = (-4) -2 + 1 = (-4*2 - (-4)) -2 +1 = (-4*2*2 -(-4*2) -(-4)) -2 +1，与正数是一样的规律，都是将最高位的数字 x 换成 2x - x

	//想像将 theV 扩展为len-1位：
	//若theV > 0 则第0位第0位为1，后面都为-1，再续上（len-vRadix 位到最末尾为 theV的 +-1 二进制位）
	//若theV < 0 则第0位第0位为-1，后面都为1，再续上（len-vRadix 位到最末尾为 theV的 +-1 二进制位）
	//theV扩展后的位置范围为：[0,len-2]

	//	0____(s)______(e-1)_______(len-vRadix-1)_________len-2
	if(e - 1 < len - vRadix - 1)
	{
		int curMin;
		if(theV > 0)
		{
			if(0 == s)//(s)______(e-1)_______(len-vRadix-1)_________len-2
			{
				curMin = 2 - e;//[s,e-1]中共有e-s个数字，其中第 0 位上为1，而第1位到第 e-1 位上都是 -1，则和的最小值为 -1 * (e-s-1) + 1 = 2-e
			}
			else //0____(s)______(e-1)_______(len-vRadix-1)_________len-2	
			{
				curMin = s - e;//有 e-s 个 -1
			}
		//	return (curMin) < 0 ? (((NULL != index ? *(index) = (e-1) : 1)) ? curMin + sorArray[s] : curMin + sorArray[s]) : 
		//				(((NULL != index ? *(index) = (s) : 1)) ? sorArray[s] : sorArray[s]);
			return QUERY_MIN_RESULT(curMin,e-1);
		}
		else
		{
			if(0 == s)//(s)______(e-1)_______(len-vRadix-1)_________len-2
			{
				curMin = -1;//[s,e-1]中共有e-s个数字，其中第 0 位上为-1，而第1位到第 e-1 位上都是 1，则最小和为 -1
			}
			else //0____(s)______(e-1)_______(len-vRadix-1)_________len-2	
			{
				curMin = 1;//有 e-s 个 1，最小和为1
			}
			
			return QUERY_MIN_RESULT(curMin,s);
		}
	}
	else//	0____(s)________(len-vRadix-1)___(e-1)______len-2
	{
		if(s <= len - vRadix-1)//	0____(s)________(len-vRadix-1)___(e-1)______len-2
		{
			//分为两段进行[s,len-vRadix-1],[len-vRadix,e-1]；注意第一段并非是到 len-vRadix-2!
			//考虑 v = 9，realRadix = 7,1 -1 -1 -1 (1 -1 -1)，它的更简表示为：1 (1 -1 -1)，更简的位数为：conciseRadix = 4，
			//发现后面的 conciseRadix - 1 = 3 位都是相同的。故序列的最小和分为两部分，前面的(1 -1 -1 -1)，后面的(1 -1 -1)；

			//取theV前面[len-vRadix , e-1] 上的位的最小和，它等于[len-vRadix-1,e-1]最小值与 theV的第 len-vRadix-1 位上是1或-1对比的结果
			//需要被右移掉的位为：[e,len-2]，即右移 len-2-e+1 = len-e-1
			int secondPartV = (thePositive >> (len - e - 1)) - (theNegative >> (len - e - 1));
			int secondPartMin;
			int tmpMinIndex = -1;	//以第 len-vRadix-1 为相对第 0 位的索引号
			int tmpMin = rmqImpl->queryMin(secondPartV,&tmpMinIndex);
			secondPartMin = tmpMin - (theV > 0? 1 : -1);
			tmpMinIndex += len - vRadix - 1;//纠正索引号
			//再求出前面一段，取theV前面[s , len-vRadix-1] 上的位的最小和
			int curMin;

			if(theV > 0)
			{
				if(0 == s)//(s)________(len-vRadix-1)___(e-1)______len-2
				{
					//[0,len-vRadix-1]有 len-vRadix-1个 -1，一个+1，即最小和vRadix - len + 2
					curMin = vRadix - len + 2;
				}
				else//0____(s)________(len-vRadix-1)___(e-1)______len-2
				{
					//[s,len-vRadix-1]有 len-vRadix-1 -s + 1 个 -1
					curMin = vRadix - len + s;
				}
				if(secondPartMin < 0)
				{
				//	SET_RESULT_IF_NOT_NULL(index,tmpMinIndex);
					return QUERY_MIN_RESULT(curMin + secondPartMin,tmpMinIndex);
				}
				else
				{
				//	SET_RESULT_IF_NOT_NULL(index,len-vRadix-1);	// s -> e-1 最小值落在了 firstpart 的最后一位即 len-vRadix-1，转化到实际序列上就是 len-vRadix-1 + 1
					return QUERY_MIN_RESULT(curMin,len-vRadix-1);
				}
			}
			else
			{
				int curMinIndex = -1;
				int theSub;
				if(0 == s)//(s)________(len-vRadix-1)___(e-1)______len-2
				{
					//[0,len-vRadix-1]有 len-vRadix-1个 1，一个-1，最小和为-1
					curMin = -1;
					theSub = len - vRadix - 2;
					curMinIndex = 0;
				}
				else//0____(s)________(len-vRadix-1)___(e-1)______len-2
				{
					//[s,len-vRadix-1]有 len-vRadix-1 -s + 1 个 1，最小和为1
					curMin = 1;
					theSub = len - vRadix - s;
					curMinIndex = 0;
				}
				if(theSub + secondPartMin < curMin)
				{
				//	SET_RESULT_IF_NOT_NULL(index,tmpMinIndex);
					return QUERY_MIN_RESULT(theSub + secondPartMin,tmpMinIndex);
				}
				else
				{
				//	SET_RESULT_IF_NOT_NULL(index,curMinIndex);
					return QUERY_MIN_RESULT(curMin,curMinIndex);
				}
			}
		}
		else//	0________(len-vRadix-1)_____(s)_____(e-1)______len-2
		{
			//取出 theV [s,e-1]位代表的数字
			//这里有一个陷阱，设 theV = 1 1 -1 -1 1 1 1 -1 -1
			//看最后三位，为 1 -1 -1，它对应的值是 4-3 = 1，而1对应的最小值是1，然而，1 -1 -1 的最小和应该是 -1！
			//这就说明，对于值一样的两个序列，不一定最小和也一样！而是：对于值一样的两个最简序列，最小和一定一样！因为值一样的最简序列没有第二种选择，它们必然一样
			
			//[e,len-2]被忽略，即最后面 len-e-1 位都被忽略
			int positive = thePositive ,negative = theNegative;
			int skipBack = (len - e - 1);
			int v = (positive >>= skipBack) - (negative >>= skipBack);
			
			//[len-vRadix-1,s-1]被忽略，即前面 s-1-(len-vRadix-1) + 1 = s+r-len+1 位被忽略
			//操作的方法是：对 positive和negative都只保留后面的 realRadix 位
			int skipFront = s + vRadix - len + 1;
			int realRadix = e - s;//从s 到 e-1位 //也就是vRadix - skipBack - skipFront;
			
			int toYu = (1L << realRadix) - 1;
			int a = toYu & positive;
			int b = toYu & negative;
			v = (a - b);
			if(1 != (v & 1))
			{
				cout << "error" << endl;
				return -1;
			}
			//位数与值不匹配，如 v = 1,realRadix=3；而正常情况下，1只有一位，说明现在1的表示是 1 -1 -1，然而正常的 1 应该表示 1
			//更多的例子：设 v = 9，realRadix = 7,1 -1 -1 -1 (1 -1 -1)，它的更简表示为：1 (1 -1 -1)，更简的位数为：conciseRadix = 4，
			//发现后面的 conciseRadix - 1 = 3 位都是相同的。前一个序列的最小和分为两部分，前面的(1 -1 -1 -1)，后面的(1 -1 -1)；
			//前面的(1 -1 -1 -1)的位数为 y = realRadix-(conciseRadix-1)；(y必定大于等于2)，其中只有最高位为 1，其它都是-1，故它的最小和为-(y-2)
			//为了求后面的(1 -1 -1)的最小和，我们发现它相对于最简序列v=(1 1 -1 -1)少了最高位的一个1，它的最小和设为 minSub，有以下结论：
			//minSub = rmqImpl->queryMin(v) - 1;
			//以上的讨论都是对 v 为正时的讨论，当v为负时，v = -9,readRadix = 7, -1 1 1 1 (-1 1 1)，更简表示为：-1 (-1 1 1)...
			int conciseRadix = radix(abs(v));
			if(realRadix == conciseRadix)
			{
				int minIndex = -1;	//相对于 s 为 0 时的索引号
				int retMin = rmqImpl->queryMin(v,&minIndex);
				minIndex += s;//修正  
			//	SET_RESULT_IF_NOT_NULL(index,minIndex);
				return QUERY_MIN_RESULT(retMin,minIndex);
			}
			//________s___(e-conciseRadix)_____(e-1)_________len-2
			//分为两部分考虑：[s,e-c],[e-c+1,e-1]
			//第二部分是由 v 代表的+-1序列的最小值与 v 的正负比较得到的结果
			int y = realRadix - (conciseRadix - 1);//第一部分是 y 位
			assert(y >= 2);
			if(v > 0)	
			{	
				int firstPartMinSub = 2 - y;
				int secondPartMinSub;
				int tmpIndex = -1;	//相对于 e-conciseRadix 为 0 时的索引号
				int theMin = rmqImpl->queryMin(v,&tmpIndex);
				tmpIndex += (e - conciseRadix);
				secondPartMinSub = theMin - 1;
				if(secondPartMinSub < 0)
				{
				//	SET_RESULT_IF_NOT_NULL(index,tmpIndex);
					return QUERY_MIN_RESULT(firstPartMinSub + secondPartMinSub,tmpIndex);
				}
				else
				{
				//	SET_RESULT_IF_NOT_NULL(index,e-conciseRadix);
					return QUERY_MIN_RESULT(firstPartMinSub,e-conciseRadix);
				}
			}
			else//________s___(e-conciseRadix)_____(e-1)_________len-2
			{
				//第一部分为 y 位，最高位位为 -1，后面都为1
				int firstPartMinSub = -1;
				int theSub = y - 2;

				//第二部分最高位为 -1
				int secondPartMinSub;
				int tmpIndex = -1;	//相对于 e-conciseRadix 为 0 时的索引号
				int theMin = rmqImpl->queryMin(v,&tmpIndex);
				secondPartMinSub = theMin - (-1);
				tmpIndex += (e - conciseRadix);
				if(theSub + secondPartMinSub < firstPartMinSub)
				{
				//	SET_RESULT_IF_NOT_NULL(index,tmpIndex);
					return QUERY_MIN_RESULT(theSub + secondPartMinSub,tmpIndex);
				}
				else
				{
				//	SET_RESULT_IF_NOT_NULL(index,s);
					return QUERY_MIN_RESULT(firstPartMinSub,s);
				}
			}
		}
	}
}

#undef QUERY_MIN_RESULT
//end of RMQ1_1_SMALL
//////////////////////////////////////////////////////////////////////////
#include "SectionMin.h"
#include <ctime>

RMQ1_1::RMQ1_1(int *theArray,int theLen)
{
	sorArray = theArray;
	len = theLen;
	init();
}

RMQ1_1::~RMQ1_1()
{
	if(NULL != theRmqImpl)
	{
		delete theRmqImpl;
		theRmqImpl = NULL;
	}
	if(NULL != sectionMins)
	{
		delete[] sectionMins;
		sectionMins = NULL;
	}
	if(NULL != theRmqSs)
	{
		delete[] theRmqSs;
		theRmqSs = NULL;
	}
	if(NULL != theSectMin)
	{
		delete theSectMin;
		theSectMin = NULL;
	}
}

void RMQ1_1:: init()
{
	theRmqImpl = new Model1_1_MinValue(SECTION_LEN - 1);
	sectionCounts = len / SECTION_LEN + ((0 != (len % SECTION_LEN)) ? 1 : 0);

	sectionMins = new int[sectionCounts];
	theRmqSs = new RMQ1_1_SMALL[sectionCounts];
	int k = 0;
	for (;k < sectionCounts-1;++ k)
	{
		theRmqSs[k].reConstruct(sorArray+k*SECTION_LEN,SECTION_LEN,theRmqImpl);
		sectionMins[k] = theRmqSs[k].queryMin(0,SECTION_LEN-1);
	}
	int lastStartPos = k * SECTION_LEN;
	int lastEndPos = len - 1;
	theRmqSs[k].reConstruct(sorArray+lastStartPos,lastEndPos - lastStartPos + 1,theRmqImpl);
	sectionMins[k] = theRmqSs[k].queryMin(0,lastEndPos - lastStartPos);

	
#ifdef VERIFY_SECTION_MINS
	int sectionIndex = 0;
	for(int i = 0;i < len;i += SECTION_LEN)
	{
		int toEnd = i + SECTION_LEN;
		if(toEnd > len)	toEnd = len;
		int theMin = INT_MAX;
		for(int j = i;j < toEnd;++ j)
		{
			if(theMin > sorArray[j])
			{
				theMin = sorArray[j];
			}
		}
		if(sectionMins[sectionIndex] != theMin)
		{
			cout << "error" <<endl;
		}
		sectionMins[sectionIndex ++] = theMin;
	}
	assert(sectionIndex == sectionCounts);
#endif
	
//	showSectMins();
	theSectMin = new SectionMin(sectionMins,sectionCounts,sectionCounts - 1);
}

void RMQ1_1:: showSectMins()
{
	for(int i = 0;i < sectionCounts;++ i)
	{
		cout << sectionMins[i] << " ";
	}
	cout << endl;
}

int RMQ1_1::queryMin(int s,int e,int *index)
{
	assert(s <= e);
	int sBlock = s / SECTION_LEN;
	int eBlock = e / SECTION_LEN;

	if(sBlock == eBlock)
	{
#ifdef ACCELEARTION
		int minValue = sorArray[s];
		int theMinIndex = s;
		for(int i = s+1;i <= e;++ i)
		{
			if(minValue > sorArray[i])
			{
				minValue = sorArray[i];
				theMinIndex = i;
			}
		}
		SET_RESULT_IF_NOT_NULL(index,theMinIndex);
		return minValue;
#else
		int startPos = sBlock*SECTION_LEN;
		RMQ1_1_SMALL rmqTmp(sorArray + startPos,SECTION_LEN,theRmqImpl);
		int theMinValue = rmqTmp.queryMin(s - startPos,e - startPos,index);
		SET_RESULT_IF_NOT_NULL(index,*index + sBlock*SECTION_LEN);
		return theMinValue;
#endif
	}

	int min1,min2,min3;
	min1 = min2 = min3= INT_MAX;

	int minIndex2 = -1;
	//求sBlock中 s 到末尾的最小值
	int sStartPos = sBlock * SECTION_LEN;
#ifdef ACCELEARTION
	minIndex2 = s;
	min2 = sorArray[s];
	int toEnd = (sBlock+1) * SECTION_LEN;
	for(int i = s + 1;i < toEnd;++ i)
	{
		if(min2 > sorArray[i])
		{
			min2 = sorArray[i];
			minIndex2 = i;
		}
	}
#else
	min2 = theRmqSs[sBlock].queryMin(s - sStartPos,(sBlock+1)*SECTION_LEN - 1 - sStartPos,&minIndex2);
	minIndex2 += sStartPos;
#endif


	

	int minIndex1 = -1;
	int minBlockIndex1 = -1;
	//求 sBlock + 1 到 eBlock -1最小值
	
	
	if(sBlock +1 <= eBlock -1)
	{
		min1 = theSectMin->queryEx(sBlock + 1,eBlock - 1,&minBlockIndex1);
		theRmqSs[minBlockIndex1].queryMin(0,SECTION_LEN - 1,&minIndex1);	//查第 minBlockIndex 块中最小值的索引
		minIndex1 += minBlockIndex1 * SECTION_LEN;
	}
	

#ifdef CMP_GENERATE_RMQ1_1_SMALL_EVERYTIME
	RMQ1_1_SMALL rmqTmp1(sorArray + sStartPos,SECTION_LEN,theRmqImpl);
	int minDupIndex2 = -1;
	int dupMin2 = rmqTmp1.queryMin(s - sStartPos,(sBlock+1)*SECTION_LEN - 1 - sStartPos,&minDupIndex2);
	minDupIndex2 += sStartPos;
	assert(min2 == dupMin2 && minDupIndex2 == minIndex2);
#endif

	//求 eBlock中首到 e 的最小值
	int eStartPos = eBlock * SECTION_LEN;
	int minIndex3 = -1;
#ifdef ACCELEARTION
	minIndex3 = eStartPos;
	min3 = sorArray[eStartPos];
	toEnd = e+1;
	for(int i = eStartPos+1;i < toEnd;++ i)
	{
		if(min3 > sorArray[i])
		{
			min3 = sorArray[i];
			minIndex3 = i;
		}
	}
#else
	min3 = theRmqSs[eBlock].queryMin(eBlock * SECTION_LEN - eStartPos,e - eStartPos,&minIndex3);
	minIndex3 += eStartPos;
#endif

#ifdef CMP_GENERATE_RMQ1_1_SMALL_EVERYTIME
	RMQ1_1_SMALL rmqTmp2(sorArray + eStartPos,SECTION_LEN,theRmqImpl);
	int minDupIndex3 = -1;
	int dupMin3 = rmqTmp2.queryMin(eBlock * SECTION_LEN - eStartPos,e - eStartPos,&minDupIndex3);
	minDupIndex3 += eStartPos;
	assert(min3 == dupMin3 && minDupIndex3 == minIndex3);
#endif
	
	//索引顺序为 m2 m1 m3
	SET_RESULT_IF_NOT_NULL(index,min2 <= min1 ? (min2 <= min3 ? minIndex2 : minIndex3) : (min1 <= min3 ? minIndex1 : minIndex3));
	return min2 <= min1 ? (min2 <= min3 ? min2 : min3) : (min1 <= min3 ? min1 : min3);

}

#undef SECTION_LEN
#undef CMP_GENERATE_RMQ1_1_SMALL_EVERYTIME
#undef VERIFY_SECTION_MINS