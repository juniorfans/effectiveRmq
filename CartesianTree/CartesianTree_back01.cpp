#include <vector>
using std::vector;
#include <queue>
using std::queue;
#include <cassert>
#include <iostream>
using std::cout;
using std::endl;
#include <ctime>

class TreeNode
{
public:
	TreeNode(int theValue = 0,int key = 0):
	  value_(theValue),key_(key)
	  {
		  left = right = NULL;
		  parent = NULL;
	  }

	  void midTraversalEx(int maxLen,vector<TreeNode*>& path)
	  {
		  TreeNode **stack = new TreeNode* [maxLen];
		  bool *visited = new bool[maxLen];
		  memset(visited,false,sizeof(bool) * maxLen);
		  int top = -1;
		  stack[++ top] = this;
		  visited[this->key_] = true;
		  int lastTop = top;
		  while (-1 != top)
		  {
			  lastTop = top;
			  //当前元素先出栈；依次将：右孩子，自己，左孩子入栈，这样才能保证出栈的顺序是左孩子，自己，右孩子，也即中序。
			  TreeNode *curNode = stack[top];
			  -- top;
			  TreeNode *left = curNode->left;
			  TreeNode *right = curNode->right;
			  if (NULL != right && !visited[right->key_])
			  {
				  visited[right->key_] = true;
				  stack[++ top] = right;
			  }

			  stack[++ top] = curNode;

			  if(NULL != left && !visited[left->key_])
			  {
				  visited[left->key_] = true;
				  stack[++ top] = left;
			  }
			  if(lastTop == top)	//如果没有进栈的元素，则当前元素出栈
			  {
				  -- top;
				  path.push_back(curNode);
			  }
		  }
	  }

	  void midTraversal(vector<TreeNode*>& path)
	  {
		  if(NULL != left)
		  {
			  left->midTraversal(path);
		  }

		  path.push_back(this);

		  if(NULL != right)
		  {
			  right->midTraversal(path);
		  }
	  }
	  int value_,key_;
	  TreeNode *left,*right;
	  TreeNode *parent;
};

#define VERIFY_CREATE_CTTREE

class CartesianTree
{
public:
	CartesianTree(int *theArray,int theLen):
	  sorArray(theArray),arrayLen(theLen)
	  {
		  createTree();
		  initEulerPath();
		  destroyTree();
		  displayEulerPath();
	  }
	~CartesianTree()
	{
		destroyEulerPath();
		cout << "over" << endl;
	}

	int *getEulerPath(){return eulerArray;}
	int *getIndexArray(){return indexArray;}
	
private:
	void createTree()
	{
		TreeNode ** stack = new TreeNode* [arrayLen];
		theNodes = new TreeNode[arrayLen];
		TreeNode *curNode;
		int k,top = -1;//k for 
		for(int i = 0;i < arrayLen;++ i)
		{
			curNode = theNodes + i;
			curNode->key_ = i;
			curNode->value_ = sorArray[i];
			//栈中所有大于当前元素的元素出栈，最后一个出栈的元素作为当前元素的左孩子
			k = top;
			TreeNode *lastPopedNode = NULL;
			while (-1 != k && (*(stack + k))->value_  > curNode->value_)
			{
				-- k;
			}
			if(k != top)	//移出了元素
			{
				lastPopedNode = *(stack + k + 1);
				curNode->left = lastPopedNode;
				lastPopedNode->parent = curNode;
			}
			//当前元素成为栈顶元素的一个右孩子
			if(-1 != k)
			{
				TreeNode *stackTop = *(stack + k);
				stackTop->right = curNode;
				curNode->parent = stackTop;
			}
			*(stack + ++ k) = curNode;
			top = k;
		}
		assert(-1 != k);
		root = stack[0];
		delete[] stack;
#ifdef VERIFY_CREATE_CTTREE
		verifyTree();
#endif
	}

