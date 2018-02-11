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
		ŷ�����У����ѿ�������������ȱ�����ÿ��Ԫ�صĲ�������
	  */
	  /************************************************************************/
	  int *getEulerPath(){return eulerArray;}

	  /************************************************************************/
	  /* 
		ŷ��������ÿ��Ԫ�ض�Ӧ��ԭ���е�������
	  */
	  /************************************************************************/
	  int *getIndexArray(){return indexArray;}

	  /************************************************************************/
	  /* 
		ÿ��Ԫ�ص�һ�γ�����ŷ�������е�λ��
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
	int *eulerArray;			//ŷ�����У����ѿ�������������ȱ�����ÿ��Ԫ�صĲ�������
	int *indexArray;			//ŷ��������ÿ��Ԫ�ض�Ӧ��ԭ���е�������
	int *firstPosInEulerArray;	//ÿ��Ԫ�ص�һ�γ�����ŷ�������е�λ��
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