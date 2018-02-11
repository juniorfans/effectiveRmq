#include "CartesianTree.h"

//#define VERIFY_TREE_CREATE
//#define VERIFY_EULERPATH_INIT

CartesianTree::CartesianTree(int *theArray,int theLen)
:sorArray(theArray),arrayLen(theLen)
{
	createTree();
#ifdef VERIFY_TREE_CREATE
	verifyTree();
#endif

	initEulerPath();
#ifdef VERIFY_EULERPATH_INIT
	verifyEulerPath();
#endif

	destroyTree();

//	displayEulerPath();
}

CartesianTree::~CartesianTree(void)
{
	//destroyEulerPath();
	//cout << "over" << endl;
}

void CartesianTree::createTree()
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
}


void CartesianTree::initEulerPath()
{
	firstPosInEulerArray = new int[arrayLen];	//记录元素第一次出现在欧拉序列中的位置。在元素入栈时记录即可。
	memset(firstPosInEulerArray,-1,sizeof(int) * arrayLen);
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
	firstPosInEulerArray[root->key_] = curEulerIndex;
	
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
		if(NULL != left && !visited[left->key_])	//left 入栈
		{
			++ top;
			stack[top] = left;
			visited[left->key_] = true;
			firstPosInEulerArray[left->key_] = curEulerIndex;
			++ curLevel;
		}
		else if(NULL != right && !visited[right->key_])	//right 入栈
		{
			++ top;
			stack[top] = right;
			visited[right->key_] = true;
			firstPosInEulerArray[right->key_] = curEulerIndex;
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

void CartesianTree::verifyTree()
{
	vector<TreeNode *> path;
	root->midTraversalEx(arrayLen,path);
	vector<TreeNode*> ::iterator it = path.begin();
	assert(path.size() == arrayLen);
	TreeNode *curNode;
	for(int i = 0;it != path.end();++ it,++ i)
	{
		curNode = *it;
		assert(i == curNode->key_ && sorArray[i] == curNode->value_);
	}

	cout << "verify cartesianTree ok" << endl;
}


void CartesianTree::verifyEulerPath()
{
	for (int i = 0;i < arrayLen;++ i)
	{
		assert(i == indexArray[firstPosInEulerArray[i]]);
	}
	cout << "verify eulerPath ok" << endl;
}

void CartesianTree::destroyTree()
{
	if(NULL != theNodes)
	{
		delete[] theNodes;	theNodes = NULL;
	}
}

void CartesianTree::destroyEulerPath()
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

void CartesianTree::displayTree()
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

void CartesianTree::displayEulerPath()
{
	int eulerArrayLen = (arrayLen << 1) - 1;
	for(int i = 0;i < eulerArrayLen;++ i)
	{
		cout << indexArray[i] << "," << eulerArray[i] << endl;
	}

	for (int i = 0;i < arrayLen;++ i)
	{
		cout << sorArray[i] << " fist occ : " << firstPosInEulerArray[i] << endl;
	}
}


void TreeNode::midTraversalEx(int maxLen,vector<TreeNode*>& path)
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

void TreeNode::midTraversal(vector<TreeNode*>& path)
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


extern void random1_1Ints(int *sorArray,int len);

void randomTestCorrect()
{
	const static int testCase = 1000000;
	int *array = new int[testCase];
	while (true)
	{
		random1_1Ints(array,testCase);
		CartesianTree ct(array,testCase);
	}
}

extern int testCheck();
/*
int main()
{

//	int array[8] = {19,15,17,16,3,11,14,2};
//	CartesianTree ct(array,8);
	randomTestCorrect();
	//	testCheck();
	system("PAUSE");
	return 0;
}
*/