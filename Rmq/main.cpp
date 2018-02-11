#include <iostream>
using namespace std;
#include <cassert>
#include <memory>
#include <ctime>
#include <limits>
#include "RMQ+1-1.h"
#include "Utils.h"

static const int MAX_SIZE = 20;
static Model1_1_MinValue *theRMQ1_1Impl;

void initRMQ1_1Impl()
{
//	int finish,start;
//	start = clock();
	theRMQ1_1Impl = new Model1_1_MinValue(MAX_SIZE);
//	finish = clock();
//	cout << "init rmq1_1Impl cost " << (finish - start) << " millseconds"<<endl;
}

void testRMQImpl()
{
	Model1_1_MinValue rmq(16);
#ifdef VERIFY_MODEL1_1_CREATE
	rmq.verify();
#endif
	int num;
	while (true)
	{
		scanf("%d",&num);
		printf(" : %d\r\n",rmq.queryMin(num));
	}
}
#include <stdlib.h>
extern void random1_1Ints(int *sorArray,int len)
{
	if(0 == len) return;
	srand(time(NULL));
	sorArray[0] = rand();
	for(int i = 1;i < len;++i)
	{
		if(1 == (rand() % 3))
		{
			sorArray[i] = sorArray[i-1] - 1;
		}
		else
		{
			sorArray[i] = sorArray[i-1] + 1;
		}
	}
}

void randomInts(int *sorArray,int len)
{
	if(0 == len) return;
	srand(time(NULL));
	for(int i = 0;i < len;++i)
	{
		sorArray[i] = rand();
	}
}

int minValue(int *sorArray,int start,int end,int *index = NULL)
{
	int theMin = sorArray[start];
	SET_RESULT_IF_NOT_NULL(index,start);
	for(int i = start + 1;i <= end;++ i)
	{
		if(theMin > sorArray[i])
		{
			theMin = sorArray[i];
			SET_RESULT_IF_NOT_NULL(index,i);
		}
	}
	return theMin;
}


void testCorrect()
{
	static const int testCase = 20;
	int sorArray[testCase] = {20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};// = {1,2,3,2,1,2,3,4,3,2};
//	fileIntsEx("d:\\randomInts.txt",sorArray,100,NULL);
//	random1_1Ints(sorArray,100);
	
	RMQ1_1 rmq(sorArray,testCase);
//	RMQ1_1_SMALL rmq(sorArray,testCase,theRMQ1_1Impl);
	//	int s,e;
	cout << "input start, end to query" <<endl;
	//cin >> s >> e;
	for(int s = 0;s < MAX_SIZE;++ s)
	{
		for(int e = s;e < MAX_SIZE;++ e)
		{
			int theQueryMin =  rmq.queryMin(s,e);
			int theCalMin = minValue(sorArray,s,e);
			if(theQueryMin != theCalMin)
			{
				cout << "error. " << "query(" << s<<","<<e<<") : " << theQueryMin << ",cal : " << theCalMin << endl;
				intsToFile(sorArray,testCase,"d:\\randomInts.txt");
				exit(-1);
			}
			else
			{
			//	cout << "success."<<endl;
			}
		}
	}
}





void testFileCorrect()
{
	int sorArray[100] ;//= {8,7,8,7,8,7,8,7,6,5,6,7,8,9,10,9,8,7,6,7};// = {1,2,3,2,1,2,3,4,3,2};
	fileIntsEx("d:\\randomInts.txt",sorArray,100,NULL);
//	random1_1Ints(sorArray,100);

	RMQ1_1_SMALL rmq(sorArray,MAX_SIZE,theRMQ1_1Impl);
	int s,e;
	cout << "input start, end to query" <<endl;
	cin >> s >> e;
//	for(int s = 0;s < MAX_SIZE;++ s)
	{
//		for(int e = s;e < MAX_SIZE;++ e)
		{
			int theQueryMin =  rmq.queryMin(s,e);
			int theCalMin = minValue(sorArray,s,e);
			if(theQueryMin != theCalMin)
			{
				cout << "error. " << "query(" << s<<","<<e<<") : " << theQueryMin << ",cal : " << theCalMin << endl;
			}
			else
			{
				//	cout << "success."<<endl;
			}
		}
	}
}



