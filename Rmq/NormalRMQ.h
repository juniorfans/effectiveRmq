#pragma once
class SectionMin;
class NormalRMQ
{
public:
	NormalRMQ(int *sorArray,int len);
	~NormalRMQ(void);
	int queryMin(int s,int e,int *index);
private:
	void init();
	
private:
	int *_sorArray,_len;
	int *blockMinIndexes;
	int *blockMins;
	int blockCounts;
	SectionMin *sm;
};
