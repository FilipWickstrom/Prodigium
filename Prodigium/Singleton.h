#pragma once
template <typename T>
class Singleton
{
private:
	Singleton();

public:
	virtual ~Singleton();
	static T& getInstance();

	Singleton(const Singleton& other) = delete;
	Singleton& operator=(const Singleton& other) = delete;
};

template<typename T>
inline Singleton<T>::~Singleton()
{
}

template<typename T>
inline T& Singleton<T>::getInstance()
{
	static T instance;

	return instance;
}
