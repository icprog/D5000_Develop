/*
 * Tree.h
 *
 *  Created on: 2012-5-10
 *      Author: BetaKDE,haozeyun
 */

#ifndef TREE_H_
#define TREE_H_

#include <list>
#include <iostream>

using namespace std;

#define _NODE_UNPROC	4
#define _NODE_DELETE_   1//判断节点是否被删除的标记位

const bool DEBUG_TREE = false;

//树的节点
template<class T>
class TreeNode
{
public:
	TreeNode()
	{
		fathernode = NULL;
		childnode = NULL;
		next = NULL;
		flag = 0;
		flag |= _NODE_UNPROC;
		flag &= ~_NODE_DELETE_;
	}

	~TreeNode()
	{
		fathernode = NULL;
		childnode = NULL;
		next = NULL;
	}

	T data; //节点的值
	TreeNode* fathernode; //指向父节点
	TreeNode* childnode; //指向孩子节点
	TreeNode* next; //下一个节点
	int flag;
};



template<class T>
class Tree
{
public:
	TreeNode<T>* root;//树的根节点指针
	list<TreeNode<T>* >  List;

public:
	//构造函数，初始化树为空
	Tree();
	//析构函数
	~Tree();

	//树的增加叶子节点函数    传入父节点指针和当前节点的值
	TreeNode<T>* TreeAddLeaf(TreeNode<T>* father,T data);

	//增加子树函数
	void TreeAddSubTree(TreeNode<T>* father,const Tree<T> *pTree, TreeNode<T>* p = NULL);

	//树的删除子树函数
	bool TreeRemoveSubTree(TreeNode<T>* &p);

	//获取指定节点的子树
	Tree* GetChildTree(TreeNode<T>* p);

	//求指定两点之间所经历的节点链表
	bool GetPath(TreeNode<T>* p1,TreeNode<T>* p2,list<TreeNode<T>* > &List);

	//求指定两点之间的距离
	int LengthOfRegion(TreeNode<T>* p1,TreeNode<T>* p2);

	//返回指定节点的父节点
	TreeNode<T>* GetParent(TreeNode<T>* p) const;

	//返回指定节点的第一个孩子节点
	TreeNode<T>* GetChildNode(TreeNode<T>* p) const;

	//返回指定节点的下一个兄弟节点
	TreeNode<T>* GetNextBrotherNode(TreeNode<T>* p) const;

	//深度优先遍历
	bool DepthFirstErgodic(TreeNode<T>* p,list<TreeNode<T>* > &nodelist);

	//广度优先遍历
	bool BreadthFirstErgodic(TreeNode<T>* p,list<TreeNode<T>* > &nodelist);

	//判断两个节点中谁是祖先节点
	bool JudgeFather(TreeNode<T>*, TreeNode<T>*);

	void ChangeRoot(TreeNode<T>* pNode, Tree<T> &new_tree, TreeNode<T>* pParent = NULL);

	const Tree<T>& operator=(const Tree<T>& tree);

	static const T GetData(TreeNode<T>* pNode)
	{
		assert(pNode != NULL);

		return pNode->data;
	}

	//将树的结点信息存成XML文件
	bool SaveTree(const char *filepath);
};

template<class T>
bool Tree<T>::SaveTree(const char *filepath)
{
	assert(filepath != NULL);

	list<TreeNode<T>* > lst;

	BreadthFirstErgodic(root, lst);

	char szTmp[1024];

	typename list<TreeNode<T>* >::iterator it;
	const char head[] = "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n<Tree>\r\n";
	const char tail[] = "</Tree>";

	FILE *fp = fopen(filepath, "w+");

	if(fp == NULL) return false;

	fwrite(head, 1, strlen(head), fp);

	for(it = lst.begin(); it != lst.end(); it++)
	{
		string strChild((*it)->data.GetText());
		string strParent((*it)->fathernode == NULL ? "" : (*it)->fathernode->data.GetText());

		snprintf(szTmp, 1024, "<node id=\"%s\" type=\"%d\" parent=\"%s\"/>\r\n", strChild.c_str(), (*it)->data.GetType(), strParent.c_str());
		fwrite(szTmp, 1, strlen(szTmp), fp);
	}

	fwrite(tail, 1, strlen(tail), fp);

	fclose(fp);

	return true;
}

template<class T>
Tree<T>::Tree()
{
	root = NULL;
}

template<class T>
Tree<T>::~Tree()
{
	TreeRemoveSubTree(root);
}

