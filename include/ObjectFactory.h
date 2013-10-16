/**--------------------------------
 * ���󹤳�ģʽ(Object Factory)
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
    typedef T* (*tCreator)();                               ///< �ض���������ɺ���ָ��
    typedef std::map<std::string, tCreator> tCreatorMap;    ///< �������ɺ���ָ��map

    /**
     * @brief ע���¡��������䡱
     * �����ɶ���ĺ���������󹤳�
     *
     * @param *name ������
     * @param procedure ������������ĺ���
     * @return �Ƿ�ɹ�ע��
     */
    bool Register(char *type, tCreator procedure);

    /**
     * @brief �ҵ����������䡱
     * ���ݴ��������������Ӧ���¶�������ɺ���
     *
     * @param &type ����
     * @return ��Ӧ���¶�������ɺ���
     */
    T* Create(const std::string &type);

private:
    /** ���������䡱ӳ�� */
    tCreatorMap _map;
};

template<class T>
bool ObjectFactory<T>::Register(char *type, tCreator procedure)
{
    string tmp(type);
    /** ���º�������map�� */
    _map[tmp] = procedure;
    return _map[tmp] != 0;
}

template<class T>
T* ObjectFactory<T>::Create(const std::string &type)
{
    /** ��ӳ�����ҵ���Ӧ���������䡱 */
    tCreatorMap::iterator iter = _map.find(type);

    /** ��⡰���䡱�Ƿ���� */
    if(iter != _map.end())
    {
        /** �÷���ֵΪ��Ӧ�ġ��������䡱 */
        tCreator r = iter->second;

        /** ���ء��������䡱 */
        return r();
    }

    return 0;
}

#endif