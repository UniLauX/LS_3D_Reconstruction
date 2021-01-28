#pragma once
#include <vector>
#include <algorithm>
using namespace std;
template<class T>



class MyHeap
{
private:
   //声明了一个函数指针用于指向比较堆里面元素大小的函数
	typedef    bool (*COMP_FUNC)(const T&, const T&);
	COMP_FUNC _comp;
	vector<T> _data;

public:
	
	//如果不指定这个函数指针变量的值，那么会自动调用类实现的 operator < 函数
	MyHeap(COMP_FUNC compare_function = NULL) :_comp(compare_function)
		     {
			         if(NULL == _comp)
	                   make_heap(_data.begin(), _data.end());
			          else
		               make_heap(_data.begin(), _data.end(), _comp);
		      }


	  //往堆里面添加元素，顺序为先添加(push_back)再重整堆结构(push_heap)
	void add(const T& item)
		     {
			         _data.push_back(item);
			         if(NULL == _comp)
				             push_heap(_data.begin(), _data.end());
			         else
				             push_heap(_data.begin(), _data.end(), _comp);
		      }


	  //从堆里面移除元素，顺序为先重整堆结构(pop_heap)再移除元素(pop_back)
	 T remove()
		     {
			          if(NULL == _comp)
			                    pop_heap(_data.begin(), _data.end());    //此处bug
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