//树的增加叶子节点函数
template<class T>
TreeNode<T>* Tree<T>::TreeAddLeaf(TreeNode<T>* father, T data)
{
	//if(DEBUG_TREE) cout << __FUNCTION__ << ":" << data << endl;
	TreeNode<T>* pNewTree = new TreeNode<T>;

	pNewTree->childnode = NULL;
	pNewTree->fathernode = NULL;
	pNewTree->next = NULL;

	TreeNode<T>* ptem = NULL;
	if (father == NULL)
	{
		root = pNewTree;
		pNewTree->fathernode = NULL;
		pNewTree->childnode = NULL;
		pNewTree->next = NULL;
	}
	else if (father->childnode == NULL)
	{
		father->childnode = pNewTree;
		pNewTree->fathernode = father;
		pNewTree->childnode = NULL;
		pNewTree->next = NULL;
	}
	else if (father->childnode != NULL)
	{
		ptem = father->childnode;
		while (ptem->next!=NULL)
		{
			ptem = ptem->next;
		}
		ptem->next = pNewTree;
		pNewTree->fathernode = father;
		pNewTree->childnode = NULL;
		pNewTree->next = NULL;
	}
	pNewTree->data = data;
	return pNewTree;
}

//增加子树函数
template<class T>
void Tree<T>::TreeAddSubTree(TreeNode<T>* father,const Tree<T> *pTree, TreeNode<T>* p)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;

	assert(pTree != NULL);

	if(p == NULL) p = pTree->root;

	if(p == NULL) return;

	TreeNode<T>* newfather = TreeAddLeaf(father, p->data);

	TreeNode<T> *pChild = pTree->GetChildNode(p);

	while(pChild != NULL)
	{
		TreeAddSubTree(newfather, pTree, pChild);

		pChild = pTree->GetNextBrotherNode(pChild);
	}
}

//树的删除叶子节点及子树函数
template<class T>
bool Tree<T>::TreeRemoveSubTree(TreeNode<T>* &p)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;

	if (p == NULL)
	{
		if(DEBUG_TREE) cout << "树为空！" << endl;
		return false;
	}
#if 1
	else
	{
		if (p->fathernode == NULL) //为根结点
		{
			//删除所有孩子结点
			TreeNode<T> *tmp_child = p->childnode;

			while (tmp_child != NULL)
			{
				TreeNode<T> *next = tmp_child->next;

				TreeRemoveSubTree(tmp_child);

				tmp_child = next;
			}

			//删除自己
			delete p;
			p = NULL;
		}
		else //存在父结点
		{
			TreeNode<T> *tmp = p->fathernode->childnode;
			TreeNode<T> *tmp1 = tmp;

			//在父结点的所有孩子结点中查找自己
			while (true)
			{
				//找到自己
				if (tmp == p)
				{
					//自己是父结点的第一个孩子结点,删除时只需要将父结点的孩子结点指针指向
					//紧挨着自己的下一个兄弟结点(如果有)
					if (tmp == p->fathernode->childnode) //
					{
						//有紧挨着自己的兄弟结点
						if (p->next != NULL)
						{
							//将父结点的孩子结点指针指向紧挨着自己的下一个孩子结点
							p->fathernode->childnode = p->next;

							//删除自己的所有孩子结点
							TreeNode<T> *tmp_child = p->childnode;

							while (tmp_child != NULL)
							{
								TreeNode<T> *next = tmp_child->next;

								TreeRemoveSubTree(tmp_child);

								tmp_child = next;
							}

							//删除自己
							delete p;
							p = NULL;
						}
						//独生子
						else
						{
							//父结点再也没有孩子
							p->fathernode->childnode = NULL;

							//删除自己的所有孩子结点
							TreeNode<T> *tmp_child = p->childnode;

							while (tmp_child != NULL)
							{
								TreeNode<T> *next = tmp_child->next;

								TreeRemoveSubTree(tmp_child);

								tmp_child = next;
							}

							//删除自己
							delete p;
							p = NULL;
						}
					}
					//自己不是父结点的第一个孩子结点,删除时只需要将自己兄长的Next指针指向
					//紧挨着自己的下一个孩子结点(如果有)
					else
					{
						//有紧挨着自己的兄弟结点
						if (p->next != NULL)
						{
							//兄长的下一个兄弟结点指针指向紧挨着自己的下一个兄弟结点
							tmp1->next = p->next;

							//删除自己的所有孩子结点
							TreeNode<T> *tmp_child = p->childnode;

							while (tmp_child != NULL)
							{
								TreeNode<T> *next = tmp_child->next;

								TreeRemoveSubTree(tmp_child);

								tmp_child = next;
							}

							//删除自己
							delete p;
							p = NULL;
						}
						//自己是最小的孩子
						else
						{
							//父结点再也没有孩子
							tmp1->next = NULL;

							//删除自己的所有孩子结点
							TreeNode<T> *tmp_child = p->childnode;

							while (tmp_child != NULL)
							{
								TreeNode<T> *next = tmp_child->next;

								TreeRemoveSubTree(tmp_child);

								tmp_child = next;
							}

							//删除自己
							delete p;
							p = NULL;
						}
					}

					break;
				}
				else
				{
					tmp1 = tmp;
					tmp = tmp->next;

					if (tmp == NULL)
						break;
				}
			}
		}

		return true;
	}
