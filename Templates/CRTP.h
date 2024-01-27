#ifndef CRTP_h
#define CRTP_h

/*
 CRTP (curiously recurring template pattern) - рекурсивный шаблон: класс Derived наследуется от шаблонного класса Base, использующего Derived как шаблонный параметр T. Через метод базового класса Base можно вызвать метод наследуемого класса Derived, используя статический полиморфизм вместо динамического полиморфизма (без таблицы виртуальных функций): static_cast<T*>(this)->method()
 Плюсы:
 - делается во время компиляции (compile), а не во время исполнения (runtime)
 - нет накладных расходов на создание таблиц виртуальных функций
 - время, затрачиваемое на выбор метода в таблице виртуальных функций
 Минусы:
 - у всех классов-наследников должны определены методы, которые вызываются в базовом классе
*/

namespace CRTP
{
    template <typename T>
    class Base
    {
    public:
        void Interface1() // 1 Cпособ
        {
            static_cast<T*>(this)->Implementation1();
        };
        
        static void Interface2()
        {
            T::Implementation2(); // 2 Способ: static
        };
    };

    class Derived : public Base<Derived>
    {
    public:
        void Implementation1()
        {
            std::cout << "interface1 Derived" << std::endl;
        }
        
        static void Implementation2()
        {
            std::cout << "interface2 Derived" << std::endl;
        }
    };
}

#endif /* CRTP_h */
