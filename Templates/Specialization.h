#ifndef Specialization_h
#define Specialization_h

#include <string>

/*
 Компилятор в приоритете выбирает более специлизированный шаблон
 Специализации шаблонов по приоритетам:
 1. Полная специализация (explicit (или full) specialization) - напоминает наследование или перегрузку - template<> class vector<bool>, тогда компилятор не будет дописывать класс под конкретный тип данных, а вызовет уже объявленную специализацию, таким образом мы может запретить для какого-то типа определенные действия или дополнить, в отличие от остальных типов
 2. Частичная специализация (partial specialization) - только для классов template<Class A> class vector<bool, A>.
 Для методов нельзя, но это можно обойти сделать в частичном специализированном классе static метод.
 3. Основной шаблон (primary template) - vector<T>
 */

namespace specialization
{
    // Основной шаблон (primary template)
    template <typename TName, typename TAge>
    class Person
    {
    public:
        Person(const TName& name, const TAge& age) :
        _name(name), _age(age)
        {
            
        }
        
    private:
        TName _name;
        TAge _age;
    };
     
    // Частичная специализация (partial specialization)
    template <typename TName>
    class Person<TName, unsigned>
    {
    public:
        Person(const TName& name, unsigned age) :
        _name(name), _age(age)
        {
            
        }
        
    private:
        TName _name;
        unsigned _age;
    };

    // Полная специализация (explicit (или full) specialization)
    template <>
    class Person<std::string, unsigned>
    {
    public:
        Person(const std::string& name, unsigned age) :
        _name(name), _age(age)
        {
            
        }
        
    private:
        std::string _name;
        unsigned _age;
    };
}

#endif /* Specialization_h */