#else
	if (p->childnode == NULL)
	{
		//当树只有根节点
		if (p->fathernode == NULL)
		{
			delete p;
			return true;
		}
		//删除的叶子节点为孩子节点
		if (p->fathernode->childnode == p)
		{
			if (p->next == NULL)
			{
				p->fathernode->childnode = NULL;
				delete p;
				return true;
			}
			else
			{
				p->fathernode->childnode = p->fathernode->childnode->next;
				delete p;
				return true;
			}
		}

		//删除的叶子节点为兄弟节点
		else
		{
			TreeNode* ptem = NULL;
			ptem = p->fathernode->childnode; //孩子节点的指针
			while (ptem->next != NULL)
			{
				if (ptem->next == p && ptem->next->next != NULL)
				{
					ptem->next = ptem->next->next;
					delete p;
				}
				else if (ptem->next == p && ptem->next->next == NULL)
				{
					delete p;
				}

			}
			return true;
		}
	}
	else
	{
		if (p->childnode != NULL)
			DepthFirstErgodic(p,nodelist);
		//如果删除的子树根节点有兄弟节点,且自己第一个孩子节点
		if (p->next != NULL)
		{
			p->fathernode->childnode = p->fathernode->childnode->next;
			return true;
		}
		//如果删除的子树根节点为任意兄弟节点且不为最后一个
		if (p->next != NULL)
		{
			TreeNode* tem = NULL;
			tem = p->fathernode->childnode;
			while (tem->next != p)
				tem = tem->next;
			tem->next = tem->next->next;
			return true;
		}
		else
		{
			TreeNode* tem = NULL;
			tem = p->fathernode->childnode;
			while (tem->next != p)
				tem = tem->next;
			tem->next = NULL;
		}
		return true;
	}
#endif
}

//获取指定节点的子树
template<class T>
Tree<T>* Tree<T>::GetChildTree(TreeNode<T>* p)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	Tree<T>* pTree = new Tree<T>;
	if (p->childnode)
		GetChildTree(p->childnode);
	if (p->next)
		GetChildTree(p->next);
	pTree->root = p;
	return pTree;
}
//求指定两点之间的距离
template<class T>
int Tree<T>::LengthOfRegion(TreeNode<T>* p1, TreeNode<T>* p2)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	int maxlen = 0;
	if (p1 == p2)return 0;
	if (JudgeFather(p1, p2)) //p2为祖先节点
	{
			while (p1->fathernode != p2)
			{
				++maxlen;
				p1 = p1->fathernode;
			}
	}
	else
	{
		while(p2->fathernode != p1)
		{
			++ maxlen;
			p2 =p2->fathernode;
		}
	}
	return maxlen + 1;
}

//求指定两点之间所经历的节点链表
template<class T>
bool Tree<T>::GetPath(TreeNode<T>* p1, TreeNode<T>* p2, list<TreeNode<T>* >&List)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	if (p1 == p2 || p1 == NULL || p2 == NULL)
	{
		if(DEBUG_TREE) cout << "所选节点相同，无效！" << endl;
		return false;
	}

	if (JudgeFather(p1, p2)) //p2为祖先节点
	{
		List.push_back(p1);
		while (p1->fathernode != p2)
		{
			List.push_back(p1->fathernode);
			p1 = p1->fathernode;

			if (p1 == NULL)
			{
				List.clear();
				return false;
			}
		}
		List.push_back(p2);
	}
	else
	{
		List.push_front(p2);
		while (p2->fathernode != p1)
		{
			List.push_front(p2->fathernode);
			p2 = p2->fathernode;

			if(p2 == NULL)
			{
				List.clear();
				return false;
			}
		}
		List.push_front(p1);
	}
	return true;
}
//判断两个接着中，谁是祖先节点
template<class T>
bool Tree<T>::JudgeFather(TreeNode<T>* p1, TreeNode<T>* p2)
{
	while (p1->fathernode != NULL)
	{
		if (p1->fathernode == p2)
			return true;
		p1 = p1->fathernode;
	}
	while (p2->fathernode != NULL)
	{
		if (p2->fathernode == p1)
			return false;
		p2 = p2->fathernode;
	}
	return false;
}
//返回指定节点的父节点
template<class T>
TreeNode<T>* Tree<T>::GetParent(TreeNode<T>* p) const
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	return (p->fathernode);
}

