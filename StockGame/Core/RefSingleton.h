#pragma once

template<class T>
class RefSingleton
{
public:
	static T& GetInstance()
	{
		return mInstance;
	}

protected:
	static T mInstance;
};

template<class T>
T RefSingleton<T>::mInstance;