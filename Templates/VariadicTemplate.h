#ifndef Arguments_h
#define Arguments_h

/*
 Variadic Template - шаблон с заранее неизвестным числом аргументов
 Оператор многоточия или элипсис  (…), когда он стоит слева от имени параметра функции, он объявляет набор параметров (template<typename... Args>), когда стоит справа от шаблона или аргумента вызова функции, он распаковывает параметры в отдельные аргументы (Args&... args).
 Например, std::tuple - может принять любое число входных параметров
 До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка)
 */

namespace variadic_template
{
    enum class MeasureProperty
    {
        One = 0,
        Two = 1,
        Three = 2
    };

    /// Stub-функция (заглушка) для рекурсии
    inline bool CheckProperty(MeasureProperty iProperty, MeasureProperty Property)
    {
        return iProperty == Property;
    }

    template<typename... TArgs>
    inline bool CheckProperty(MeasureProperty iProperty, MeasureProperty Property, TArgs&&... types)
    {
        return CheckProperty(iProperty, Property) || CheckProperty(iProperty, types...);
    }
    
    /// Stub-функция (заглушка) для рекурсии
    void print()
    {
        std::cout << "end print" ;
    }

    template<typename T, typename... TArgs>
    inline void print(const T& first, TArgs&&... args)
    {
        std::cout << first << std::endl;
        print(std::forward<TArgs>(args)...);
    }
}

#endif /* Arguments_h */