//返回指定节点的第一个孩子节点
template<class T>
TreeNode<T>* Tree<T>::GetChildNode(TreeNode<T>* p) const
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	return (p->childnode);
}

//返回指定节点的下一个兄弟节点
template<class T>
TreeNode<T>* Tree<T>::GetNextBrotherNode(TreeNode<T>* p) const
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	return (p->next);
}

//深度优先遍历
template<class T>
bool Tree<T>::DepthFirstErgodic(TreeNode<T>* p,list<TreeNode<T>* > &nodelist)
{
	if (p == NULL)
	{
		if(DEBUG_TREE) cout << "树为空！遍历结束。" << endl;
		return false;
	}
	nodelist.push_front(p);
	if (p->childnode == NULL||p->flag == _NODE_DELETE_)
	{
		return true;
	}
	TreeNode<T>* ptem = NULL;
	if (DepthFirstErgodic(p->childnode,nodelist))
	{
		ptem = p->childnode->next;
		while (ptem)
		{
			if (!DepthFirstErgodic(ptem,nodelist))
			{
				return false;
			}
			ptem = ptem->next;
		}
		return true;
	}
	return false;
}
//广度优先遍历(层遍历)
template<class T>
bool Tree<T>::BreadthFirstErgodic(TreeNode<T>* p,list<TreeNode<T>*> &coll)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	if (p == NULL)
	{
		if(DEBUG_TREE) cout << "树为空，遍历结束！" << endl;
		return false;
	}
	if (p->childnode == NULL)
	{
		if(DEBUG_TREE) cout << "只有根节点，遍历结束！" << endl;
		if (!(p->flag & _NODE_DELETE_))
		{
			coll.push_back(p);
		}
		return true;
	}
	coll.clear();
	list<TreeNode<T>*> lst_temp;
	TreeNode<T>* ptem = NULL;
	lst_temp.push_back(p);

	
	
	while (lst_temp.size() > 0)
	{
		if ((*lst_temp.begin())->childnode != NULL )
		{
			if(!((*lst_temp.begin())->childnode->flag & _NODE_DELETE_))
			{
				lst_temp.push_back((*lst_temp.begin())->childnode);
			}

			ptem = (*lst_temp.begin())->childnode->next;

			while (ptem != NULL)
			{
				if (!(ptem->flag & _NODE_DELETE_))
				{
					lst_temp.push_back(ptem);
				}

				ptem = ptem->next;
			}
		}

		coll.push_back(lst_temp.front());
		lst_temp.pop_front();
	}
	
	
//	while (lst_temp.size() > 0)
//	{
//	 if ((*lst_temp.begin())->childnode != NULL && !((*lst_temp.begin())->childnode->flag & _NODE_DELETE_))
//		{
//			lst_temp.push_back((*lst_temp.begin())->childnode);
//		ptem = (*lst_temp.begin())->childnode->next;
//	     	while (ptem != NULL)
//			{
//				if(!(ptem->flag & _NODE_DELETE_))
//				{
//					lst_temp.push_back(ptem);
//				}
//				ptem = ptem->next;
//			}
//		}
//
//		coll.push_back(lst_temp.front());
//		if(DEBUG_TREE) cout << __FUNCTION__ << __LINE__ <<endl;
//		lst_temp.pop_front();
//	}

	return true;
}

//将pnode设置为树的根结点
template<class T>
void Tree<T>::ChangeRoot(TreeNode<T>* pNode, Tree<T> &new_tree, TreeNode<T>* pRoot)
{
	assert(pNode != NULL);

	TreeNode<T>* pnewRoot = new_tree.TreeAddLeaf(pRoot, pNode->data);

	TreeNode<T> *pParent = GetParent(pNode);

	if(pParent != NULL && pParent != pNode)
	{
//		TreeNode<T>* pnewRoot = new_tree.TreeAddLeaf(pnewRoot, pParent->data);
		ChangeRoot(pParent, new_tree, pnewRoot);
	}

	if (pParent == NULL)
	{
		return;
	}
	TreeNode<T> *pChild = GetChildNode(pParent);

	while(pChild != NULL)
	{
		if(pChild != pNode)
		{
			new_tree.TreeAddSubTree(pnewRoot, this, pChild);
		}

		pChild = GetNextBrotherNode(pChild);
	}
}

template<class T>
const Tree<T>& Tree<T>::operator=(const Tree<T>& tree)
{
	TreeRemoveSubTree(root);
	TreeAddSubTree(NULL, &tree);

	return *this;
}
#endif /* TREE_H_ */
