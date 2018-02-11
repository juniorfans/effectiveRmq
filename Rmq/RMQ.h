#pragma once
class RMQ1_1;
class RMQ
{
public:
	RMQ(int *theArray,int theLen);
	~RMQ(void);
public:
	int queryMin(int s,int e,int *index = NULL);
private:
	void init();
private:
	int *sorArray,arrayLen;
	int *eulerPath,*indexPath,*firstOccInEulerPath;
	RMQ1_1 *theRmq1_1;
};
