#ifndef Matching_h
#define Matching_h

/*
 Matching - паттерн сопоставляющий типы, который можно рассматривать как обобщение оператора switch-case
 */

namespace matching
{
    namespace details
    {
        template <class...>
        struct Match;

        template <class Head, class... Tail>
        struct Match<Head, Tail...> : Match<Tail...>, Head
        {
            template <class Head2, class... Tail2>
            Match(Head2&& head, Tail2&& ... tail) : Match<Tail...>(std::forward<Tail2>(tail)...), Head(std::forward<Head2>(head)) {}
            using Head::operator();
            using Match<Tail...>::operator();
        };

        template <class T>
        struct Match<T> : T 
        {
            template <class R>
            Match(R&& r) : T(std::forward<R>(r)) {}
            using T::operator();
        };
    }

    template <class... TArgs>
    auto Match(TArgs&&... args)
    {
        return details::Match<typename std::decay<TArgs>::type...>{std::forward<TArgs>(args)...};
    }
}

#endif /* Matching_h */