	void initEulerPath()
	{
		int eulerArrayLen = (arrayLen << 1) - 1;
		eulerArray = new int[eulerArrayLen];
		indexArray = new int[eulerArrayLen];
		bool *visited = new bool[arrayLen];
		memset(visited,0,sizeof(bool) * arrayLen);
		TreeNode ** stack = new TreeNode*[arrayLen];
		int curEulerIndex = 0;
		int top = -1;
		//root 入栈
		top = 0;
		stack[0] = root;
		visited[root->key_] = true;
		int curLevel = 0;
		
		while (-1 != top)
		{
			//只要经历了这个元素，则它就应该被放入欧拉序列
			TreeNode *topEle = stack[top];
			eulerArray[curEulerIndex] = curLevel;//topEle->value_;
			indexArray[curEulerIndex] = topEle->key_;
			++ curEulerIndex;
			TreeNode *left = topEle->left;
			TreeNode *right = topEle->right;
			if(NULL != left && !visited[left->key_])
			{
				++ top;
				stack[top] = left;
				visited[left->key_] = true;
				++ curLevel;
			}
			else if(NULL != right && !visited[right->key_])
			{
				++ top;
				stack[top] = right;
				visited[right->key_] = true;
				++ curLevel;
			}
			else //没有进栈的元素，则当前元素出栈
			{
				-- top;
				-- curLevel;
			}
		}
		assert(curEulerIndex == eulerArrayLen);
		delete[] stack;
		delete[] visited;
	}


	void verifyTree()
	{

		vector<TreeNode *> path;
		int ts = clock();
		root->midTraversal(path);
		int te = clock();
		cout << "midTraversal cost : " << (te -ts) << " millsecs" << endl;
		path.clear();
		ts = clock();
		root->midTraversalEx(arrayLen,path);
		te = clock();
		cout << "midTraversalEx cost : " << (te -ts) << " millsecs" << endl;
		vector<TreeNode*> ::iterator it = path.begin();
		assert(path.size() == arrayLen);
		TreeNode *curNode;
		for(int i = 0;it != path.end();++ it,++ i)
		{
			curNode = *it;
			assert(i == curNode->key_ && sorArray[i] == curNode->value_);
		}

		cout << "ok" << endl;
	}


	void destroyTree()
	{
		if(NULL != theNodes)
		{
			delete[] theNodes;	theNodes = NULL;
		}
	}

	void destroyEulerPath()
	{
		if(NULL != eulerArray)
		{
			delete[] eulerArray;	eulerArray = NULL;
		}
		if(NULL != indexArray)
		{
			delete[] indexArray;	indexArray = NULL;
		}
	}


	void displayTree()
	{
		vector<TreeNode *> path;
		root->midTraversal(path);
		vector<TreeNode*> ::iterator it = path.begin();
		TreeNode *curNode;
		for(;it != path.end();++ it)
		{
			curNode = *it;
			cout << "(" << curNode->key_ << "," << curNode->key_ << ")" << "\t";
		}
		cout << endl;
	}

	void displayEulerPath()
	{
		int eulerArrayLen = (arrayLen << 1) - 1;
		for(int i = 0;i < eulerArrayLen;++ i)
		{
			cout << indexArray[i] << "," << eulerArray[i] << endl;
		}
	}

private:
	int *sorArray,arrayLen;
	TreeNode *root;
	TreeNode *theNodes;
	int *eulerArray,*indexArray;
};

#include <ctime>
void random1_1Ints(int *sorArray,int len)
{
	if(0 == len) return;
	srand(time(NULL));
	sorArray[0] = rand();
	for(int i = 1;i < len;++i)
	{
		sorArray[i] = rand();
	}
}

void randomTestCorrect()
{
	const static int testCase = 10000000;
	int *array = new int[testCase];
	while (true)
	{
		random1_1Ints(array,testCase);
		CartesianTree ct(array,testCase);
	}
}


extern int testCheck();
int main()
{
	
	int array[8] = {19,15,17,16,3,11,14,2};
	CartesianTree ct(array,8);
//	randomTestCorrect();
//	testCheck();
	system("PAUSE");
	return 0;
}