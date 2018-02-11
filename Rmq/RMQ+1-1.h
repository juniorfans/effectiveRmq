#pragma once

//#define CHECK_RMQ1_1_INIT
//#define VERIFY_MODEL1_1_CREATE
#define ACCELEARTION
#define SET_RESULT_IF_NOT_NULL(theIndexPointer,theIndex) if(NULL!=(theIndexPointer)) *(theIndexPointer)=(theIndex)
class Model1_1_MinValue
{
public:
	Model1_1_MinValue(int theL);
	~Model1_1_MinValue();
	
	#ifdef VERIFY_MODEL1_1_CREATE
	void depose(int ,int *,int *);
	void verify();
	#endif
	int queryMin(int num,int *index = NULL);
	
private:
	#ifdef VERIFY_MODEL1_1_CREATE
	inline int minValueDirectly(int positive,int negative);
	#endif
	void init();
	void Model1_1_MinValue::minValue(int positive,int negative);
	inline int Model1_1_MinValue:: lastMin2n(int num);
	inline void addMin(int,int );
	
private:
	int L;
	int maxOdd;
	int *theMinValues;
#ifdef VERIFY_MODEL1_1_CREATE
	int *theMinValues_directyly;
	int *theMinIndexs_directyly;
#endif // BENCHMARK_CMP
	int *theSubs;
	int *theMinIndexes;
#define MIN_VALUES_INDEX(lm2n) (((lm2n) + maxOdd) >> 1)

};



class RMQ1_1_SMALL
{
public:
	RMQ1_1_SMALL(){}
	void reConstruct(int *theArray,int theLen,Model1_1_MinValue *theRmqImpl)
	{
		sorArray = theArray;
		len = theLen;
		rmqImpl = theRmqImpl;
		init();
	}
	RMQ1_1_SMALL(int *theArray,int theLen,Model1_1_MinValue *theRmqImpl):
	  sorArray(theArray),len(theLen),rmqImpl(theRmqImpl)
	{
		init();
	}
	~RMQ1_1_SMALL()
	{
	}
	
	int queryMin(int s,int e,int *index = NULL);
private:
	void init();
	void show1_1();
private:
	int *sorArray,len;
	Model1_1_MinValue *rmqImpl;
	int theV;
	int thePositive,theNegative;
	int vRadix;
#define QUERY_MIN_RESULT(curMin,theIndex) \
	(curMin) < 0 ? (((NULL != index ? (*(index) = (theIndex)+1) : 1)) ? (curMin) + sorArray[s] : (curMin) + sorArray[s]) : \
	(((NULL != index ? *index = s : 1)) ? sorArray[s] : sorArray[s])
//#define SET_MIN_INDEX_IN_SORCEARRAY (OUT_indexPointer,IN_indexValue,IN_theMinValue) if(IN_theMinValue)
};

class SectionMin;

//#define CMP_GENERATE_RMQ1_1_SMALL_EVERYTIME
//#define VERIFY_SECTION_MINS

class RMQ1_1
{
public:
	RMQ1_1(int *theArray,int theLen);
	~RMQ1_1();
	
	int queryMin(int s,int e,int *index = NULL);
private:
	void init();
	void showSectMins();
private:
	int *sorArray;
	int len;
	int *sectionMins;
	int sectionCounts;
	Model1_1_MinValue *theRmqImpl;
	SectionMin *theSectMin;
	RMQ1_1_SMALL *theRmqSs;
#define SECTION_LEN 20
};