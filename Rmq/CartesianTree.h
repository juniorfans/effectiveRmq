#pragma once
#include <vector>
using std::vector;
#include <queue>
using std::queue;
#include <cassert>
#include <iostream>
using std::cout;
using std::endl;
#include <ctime>
class TreeNode;

class CartesianTree
{
public:
	CartesianTree(int *theArray,int theLen);
	  ~CartesianTree();

	  /************************************************************************/
	  /* 
		欧拉序列，即笛卡尔树的深度优先遍历到每个元素的层数序列
	  */
	  /************************************************************************/
	  int *getEulerPath(){return eulerArray;}

	  /************************************************************************/
	  /* 
		欧拉序列中每个元素对应的原序列的索引号
	  */
	  /************************************************************************/
	  int *getIndexArray(){return indexArray;}

	  /************************************************************************/
	  /* 
		每个元素第一次出现在欧拉序列中的位置
	  */
	  /************************************************************************/
	  int *getFirstOccInEulerPath(){return firstPosInEulerArray;}

private:
	void createTree();
	void initEulerPath();
	void verifyTree();
	void verifyEulerPath();
	void destroyTree();
	void destroyEulerPath();
	void displayTree();
	void displayEulerPath();
private:
	int *sorArray,arrayLen;
	TreeNode *root;
	TreeNode *theNodes;
	int *eulerArray;			//欧拉序列，即笛卡尔树的深度优先遍历到每个元素的层数序列
	int *indexArray;			//欧拉序列中每个元素对应的原序列的索引号
	int *firstPosInEulerArray;	//每个元素第一次出现在欧拉序列中的位置
};


class TreeNode
{
public:
	TreeNode(){left = right = parent = NULL;};
	void midTraversalEx(int maxLen,vector<TreeNode*>& path);
	void midTraversal(vector<TreeNode*>& path);
	int value_,key_;
	TreeNode *left,*right;
	TreeNode *parent;
};