#pragma once


template<typename T>
struct  BinaryTreeNode
{
	BinaryTreeNode<T>* _pLeft;
	BinaryTreeNode<T>* _pRight;
	T _data;
	BinaryTreeNode(const T& data=T())
		:_pLeft(NULL)
		, _pRight(NULL)
		, _data(data)
	{}
	BinaryTreeNode(const T& data, BinaryTreeNode<T>* pleft, BinaryTreeNode<T>* pRight)
	{
		_data = data;
		_pLeft = pleft;
		_pRight = pRight;
	}

};

