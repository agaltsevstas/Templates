#ifndef Forward_h
#define Forward_h

/*
 std::forward - идеальной передача (perfect forwarding).
 Свойства:
 - std::forward РАБОТАЕТ с T&& с НЕконстантными ссылками
 - std::forward НЕ работает с const T& - для всех типов ссылок (rvalue, const rvalue, lvalue, const lvalue), т.к. rvalue(T&&) -> lvalue (T&) на основе сжатии ссылок (T&(A&&) = A& // && отбрасывается).
 - std::forward НЕ будет работает c const&& для lvalue

 std::forward используется ТОЛЬКО в шаблонах:
 template<typename T> T&& forward(T&& param)
 {
     if (is_lvalue_reference<T>::value)
         return param;
     else
         return move(param);
 }
 Тип typename T имеет 2 перегрузки:
 1. lvalue-ссылка (T = const A& или T = A&)
 2. rvalue-ссылка (T = A&&)
 3. Бессылочный тип (T = A*) - указатель

 Идеальная передача позволяет создавать функции-обертки, где lvalue-выражения копируются (просто возвращается lvalue), а rvalue-выражения перемещаются (std::move).
 Использование:
 class Example
 {
    std::string field;
    template <class T>
    Example(T&& x) : field(std::forward<T>(x)) {}
 };
 
 reference collapse — сжатие ссылок, которое используется только в шаблонах и определяет поведение (отбрасывает не нужные &) при появлении ссылки на ссылку. Меньшее число (&) - выигрывает у большего числа (&&).
 Пример:
 T инстанцирован A&&, то T&& (A&& &&) = A&& // && отбрасывается
 Правила:
 T&(A&)  = A& // & отбрасывается
 T&&(A&) = A& // && отбрасывается
 T&(A&&) = A& // && отбрасывается
 T&&(A&&) = A&& // && отбрасывается, но тип xvalue->lvalue
 Универсальной ссылка - аргумент типа T&&, где T — шаблонный тип, может принимать оба типа ссылок (A& и A&&).
 Пример:
 template<typename T> function(T&& arg) // arg - lvalue всегда, т.к. T&& arg - xvalue, поэтому будет вызываться конструктор копирования
 Отличие std::forward от std::move: std::move - приводит lvalue к rvalue, std::forward - lvalue просто возвращает lvalue, а rvalue – возвращает std::move(rvalue).
 */

namespace lvalue_rvalue
{
    namespace priority
    {
        void function(int&& value)
        {
            std::cout << "&&" << std::endl;
        }
        
        void function(const int&& value)
        {
            std::cout << "const &&" << std::endl;
        }
        
        void function(int& value)
        {
            std::cout << "&" << std::endl;
        }

        void function(const int& value)
        {
            std::cout << "const &" << std::endl;
        }
    }
}

namespace FORWARD
{
    template <class T>
    constexpr T&& forward(std::remove_reference_t<T>& args) noexcept
    {
        return static_cast<T&&>(args);
    }

    class A
    {
    public:
        A() { std::cout << "A construtor" << std::endl; } // Обычный конструктор
        A(A&&) { std::cout << "A&& construtor" << std::endl; } // Конструктор перемещения
        A(A&) { std::cout << "A& construtor" << std::endl; } // Конструктор копирования
        A(const A&) { std::cout << "const A& construtor" << std::endl; } // Конструктор копирования перекроет
    };

    template<typename T>
    void Function(T&& arg)
    {
        lvalue_rvalue::priority::function(arg);
        lvalue_rvalue::priority::function(std::move(arg));
        lvalue_rvalue::priority::function(FORWARD::forward<T>(arg));
    }

    template<class T, typename Args>
    std::shared_ptr<T> Make_Shared(Args&& args)
    {
        return std::make_shared<T>(args);
    }

    template<class T, typename Args>
    std::shared_ptr<T> Make_Shared_Forward(Args&& args)
    {
        return std::make_shared<T>(FORWARD::forward<T>(args));
    }
}

#endif /* Forward_h */
