#pragma once
#include <functional>
#include <memory>

using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : mCallback(std::move(callback))
	{

	}

	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		mCallback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	void Execute()
	{
		mCallback();
	}

private:
	CallbackType mCallback;
};