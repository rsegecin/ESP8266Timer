#ifndef _SIMPLE_BINDERS_H_
#define _SIMPLE_BINDERS_H_

template<class T, void(T::*PTR)()>
struct bind_member
{
	typedef void(*fn_type)();
	explicit bind_member(T* _ptr)
	{
		ptr = _ptr;
	}
	static void func(void)
	{
		(ptr->*PTR)();
	}
	operator fn_type()
	{
		return &func;
	}
private:
	static T*  ptr;
};
template<class T, void(T::*PTR)()>
T* bind_member<T, PTR>::ptr = NULL;


template<class T, bool(T::*PTR)()>
struct bind_bool_member
{
	typedef bool(*fn_type)();
	explicit bind_bool_member(T* _ptr)
	{
		ptr = _ptr;
	}
	static bool func(void)
	{
		(ptr->*PTR)();
	}
	operator fn_type()
	{
		return &func;
	}
private:
	static T*  ptr;
};
template<class T, bool(T::*PTR)()>
T* bind_bool_member<T, PTR>::ptr = NULL;

#endif