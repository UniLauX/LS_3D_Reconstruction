#pragma once
#include <vector>
#include <algorithm>
using namespace std;
template<class T>



class MyHeap
{
private:
   //������һ������ָ������ָ��Ƚ϶�����Ԫ�ش�С�ĺ���
	typedef    bool (*COMP_FUNC)(const T&, const T&);
	COMP_FUNC _comp;
	vector<T> _data;

public:
	
	//�����ָ���������ָ�������ֵ����ô���Զ�������ʵ�ֵ� operator < ����
	MyHeap(COMP_FUNC compare_function = NULL) :_comp(compare_function)
		     {
			         if(NULL == _comp)
	                   make_heap(_data.begin(), _data.end());
			          else
		               make_heap(_data.begin(), _data.end(), _comp);
		      }


	  //�����������Ԫ�أ�˳��Ϊ�����(push_back)�������ѽṹ(push_heap)
	void add(const T& item)
		     {
			         _data.push_back(item);
			         if(NULL == _comp)
				             push_heap(_data.begin(), _data.end());
			         else
				             push_heap(_data.begin(), _data.end(), _comp);
		      }


	  //�Ӷ������Ƴ�Ԫ�أ�˳��Ϊ�������ѽṹ(pop_heap)���Ƴ�Ԫ��(pop_back)
	 T remove()
		     {
			          if(NULL == _comp)
			                    pop_heap(_data.begin(), _data.end());    //�˴�bug
			          else
				                pop_heap(_data.begin(), _data.end(), _comp);
			          T removed = _data.back();
			          _data.pop_back();
			          return removed;
		      }

	  bool isEmpty() const
	          {
			         return _data.empty();
	          }

	  void clear()
	         {
			         _data.clear();
             }


	  T top()
	  {
		  if(NULL == _comp)
			  pop_heap(_data.begin(), _data.end());
		  else
			  pop_heap(_data.begin(), _data.end(), _comp);
		  T topVal = _data.back();
		 return  topVal;
	  }
	  
		  

	int size()
	{
		return _data.size();
	}


	~MyHeap(void)
	{

	}
	

};

