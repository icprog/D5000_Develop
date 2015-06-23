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
#define _NODE_DELETE_   1//�жϽڵ��Ƿ�ɾ���ı��λ

const bool DEBUG_TREE = false;

//���Ľڵ�
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

	T data; //�ڵ��ֵ
	TreeNode* fathernode; //ָ�򸸽ڵ�
	TreeNode* childnode; //ָ���ӽڵ�
	TreeNode* next; //��һ���ڵ�
	int flag;
};



template<class T>
class Tree
{
public:
	TreeNode<T>* root;//���ĸ��ڵ�ָ��
	list<TreeNode<T>* >  List;

public:
	//���캯������ʼ����Ϊ��
	Tree();
	//��������
	~Tree();

	//��������Ҷ�ӽڵ㺯��    ���븸�ڵ�ָ��͵�ǰ�ڵ��ֵ
	TreeNode<T>* TreeAddLeaf(TreeNode<T>* father,T data);

	//������������
	void TreeAddSubTree(TreeNode<T>* father,const Tree<T> *pTree, TreeNode<T>* p = NULL);

	//����ɾ����������
	bool TreeRemoveSubTree(TreeNode<T>* &p);

	//��ȡָ���ڵ������
	Tree* GetChildTree(TreeNode<T>* p);

	//��ָ������֮���������Ľڵ�����
	bool GetPath(TreeNode<T>* p1,TreeNode<T>* p2,list<TreeNode<T>* > &List);

	//��ָ������֮��ľ���
	int LengthOfRegion(TreeNode<T>* p1,TreeNode<T>* p2);

	//����ָ���ڵ�ĸ��ڵ�
	TreeNode<T>* GetParent(TreeNode<T>* p) const;

	//����ָ���ڵ�ĵ�һ�����ӽڵ�
	TreeNode<T>* GetChildNode(TreeNode<T>* p) const;

	//����ָ���ڵ����һ���ֵܽڵ�
	TreeNode<T>* GetNextBrotherNode(TreeNode<T>* p) const;

	//������ȱ���
	bool DepthFirstErgodic(TreeNode<T>* p,list<TreeNode<T>* > &nodelist);

	//������ȱ���
	bool BreadthFirstErgodic(TreeNode<T>* p,list<TreeNode<T>* > &nodelist);

	//�ж������ڵ���˭�����Ƚڵ�
	bool JudgeFather(TreeNode<T>*, TreeNode<T>*);

	void ChangeRoot(TreeNode<T>* pNode, Tree<T> &new_tree, TreeNode<T>* pParent = NULL);

	const Tree<T>& operator=(const Tree<T>& tree);

	static const T GetData(TreeNode<T>* pNode)
	{
		assert(pNode != NULL);

		return pNode->data;
	}

	//�����Ľ����Ϣ���XML�ļ�
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

//��������Ҷ�ӽڵ㺯��
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

//������������
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

//����ɾ��Ҷ�ӽڵ㼰��������
template<class T>
bool Tree<T>::TreeRemoveSubTree(TreeNode<T>* &p)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;

