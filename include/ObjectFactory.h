/**--------------------------------
 * 对象工厂模式(Object Factory)
 *
 * Code by XadillaX
 * http://www.xcoder.in
 * Created at 2010-11-17 1:33
 */
#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#pragma once
#include <map>
#include <string>
#include "Singleton.h"

template<class T>
class ObjectFactory : public Singleton<ObjectFactory<T>>
{
public:
    typedef T* (*tCreator)();                               ///< 重定义对象生成函数指针
    typedef std::map<std::string, tCreator> tCreatorMap;    ///< 对象生成函数指针map

    /**
     * @brief 注册新“生产车间”
     * 将生成对象的函数加入对象工厂
     *
     * @param *name 类名称
     * @param procedure “生产”对象的函数
     * @return 是否成功注册
     */
    bool Register(char *type, tCreator procedure);

    /**
     * @brief 找到“生产车间”
     * 根据传入的类名返回相应的新对象的生成函数
     *
     * @param &type 类名
     * @return 相应的新对象的生成函数
     */
    T* Create(const std::string &type);

private:
    /** “生产车间”映射 */
    tCreatorMap _map;
};

template<class T>
bool ObjectFactory<T>::Register(char *type, tCreator procedure)
{
    string tmp(type);
    /** 将新函数加入map中 */
    _map[tmp] = procedure;
    return _map[tmp] != 0;
}

template<class T>
T* ObjectFactory<T>::Create(const std::string &type)
{
    /** 在映射中找到相应“生产车间” */
    tCreatorMap::iterator iter = _map.find(type);

    /** 检测“车间”是否存在 */
    if(iter != _map.end())
    {
        /** 让返回值为相应的“生产车间” */
        tCreator r = iter->second;

        /** 返回“生产车间” */
        return r();
    }

    return 0;
}

#endif