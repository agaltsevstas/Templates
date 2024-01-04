#ifndef CRTP_h
#define CRTP_h

/*
 CRTP (curiously recurring template pattern) - рекурсивный шаблон: класс Derived наследуется от шаблона класса Base, использующего Derived как шаблонный параметр. Вызов Base::interface вызывает Derived::interface. Используется статический полиморфизм вместо динамического полиморфизма (без виртуальных функций)
 Плюсы:
 - делается во время компиляции (compile), а не во время исполнения (runtime)
 - нет накладных расходов на создание таблиц виртуальных функций
 - время, затрачиваемое на выбор метода
 Минусы:
 - У всех классов-наследников должны определены методы, которые которые вызываются в базовом классе
*/

namespace CRTP
{
    template <typename T>
    class Base
    {
    public:
        void interface1() // 1 Cпособ
        {
            static_cast<T*>(this)->implementation1();
        };
        
        static void interface2()
        {
            T::implementation2(); // 2 Способ: static
        };
    };

    class Derived : public Base<Derived>
    {
    public:
        void implementation1()
        {
            std::cout << "interface1 Derived" << std::endl;
        }
        
        static void implementation2()
        {
            std::cout << "interface2 Derived" << std::endl;
        }
    };
}

#endif /* CRTP_h */
