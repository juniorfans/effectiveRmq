
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
	maxOdd = (1 << L) - 1; //�������
	int minValueSize = maxOdd + 1;//(maxOdd - (-maxOdd)) / 2 + 1 ������һ��
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
	//5 = 4+2 - 1 ���Ϊ�������ϣ�{4,2} �� {1} ǰһ�����ϱ�ʾ������λΪ1����һ��ֵ��ʾΪ-1.
	//positive ��ʾǰ��һ�����ϵ�����ֵ�Ļ�negative ��ʾ��һ�����ϵĻ�
	int positive = 1,negative = 0;
	addMin(1,0);
	addMin(0,1);	//-1
	int lastNegative2N = 0;
	for(int i = 3;i <= maxOdd;i += 2) 
	{
		//��ǰ��������һ��������2������������һ�������Ķ����Ƶõ���ǰ�����Ķ�����
		//ʹ�õķ����ǣ��� negative �е� 1 ת�Ƶ� positive�У���ʱ�൱��ԭ���Ķ�����ֵ�� 2��
		//�� negative �в����� 1����negative �е� 2 ת�Ƶ� positive���� positive �е� 1 ת�Ƶ� negative
		//�� negative �в����� 2����negative �е� 4 ת�Ƶ� positive���� positive �е� 2,1 ת�Ƶ� negative
		//...
		//���ݵ�ԭ���� ��2^n - (2^n-1 + 2 ^n-2 + ... + 2^1 + 2^0) = 1��ֻ��Ҫ�� negative �е�2^nת�Ƶ� positive���ٽ�
		//positive �е�2^n-1 + 2 ^n-2 + ... + 2^1 + 2^0) ת�Ƶ� negative �����ܱ�֤ǰ������������ֵ��� 2.

		//������ 3 ��ʼ�ĵ���2���������У��� 2^n �� negative �У��� 1,2,4,... 2^(n-1) һ���� positive ���С���֮��Ȼ��

		//���ǻ�����˼·����һ�����������Ҫ������ negative Ϊ 0 ʱ��
		//���ʱ positive ���ϵĺͣ����ڱ���Ϊ positive ��ֵ��Ϊ x (x�ض�Ϊ��������Ϊ1��2��4��8...���� positive ��)������������һ������λ�����ַ��� positive����ֵΪ x + 1
		//��Ϊ�������е�λ��Ϊ1������ 2^n - (2^n-1 + 2 ^n-2 + ... + 2^1 + 2^0) = 1 ���ʸ���λ�����ֱ�ʾ x + 1
		//��� positive ת�� b �� negative��positive �л�ʣ�� a����ʱ��Ӧ�Ķ���������Ӧ���� x + 2������һ��������2�������·��̣�
		//a + b = x+1 + x��a - b = x + 2���ⷽ�̣�b = (x-1)/2��a = (3x+3)/2
		//����positive = (3x+3)/2��negative = (x-1)/2��
		//ʵ���ϣ��� positive = 2^n + 2^(n-1) + ... + 2 + 1; negative = 0; ��ʱֻ��Ҫ�� positve = 2^(n+1) + 2^n��negative = 2^(n-1) + ... + 2 + 1 ����

		//�ȴ������������ negative Ϊ��
		if(0 == negative)
		{
			#ifdef CHECK_RMQ1_1_INIT
			assert(0 == ((positive + 1) & positive));//����λȫΪ 1
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
			//�ҵ� negative����͵�ֵΪ 1 ��λ �� negative = {4,16} ���ҵ� 4����3λ��
			int curAdd = 1;
			int n = 0;
			while(curAdd != (negative & curAdd))
			{
				curAdd <<= 1;
				++ n;
			}
			
			//�� 2^n �� negative ת�Ƶ� positive
			#ifdef CHECK_RMQ1_1_INIT
			assert(0 == (positive & curAdd));//2^n ����negative ֮�У��ͱض������� positive �����С���Ϊһ�����ֲ������ڵ� n λ��ͬʱΪ 1 �� -1
			#endif
			positive |= curAdd;
			negative -= curAdd;

			//�� 2^n-1 + 2 ^n-2 + ... + 2^1 + 2^0 �� positive ת�Ƶ� negative
			#ifdef CHECK_RMQ1_1_INIT
			assert(curAdd - 1 == ((curAdd - 1) & positive));//��֤ 2^n-1 �� 2 ^n-2 �� ... ��2^1 �� 2^0���� positive ֮�У�
			assert(0 == (negative & (curAdd - 1)));//��֤ 2^n-1 �� 2 ^n-2 �� ... ��2^1 �� 2^0������ negative ֮�У�
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

//���һ��С�� num �� 2^n
int Model1_1_MinValue:: lastMin2n(int num)
{
	long long ret = 1;//�������Ҫ������� 32 λ������������� INT_MAX ��˵��ret ��������ֱ��С��0���ٵ�С��0.
	while (ret <= num)
	{
		ret <<= 1;
	}
	return ret >> 1;
}

/************************************************************************/
/* 
	�� v = positive - negative;	lastOodV = (positive-negative)/2;
	lastOodV �൱�ڰ� positive �� negative �ֱ𶼳��� 2��
	�� 1 �� positve �У��� positive / 2 �൱�ڰ� positive �е� 1 �Ƴ�����
		������
		�� positive = {16,4,1},negative={8,2}����ʾ��1 -1 1 -1 1��
		�ֱ����2���ɣ�{8,2} �� {4,1}����ʾ 1 -1 1 -1
		�Ը�ֵ�Ӱ�������������ĩβһ�� 1,����ʾ������֮�ͱ�ԭ��С 1
	�� 1 �� negative �У����ʾ����֮�ͱ�ԭ���� 1

	���� v ��ʾ������֮�ͱ� v/2 ��ʾ������֮�ͣ��� 1(�� 1 �� positve ��)��С 1 (�� 1 �� negative ��)

	��һ���棬���� odd �� lastOdd �ֱ���� 2 �����е�˳����Ӱ�죬ֻ���������һ�� 1 �� -1�������������б�ʾ�����е���ֵ
	�����±仯��
		[1].���ֵ : {1 �� positive �У��� odd ����ֵ�� lastOdd ��1��1 �� negative �У����ֵ������������1������ֵ����
		[2].��Сֵ : {1 �� positive �У���Сֵ���ῼ�������1���� odd ����ֵ���䣻1 �� negative �У�odd ��ֵ�� lastOdd С1}
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
	int halfV = (positive >> 1) - (negative >> 1);//ע�ⲻ���� (positive-negative)>>1
	
	int lowLevelIndex = MIN_VALUES_INDEX(halfV);
	//����� 1 �� positive ������ negative ����
	//Ŀǰ�� +1 -1 ���������ԭ����������ĩβ����һ�� +1 �� -1
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
	int halfV = (positive >> 1) - (negative >> 1);//ע�ⲻ���� (positive-negative)>>1

	int lowLevelIndex = MIN_VALUES_INDEX(halfV);
	//����� 1 �� positive ������ negative ����
	//Ŀǰ�� +1 -1 ���������ԭ����������ĩβ����һ�� +1 �� -1
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
	//�������
	if(0 == negative)//positive �����е�λ��Ϊ 1����СֵΪ1
	{
		theMinValues_directyly[index] = 1;
		theMinIndexs_directyly[index] = 0;
		return 1;
	}
	//negative ������λ��Ϊ 1 ʱ��Ҫѭ�������ж��ٸ� 1���ܾ�����Сֵ��ת������Ĵ�����
	
	//��������
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
		if(sorArray[i] > sorArray[i - 1])	//����
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

//��ѯ���� s(inclusive) -> e(exclusive)��Сֵ
//index ��ʾ s �� e ����������Сֵ������
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
	//��ѯ [s,e]��Сֵ�൱�ڲ�ѯ theV �ĵ� [s,e-1] �������Сֵ�ۺ� sorArray[s]�Ľ����
	//�����Ҫ�õ����� [s,e - 1]��Ӧ�����֣���ʹ��RMQ_IMPL��ѯ������ֶ�Ӧ����Сֵ
	//�� theV �����ʽ����û�� s λ�� e-1 λ��������չΪ����λ����������� (1 -1)����������
	//�� 5 = (4) + 2 - 1 = (8 - 4) + 2 -1 = (16 - 8 - 4) + 2 -1 = (32 - 16 - 8 - 4) + 2 - 1
	//���� 5 = (1 1 -1) = (1 -1 1 -1) = (1 -1 -1 1 -1) = (1 -1 -1 -1 1 -1) ...
	//���ɾ��ǣ������λ������ x ���� 2x - x
	//���ڸ�����-5 = (-4) -2 + 1 = (-4*2 - (-4)) -2 +1 = (-4*2*2 -(-4*2) -(-4)) -2 +1����������һ���Ĺ��ɣ����ǽ����λ������ x ���� 2x - x

	//���� theV ��չΪlen-1λ��
	//��theV > 0 ���0λ��0λΪ1�����涼Ϊ-1�������ϣ�len-vRadix λ����ĩβΪ theV�� +-1 ������λ��
	//��theV < 0 ���0λ��0λΪ-1�����涼Ϊ1�������ϣ�len-vRadix λ����ĩβΪ theV�� +-1 ������λ��
	//theV��չ���λ�÷�ΧΪ��[0,len-2]

	//	0____(s)______(e-1)_______(len-vRadix-1)_________len-2
	if(e - 1 < len - vRadix - 1)
	{
		int curMin;
		if(theV > 0)
		{
			if(0 == s)//(s)______(e-1)_______(len-vRadix-1)_________len-2
			{
				curMin = 2 - e;//[s,e-1]�й���e-s�����֣����е� 0 λ��Ϊ1������1λ���� e-1 λ�϶��� -1����͵���СֵΪ -1 * (e-s-1) + 1 = 2-e
			}
			else //0____(s)______(e-1)_______(len-vRadix-1)_________len-2	
			{
				curMin = s - e;//�� e-s �� -1
			}
		//	return (curMin) < 0 ? (((NULL != index ? *(index) = (e-1) : 1)) ? curMin + sorArray[s] : curMin + sorArray[s]) : 
		//				(((NULL != index ? *(index) = (s) : 1)) ? sorArray[s] : sorArray[s]);
			return QUERY_MIN_RESULT(curMin,e-1);
		}
		else
		{
			if(0 == s)//(s)______(e-1)_______(len-vRadix-1)_________len-2
			{
				curMin = -1;//[s,e-1]�й���e-s�����֣����е� 0 λ��Ϊ-1������1λ���� e-1 λ�϶��� 1������С��Ϊ -1
			}
			else //0____(s)______(e-1)_______(len-vRadix-1)_________len-2	
			{
				curMin = 1;//�� e-s �� 1����С��Ϊ1
			}
			
			return QUERY_MIN_RESULT(curMin,s);
		}
	}
	else//	0____(s)________(len-vRadix-1)___(e-1)______len-2
	{
		if(s <= len - vRadix-1)//	0____(s)________(len-vRadix-1)___(e-1)______len-2
		{
			//��Ϊ���ν���[s,len-vRadix-1],[len-vRadix,e-1]��ע���һ�β����ǵ� len-vRadix-2!
			//���� v = 9��realRadix = 7,1 -1 -1 -1 (1 -1 -1)�����ĸ����ʾΪ��1 (1 -1 -1)�������λ��Ϊ��conciseRadix = 4��
			//���ֺ���� conciseRadix - 1 = 3 λ������ͬ�ġ������е���С�ͷ�Ϊ�����֣�ǰ���(1 -1 -1 -1)�������(1 -1 -1)��

			//ȡtheVǰ��[len-vRadix , e-1] �ϵ�λ����С�ͣ�������[len-vRadix-1,e-1]��Сֵ�� theV�ĵ� len-vRadix-1 λ����1��-1�ԱȵĽ��
			//��Ҫ�����Ƶ���λΪ��[e,len-2]�������� len-2-e+1 = len-e-1
			int secondPartV = (thePositive >> (len - e - 1)) - (theNegative >> (len - e - 1));
			int secondPartMin;
			int tmpMinIndex = -1;	//�Ե� len-vRadix-1 Ϊ��Ե� 0 λ��������
			int tmpMin = rmqImpl->queryMin(secondPartV,&tmpMinIndex);
			secondPartMin = tmpMin - (theV > 0? 1 : -1);
			tmpMinIndex += len - vRadix - 1;//����������
			//�����ǰ��һ�Σ�ȡtheVǰ��[s , len-vRadix-1] �ϵ�λ����С��
			int curMin;

			if(theV > 0)
			{
				if(0 == s)//(s)________(len-vRadix-1)___(e-1)______len-2
				{
					//[0,len-vRadix-1]�� len-vRadix-1�� -1��һ��+1������С��vRadix - len + 2
					curMin = vRadix - len + 2;
				}
				else//0____(s)________(len-vRadix-1)___(e-1)______len-2
				{
					//[s,len-vRadix-1]�� len-vRadix-1 -s + 1 �� -1
					curMin = vRadix - len + s;
				}
				if(secondPartMin < 0)
				{
				//	SET_RESULT_IF_NOT_NULL(index,tmpMinIndex);
					return QUERY_MIN_RESULT(curMin + secondPartMin,tmpMinIndex);
				}
				else
				{
				//	SET_RESULT_IF_NOT_NULL(index,len-vRadix-1);	// s -> e-1 ��Сֵ������ firstpart �����һλ�� len-vRadix-1��ת����ʵ�������Ͼ��� len-vRadix-1 + 1
					return QUERY_MIN_RESULT(curMin,len-vRadix-1);
				}
			}
			else
			{
				int curMinIndex = -1;
				int theSub;
				if(0 == s)//(s)________(len-vRadix-1)___(e-1)______len-2
				{
					//[0,len-vRadix-1]�� len-vRadix-1�� 1��һ��-1����С��Ϊ-1
					curMin = -1;
					theSub = len - vRadix - 2;
					curMinIndex = 0;
				}
				else//0____(s)________(len-vRadix-1)___(e-1)______len-2
				{
					//[s,len-vRadix-1]�� len-vRadix-1 -s + 1 �� 1����С��Ϊ1
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
			//ȡ�� theV [s,e-1]λ���������
			//������һ�����壬�� theV = 1 1 -1 -1 1 1 1 -1 -1
			//�������λ��Ϊ 1 -1 -1������Ӧ��ֵ�� 4-3 = 1����1��Ӧ����Сֵ��1��Ȼ����1 -1 -1 ����С��Ӧ���� -1��
			//���˵��������ֵһ�����������У���һ����С��Ҳһ�������ǣ�����ֵһ��������������У���С��һ��һ������Ϊֵһ�����������û�еڶ���ѡ�����Ǳ�Ȼһ��
			
			//[e,len-2]�����ԣ�������� len-e-1 λ��������
			int positive = thePositive ,negative = theNegative;
			int skipBack = (len - e - 1);
			int v = (positive >>= skipBack) - (negative >>= skipBack);
			
			//[len-vRadix-1,s-1]�����ԣ���ǰ�� s-1-(len-vRadix-1) + 1 = s+r-len+1 λ������
			//�����ķ����ǣ��� positive��negative��ֻ��������� realRadix λ
			int skipFront = s + vRadix - len + 1;
			int realRadix = e - s;//��s �� e-1λ //Ҳ����vRadix - skipBack - skipFront;
			
			int toYu = (1L << realRadix) - 1;
			int a = toYu & positive;
			int b = toYu & negative;
			v = (a - b);
			if(1 != (v & 1))
			{
				cout << "error" << endl;
				return -1;
			}
			//λ����ֵ��ƥ�䣬�� v = 1,realRadix=3������������£�1ֻ��һλ��˵������1�ı�ʾ�� 1 -1 -1��Ȼ�������� 1 Ӧ�ñ�ʾ 1
			//��������ӣ��� v = 9��realRadix = 7,1 -1 -1 -1 (1 -1 -1)�����ĸ����ʾΪ��1 (1 -1 -1)�������λ��Ϊ��conciseRadix = 4��
			//���ֺ���� conciseRadix - 1 = 3 λ������ͬ�ġ�ǰһ�����е���С�ͷ�Ϊ�����֣�ǰ���(1 -1 -1 -1)�������(1 -1 -1)��
			//ǰ���(1 -1 -1 -1)��λ��Ϊ y = realRadix-(conciseRadix-1)��(y�ض����ڵ���2)������ֻ�����λΪ 1����������-1����������С��Ϊ-(y-2)
			//Ϊ��������(1 -1 -1)����С�ͣ����Ƿ�����������������v=(1 1 -1 -1)�������λ��һ��1��������С����Ϊ minSub�������½��ۣ�
			//minSub = rmqImpl->queryMin(v) - 1;
			//���ϵ����۶��Ƕ� v Ϊ��ʱ�����ۣ���vΪ��ʱ��v = -9,readRadix = 7, -1 1 1 1 (-1 1 1)�������ʾΪ��-1 (-1 1 1)...
			int conciseRadix = radix(abs(v));
			if(realRadix == conciseRadix)
			{
				int minIndex = -1;	//����� s Ϊ 0 ʱ��������
				int retMin = rmqImpl->queryMin(v,&minIndex);
				minIndex += s;//����  
			//	SET_RESULT_IF_NOT_NULL(index,minIndex);
				return QUERY_MIN_RESULT(retMin,minIndex);
			}
			//________s___(e-conciseRadix)_____(e-1)_________len-2
			//��Ϊ�����ֿ��ǣ�[s,e-c],[e-c+1,e-1]
			//�ڶ��������� v �����+-1���е���Сֵ�� v �������Ƚϵõ��Ľ��
			int y = realRadix - (conciseRadix - 1);//��һ������ y λ
			assert(y >= 2);
			if(v > 0)	
			{	
				int firstPartMinSub = 2 - y;
				int secondPartMinSub;
				int tmpIndex = -1;	//����� e-conciseRadix Ϊ 0 ʱ��������
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
				//��һ����Ϊ y λ�����λλΪ -1�����涼Ϊ1
				int firstPartMinSub = -1;
				int theSub = y - 2;

				//�ڶ��������λΪ -1
				int secondPartMinSub;
				int tmpIndex = -1;	//����� e-conciseRadix Ϊ 0 ʱ��������
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
	//��sBlock�� s ��ĩβ����Сֵ
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
	//�� sBlock + 1 �� eBlock -1��Сֵ
	
	
	if(sBlock +1 <= eBlock -1)
	{
		min1 = theSectMin->queryEx(sBlock + 1,eBlock - 1,&minBlockIndex1);
		theRmqSs[minBlockIndex1].queryMin(0,SECTION_LEN - 1,&minIndex1);	//��� minBlockIndex ������Сֵ������
		minIndex1 += minBlockIndex1 * SECTION_LEN;
	}
	

#ifdef CMP_GENERATE_RMQ1_1_SMALL_EVERYTIME
	RMQ1_1_SMALL rmqTmp1(sorArray + sStartPos,SECTION_LEN,theRmqImpl);
	int minDupIndex2 = -1;
	int dupMin2 = rmqTmp1.queryMin(s - sStartPos,(sBlock+1)*SECTION_LEN - 1 - sStartPos,&minDupIndex2);
	minDupIndex2 += sStartPos;
	assert(min2 == dupMin2 && minDupIndex2 == minIndex2);
#endif

	//�� eBlock���׵� e ����Сֵ
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
	
	//����˳��Ϊ m2 m1 m3
	SET_RESULT_IF_NOT_NULL(index,min2 <= min1 ? (min2 <= min3 ? minIndex2 : minIndex3) : (min1 <= min3 ? minIndex1 : minIndex3));
	return min2 <= min1 ? (min2 <= min3 ? min2 : min3) : (min1 <= min3 ? min1 : min3);

}

#undef SECTION_LEN
#undef CMP_GENERATE_RMQ1_1_SMALL_EVERYTIME
#undef VERIFY_SECTION_MINS