	if (p == NULL)
	{
		if(DEBUG_TREE) cout << "��Ϊ�գ�" << endl;
		return false;
	}
#if 1
	else
	{
		if (p->fathernode == NULL) //Ϊ�����
		{
			//ɾ�����к��ӽ��
			TreeNode<T> *tmp_child = p->childnode;

			while (tmp_child != NULL)
			{
				TreeNode<T> *next = tmp_child->next;

				TreeRemoveSubTree(tmp_child);

				tmp_child = next;
			}

			//ɾ���Լ�
			delete p;
			p = NULL;
		}
		else //���ڸ����
		{
			TreeNode<T> *tmp = p->fathernode->childnode;
			TreeNode<T> *tmp1 = tmp;

			//�ڸ��������к��ӽ���в����Լ�
			while (true)
			{
				//�ҵ��Լ�
				if (tmp == p)
				{
					//�Լ��Ǹ����ĵ�һ�����ӽ��,ɾ��ʱֻ��Ҫ�������ĺ��ӽ��ָ��ָ��
					//�������Լ�����һ���ֵܽ��(�����)
					if (tmp == p->fathernode->childnode) //
					{
						//�н������Լ����ֵܽ��
						if (p->next != NULL)
						{
							//�������ĺ��ӽ��ָ��ָ��������Լ�����һ�����ӽ��
							p->fathernode->childnode = p->next;

							//ɾ���Լ������к��ӽ��
							TreeNode<T> *tmp_child = p->childnode;

							while (tmp_child != NULL)
							{
								TreeNode<T> *next = tmp_child->next;

								TreeRemoveSubTree(tmp_child);

								tmp_child = next;
							}

							//ɾ���Լ�
							delete p;
							p = NULL;
						}
						//������
						else
						{
							//�������Ҳû�к���
							p->fathernode->childnode = NULL;

							//ɾ���Լ������к��ӽ��
							TreeNode<T> *tmp_child = p->childnode;

							while (tmp_child != NULL)
							{
								TreeNode<T> *next = tmp_child->next;

								TreeRemoveSubTree(tmp_child);

								tmp_child = next;
							}

							//ɾ���Լ�
							delete p;
							p = NULL;
						}
					}
					//�Լ����Ǹ����ĵ�һ�����ӽ��,ɾ��ʱֻ��Ҫ���Լ��ֳ���Nextָ��ָ��
					//�������Լ�����һ�����ӽ��(�����)
					else
					{
						//�н������Լ����ֵܽ��
						if (p->next != NULL)
						{
							//�ֳ�����һ���ֵܽ��ָ��ָ��������Լ�����һ���ֵܽ��
							tmp1->next = p->next;

							//ɾ���Լ������к��ӽ��
							TreeNode<T> *tmp_child = p->childnode;

							while (tmp_child != NULL)
							{
								TreeNode<T> *next = tmp_child->next;

								TreeRemoveSubTree(tmp_child);

								tmp_child = next;
							}

							//ɾ���Լ�
							delete p;
							p = NULL;
						}
						//�Լ�����С�ĺ���
						else
						{
							//�������Ҳû�к���
							tmp1->next = NULL;

							//ɾ���Լ������к��ӽ��
							TreeNode<T> *tmp_child = p->childnode;

							while (tmp_child != NULL)
							{
								TreeNode<T> *next = tmp_child->next;

								TreeRemoveSubTree(tmp_child);

								tmp_child = next;
							}

							//ɾ���Լ�
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
		//����ֻ�и��ڵ�
		if (p->fathernode == NULL)
		{
			delete p;
			return true;
		}
		//ɾ����Ҷ�ӽڵ�Ϊ���ӽڵ�
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

		//ɾ����Ҷ�ӽڵ�Ϊ�ֵܽڵ�
		else
		{
			TreeNode* ptem = NULL;
			ptem = p->fathernode->childnode; //���ӽڵ��ָ��
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
		//���ɾ�����������ڵ����ֵܽڵ�,���Լ���һ�����ӽڵ�
		if (p->next != NULL)
		{
			p->fathernode->childnode = p->fathernode->childnode->next;
			return true;
		}
		//���ɾ�����������ڵ�Ϊ�����ֵܽڵ��Ҳ�Ϊ���һ��
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

//��ȡָ���ڵ������
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
//��ָ������֮��ľ���
template<class T>
int Tree<T>::LengthOfRegion(TreeNode<T>* p1, TreeNode<T>* p2)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	int maxlen = 0;
	if (p1 == p2)return 0;
	if (JudgeFather(p1, p2)) //p2Ϊ���Ƚڵ�
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

//��ָ������֮���������Ľڵ�����
template<class T>
bool Tree<T>::GetPath(TreeNode<T>* p1, TreeNode<T>* p2, list<TreeNode<T>* >&List)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	if (p1 == p2 || p1 == NULL || p2 == NULL)
	{
		if(DEBUG_TREE) cout << "��ѡ�ڵ���ͬ����Ч��" << endl;
		return false;
	}

	if (JudgeFather(p1, p2)) //p2Ϊ���Ƚڵ�
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
//�ж����������У�˭�����Ƚڵ�
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
//����ָ���ڵ�ĸ��ڵ�
template<class T>
TreeNode<T>* Tree<T>::GetParent(TreeNode<T>* p) const
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	return (p->fathernode);
}

//����ָ���ڵ�ĵ�һ�����ӽڵ�
template<class T>
TreeNode<T>* Tree<T>::GetChildNode(TreeNode<T>* p) const
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	return (p->childnode);
}

//����ָ���ڵ����һ���ֵܽڵ�
template<class T>
TreeNode<T>* Tree<T>::GetNextBrotherNode(TreeNode<T>* p) const
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	return (p->next);
}

//������ȱ���
template<class T>
bool Tree<T>::DepthFirstErgodic(TreeNode<T>* p,list<TreeNode<T>* > &nodelist)
{
	if (p == NULL)
	{
		if(DEBUG_TREE) cout << "��Ϊ�գ�����������" << endl;
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
//������ȱ���(�����)
template<class T>
bool Tree<T>::BreadthFirstErgodic(TreeNode<T>* p,list<TreeNode<T>*> &coll)
{
	if(DEBUG_TREE) cout << __FUNCTION__ << endl;
	if (p == NULL)
	{
		if(DEBUG_TREE) cout << "��Ϊ�գ�����������" << endl;
		return false;
	}
	if (p->childnode == NULL)
	{
		if(DEBUG_TREE) cout << "ֻ�и��ڵ㣬����������" << endl;
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

//��pnode����Ϊ���ĸ����
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
