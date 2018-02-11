#include <iostream>
#include <queue>
#include <cassert>
#include <ctime>
using namespace std;
int maxnum=1;

int *a = new int[maxnum];
struct node
{
	int key;
	int parent;
	int l;
	int r;
};

node *tree = new node[maxnum];


void Init()
{
	int i;
	for(i=0;i<maxnum;i++)
		tree[i].parent=tree[i].l=tree[i].r=-1;  //��ʼ��
}

int Build_Tree()
{
	int ts = 0;
	int i,top,k;
	int *stack = new int [maxnum];
	top=-1;
	for(i=0;i<maxnum;i++)
	{
		k=top;
		int tts = clock();
		while(k>=0 && a[stack[k]]>a[i])  //ջ�бȵ�ǰԪ�ش�Ķ���ջ
			k--;
		ts += (clock() - tts);
		if(k!=-1)  //find it��ջ��Ԫ��û����ȫ��ջ����ǰԪ��Ϊջ��Ԫ�ص��Һ���
		{
			tree[i].parent=stack[k];
			tree[stack[k]].r=i;
		}
		if(k<top)    //��ջ��Ԫ��Ϊ��ǰԪ�ص�����
		{
			tree[stack[k+1]].parent=i;
			tree[i].l=stack[k+1];
		}
		cout << "cycle cost : " << (ts) << endl;
		stack[++k]=i;//��ǰԪ����ջ
		top=k;//topָ��ջ��Ԫ��
	}
	tree[stack[0]].parent=-1;//������ɺ��ջ��Ԫ�ؾ��Ǹ�
	return stack[0];
}


//int times = 0;
void inorder(int node,vector<int> &path)
{
	if(node!=-1)
	{
		inorder(tree[node].l,path);
		path.push_back(tree[node].key);
		inorder(tree[node].r,path);
	}
}


void verify(int root)
{
	vector<int> *path = new vector<int>;
	inorder(root,*path);
	assert(maxnum == path->size());
	for (int i = 0;i < maxnum;++ i)
	{
		assert(a[i] == path->at(i));
	}
	cout << "ok" << endl;
}

void levelorder(int node)
{
	queue<int> q;
	q.push(node);
	while(!q.empty())
	{
		int k=q.front();
		q.pop();
		cout<<tree[k].key<<endl;
		if(tree[k].l!=-1)
			q.push(tree[k].l);
		if(tree[k].r!=-1)
			q.push(tree[k].r);
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

int testCheck()
{
	while (true)
	{
		int i;
		Init();
		random1_1Ints(a,maxnum);
		for (i = 0;i < maxnum;++ i)
		{
			tree[i].key = a[i];
		}

		int root=Build_Tree();

		cout << "over" << endl;
	}
	

	//verify(root);
	//levelorder(root);
	return 0;
}