void testMM()
{
	int sorArray[25] = {8,7,8,7,8,7,8,7,6,5,6,7,8,9,10,9,8,7,6,7,8,9,10,11,10};
	Model1_1_MinValue mm(20);
	int s,e;
	int index ;
	while (true)
	{
		cin >> s;
		int minNum = mm.queryMin(s,&index);
		cout << index << "   |   " << minNum << endl;
	}
}



#define VERIFY_QUERY
void testRMQ1_1Correct()
{
	clock_t startT,endT;
	startT = clock();
	int TESTTIMES = 100;
	int CASESIZE = 10000;//(1 << 20) * 10;
	int *sorArray = new int [CASESIZE];
//	fileIntsEx("D:/randomInts.txt",sorArray,CASESIZE,NULL);
	random1_1Ints(sorArray,CASESIZE);
	intsToFile(sorArray,CASESIZE,"D:/randomInts.txt");
	RMQ1_1 rmq(sorArray,CASESIZE);
	srand(time(NULL));
	int errorCounts = 0;
	int s,e;
	int m1,m2;
	int minIndex1,minIndex2;
	while (0 != TESTTIMES --)
	{
		s = rand() % CASESIZE;
		e = rand() % CASESIZE;
		while(s > e)
		{
			e = rand() % CASESIZE;
		}
		m1 = rmq.queryMin(s,e,&minIndex1);
#ifdef VERIFY_QUERY
		m2 = minValue(sorArray,s,e,&minIndex2);
		if(m1 != m2 || minIndex1 != minIndex2)
		{
			cout << "error" << endl;

			++ errorCounts;
		}
#else
		rmq.queryMin(s,e);
#endif

	}
	endT = clock();
	cout << "test " << TESTTIMES << " times,find " << errorCounts << " errors,cost " << (endT - startT) << " millseconds." << endl;
	system("PAUSE");
}

#include "SectionMin.h"
void testSectionMinPerformance()
{
	clock_t startT,endT;
	startT = clock();
	int TESTTIMES = 1000000;
	int CASESIZE = (1 << 20) * 10;
	int *sorArray = new int [CASESIZE];
	random1_1Ints(sorArray,CASESIZE);
	SectionMin *theSm = new SectionMin(sorArray,CASESIZE,CASESIZE - 1);
	srand(time(NULL));
	int errorCounts = 0;
	int s,e;
	while (0 != TESTTIMES --)
	{
		s = rand() % CASESIZE;
		e = rand() % CASESIZE;
		while(s > e)
		{
			e = rand() % CASESIZE;
		}
		theSm->query(s,e);
	}
	endT = clock();
	cout << "test " << TESTTIMES << " times,cost " << (endT - startT) << " millseconds." << endl;
	system("PAUSE");
}



void testRMQSmallFileCorrect()
{
	const static int testCase = 20;
	initRMQ1_1Impl();
	while (true)
	{
		int sorArray[testCase] ;//= {8,7,8,7,8,7,8,7,6,5,6,7,8,9,10,9,8,7,6,7};// = {1,2,3,2,1,2,3,4,3,2};
		random1_1Ints(sorArray,testCase);
		intsToFile(sorArray,testCase,"D:/randomInts.txt");
	//	fileIntsEx("D:\\randomInts.txt",sorArray,testCase,NULL);
		//	random1_1Ints(sorArray,100);
		
		RMQ1_1_SMALL rmq(sorArray,testCase,theRMQ1_1Impl);
		int s,e;
		//	cout << "input start, end to query" <<endl;
		//	cin >> s >> e;
		int errorCounts = 0;
		for(int s = 0;s < testCase;++ s)
		{
			for(int e = s;e < testCase;++ e)
			{
				if(s == 10 && e == 18)
				{
					int a  = 0;
					++ a;
				}
				int index1,index2;
				int theQueryMin =  rmq.queryMin(s,e,&index1);
				int theCalMin = minValue(sorArray,s,e,&index2);
				if(theQueryMin != theCalMin || index1 != index2)
				{
					if(1 != abs(index1 - index2))
					++ errorCounts;
					//cout << "error. " << "query(" << s<<","<<e<<") : " << theQueryMin << ",cal : " << theCalMin << endl;
				}
				else
				{
					//	cout << "success."<<endl;
				}
			}
		}
	if(0 != errorCounts)	cout << errorCounts << " errors." << endl;
	//	system("pause");
	}
}


