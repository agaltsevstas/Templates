#ifndef Matching_h
#define Matching_h

/*
 Matching - паттерн сопоставляющий типы, который можно рассматривать как обобщение оператора switch-case
 */

namespace matching
{
    namespace C11
    {
        namespace details
        {
            template <class...>
            struct Match;

            template <class Head, class... Tail>
            struct Match<Head, Tail...> : Head, Match<Tail...>
            {
                template <class Head2, class... Tail2>
                Match(Head2&& head, Tail2&& ... tail) : Head(std::forward<Head2>(head)), Match<Tail...>(std::forward<Tail2>(tail)...) {}
                using Head::operator();
                using Match<Tail...>::operator();
            };

            /// Закрытие рекурсии специализацией
            template <class T>
            struct Match<T> : T
            {
                template <class R>
                Match(R&& r) noexcept : T(std::forward<R>(r)) {}
                using T::operator();
            };
        }

        template <class... TArgs>
        auto Match(TArgs&&... args)
        {
            return details::Match<typename std::decay<TArgs>::type...>{std::forward<TArgs>(args)...};
        }
    }

    namespace C17
    {
        namespace first_implementation
        {
            namespace details
            {
                template <typename... TArgs>
                struct Match : TArgs...
                {
                    using TArgs::operator()...;
                    Match(TArgs&&... args) : TArgs(std::forward<TArgs>(args))... {}
                };
            }

            template <class... TArgs>
            auto Match(TArgs&&... args)
            {
                return details::Match<TArgs...>(std::forward<TArgs>(args)...);
            }
        }
    
        namespace second_implementation
        {
            template <typename... TArgs>
            struct Match : TArgs...
            {
                using TArgs::operator()...;
                Match(TArgs&&... args) : TArgs(std::forward<TArgs>(args))... {}
            };
        }
    
        namespace third_implementation
        {
            template <typename... TArgs>
            struct Match : TArgs...
            {
                using TArgs::operator()...;
            };
        
            /// Deduction hints: нет лишнего создания объектов и лишней передачив конструктор, создается только 1 объект и вызывается 1 конструктор
            template <typename ...TArgs> Match(TArgs...) -> Match<TArgs...>;
        }
    }
}

#endif /* Matching_h */
