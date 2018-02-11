#pragma once

#define SET_RESULT_IF_NOT_NULL(theIndexPointer,theIndex) if(NULL!=(theIndexPointer)) *(theIndexPointer)=(theIndex)
class SectionMin
{
public:
	SectionMin(int *height,int len,int maxQuerySection);
	~SectionMin();

	int query(int start,int end,int *index = NULL);
	int queryEx(int start,int end,int *index = NULL);
	int SectionMin::queryEx_NoLimit(int start,int end,int *index = NULL);
	int getMaxQuerySection(){return _maxQuerySection;}
	void toString();
private:
	void init();
	void showRMQ();
	void SectionMin:: memoryConstruct();
	void SectionMin:: memoryDestroy();
	inline int * SectionMin ::oneLevelBucket(int element);
	inline int * SectionMin ::twoLevelBucket(int element,int k);
	int *_sorArray,_len,_maxKLevel,*rmq,_maxQuerySection;
	//_maxQuerySection : 最多允许查询第 i 个及后面 _maxQuerySection 个元素（总共 _maxQuerySection + 1个元素）中的最小值
	//也即可以处理的查询区域最大值为  _maxQuerySection + 1
#define MIN_INDEX(smallIndex,bigIndex) (INDEX_TO_RESULT(smallIndex) <= INDEX_TO_RESULT(bigIndex) ? (smallIndex) : (bigIndex))
#define MIN_RESULT(smallIndex,bigIndex) INDEX_TO_RESULT(MIN_INDEX(smallIndex,bigIndex))
#define INDEX_TO_RESULT(index) *(_sorArray+(index))
};
