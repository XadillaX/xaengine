/**--------------------------------
 * 单件模式(Singleton)
 *
 * Code by XadillaX
 * http://www.xcoder.in
 * Created at 2010-11-17 1:33
 */
#ifndef SINGLETON_H
#define SINGLETON_H
#pragma once

template<class T>
class Singleton
{
public:
    static T& Instance();

protected:
    Singleton(){}
    virtual ~Singleton(){}

/**
 * 防止拷贝复制
 */
private:
    Singleton(const Singleton &);
    Singleton & operator = (const Singleton &);
};

template<class T>
T& Singleton<T>::Instance()
{
    static T instance;
    return instance;
}

#endif