void testSectionCorrect()
{
	while (true)
	{
		clock_t startT,endT;
		startT = clock();
		int TESTTIMES = 1000000;
		int CASESIZE = 10000000;
		int *sorArray = new int [CASESIZE];
		random1_1Ints(sorArray,CASESIZE);
		SectionMin *theSm = new SectionMin(sorArray,CASESIZE,CASESIZE - 1);
		srand(time(NULL));
		int errorCounts = 0;
		int s,e;
		int minIndex1 = -1;
		int minIndex2 = -1;
		while (0 != TESTTIMES --)
		{
			s = rand() % CASESIZE;
			e = rand() % CASESIZE;
			while(s > e)
			{
				e = rand() % CASESIZE;
			}
			int m1 = theSm->query(s,e,&minIndex1);
		/*	int m2 = minValue(sorArray,s,e,&minIndex2);
			if(m1 != m2 || minIndex1 != minIndex2)
			{
				system("PAUSE");
				cout << "error" << endl;
			}
			*/
		}
		endT = clock();
		cout << "test " << TESTTIMES << " times,error : " << errorCounts << "cost " << (endT - startT) << " millseconds." << endl;
		delete[] sorArray;
		delete theSm;
	}
	
	
	system("PAUSE");
}

#include "RMQ.h"
void testRMQ()
{
	static const int testCase = 10000000;
	//int sorArray[testCase] = {\
		2,5,7,1,8,\
		3,0,23,5,21,\
		9,12,5,1,2,\
		0,1,34,21,12,\
		8,4,2,7,1};
	while (true)
	{
		int *sorArray = new int[testCase];
		randomInts(sorArray,testCase);
		int initst = clock();
		RMQ rmq(sorArray,testCase);
		int initet = clock();
		cout << "init cost " << (initet - initst) << " millsecs " <<endl;
		int s,e;
		int m1,m2;
		int index1,index2;
		int errorCounts = 0;
		srand(time(NULL));
		int ts = clock();
		int testTimes = 10000000;
		for(int i = 0;i < testTimes;++ i)
		{
			s = rand() % testCase;
			e = rand() % testCase;
			while(s > e)
			{
				e = rand() % testCase;
			}
			m1 = rmq.queryMin(s,e,&index1);
		/*	
			m2 = minValue(sorArray,s,e,&index2);
			if(m1 != m2 || index1 != index2)
			{
				++ errorCounts;
			}
			*/
		}
		int te = clock();
		cout << "query " << testTimes << " times,cost "  << (te -ts) << " millsecs." << endl;
		if(0 != errorCounts)cout << errorCounts << " errors." << endl;
		delete[] sorArray;
	}
	
	system("PAUSE");
}
#include "NormalRMQ.h"
void testNormalRMQ()
{
	static const int testCase = 100000000;
//	int sorArray[testCase] = {\
	2,5,7,1,8,\
	3,0,23,5,21,\
	9,12,5,1,2,\
	0,1,34,21,12,\
	8,4,2,7,1};
	int *sorArray = new int [testCase];
	randomInts(sorArray,testCase);
	int ts = clock();
	NormalRMQ nrmq(sorArray,testCase);
	int te = clock();
	cout << "init cost " << (te - ts) << endl;
	int s,e;
	int m1,index1,m2,index2;
	int testTimes = 10000;
	int errorCounts = 0;
	ts = clock();
	for(int i = 0;i < testTimes;++ i)
	{
		s = rand() % testCase;
		e = rand() % testCase;
		while(s > e)
		{
			e = rand() % testCase;
		}
		m1 = nrmq.queryMin(s,e,&index1);
		
		m2 = minValue(sorArray,s,e,&index2);
		if(m1 != m2 || index1 != index2)
		{
			++ errorCounts;
		}
		
	}
	if(0 != errorCounts)
		cout << errorCounts <<" errors." << endl;
	else
		cout << "ok" << endl;
	te = clock();
	cout << "query " << testTimes << " times,cost " << (te - ts) << " millsecs." << endl;
	system("PAUSE");
}

void main()
{
//	initRMQ1_1Impl();
//	while (true)
//	{
//		testCorrect();
	//	testFileCorrect();
//	}
//	testCorrect();
//	system("PAUSE");
//	testRMQ1_1();
//	testRMQ1_1Correct();
//	testRMQImpl();
//	testSectionMinPerformance();
//	testRMQSmallFileCorrect();
//	testSectionCorrect();
	testMM();
//	testRMQ();
//	testNormalRMQ();
	return;
}