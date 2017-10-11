#ifndef _COMMUNS_H_
#define _COMMUNS_H_

#include <pgmspace.h>

const char DateISORegex[] PROGMEM = "(%d+)%-(%d+)%-(%d+)";
const char TimeISORegex[] PROGMEM = "(%d+):(%d+):(%d+)";

template<class T, void(T::*PTR)(), size_t I>
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
template<class T, void(T::*PTR)(), size_t I>
T* bind_member<T, PTR, I>::ptr = NULL;

#endif