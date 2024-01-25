#ifndef Callback_h
#define Callback_h

#include <functional>

namespace callback
{
    class ICallback
    {
    public:
        virtual void operator()() = 0;
        virtual ~ICallback() = default;
    };

    template<class TClass, class TCallBack, typename TArgs>
    class AbstractCallback : public ICallback
    {
    public:
        AbstractCallback(TClass& iClass, TCallBack& iCallback, TArgs&& iArgs) :
        _class(iClass),
        _callback(iCallback),
        _args(iArgs)
        {
            
        }
        virtual void operator()() override
        {
            (_class.*(_callback))(std::forward<TArgs>(_args));
        }
    protected:
        TClass _class;
        TCallBack _callback;
        TArgs _args;
    };

    template <class TClass, class TCallBack, class... TArgs>
    class Callback : public ICallback
    {
        using _CallBack = std::function<void()>;
    public:
        Callback(const TClass& iClass, const TCallBack& iCallback, TArgs&&... iArgs)
        {
            _callback = std::bind(iCallback, iClass, std::forward<TArgs>(iArgs)...);
        }

        void operator()() override
        {
            _callback();
        }

    private:
        _CallBack _callback;
    };

    class Example
    {
    public:
        void Method()
        {
            std:: cout << "Method1" << std::endl;
        }
        
        void Method1(bool result)
        {
            std:: cout << "Method1, result: " << result << std::endl;
        }
        
        void Method2(bool result1, bool result2)
        {
            std:: cout << "Method1, result1: " << result1 << ", result2: " << result2 << std::endl;
        }
        
        void Method3(std::string &str)
        {
            std:: cout << "Method3, str: " << str << std::endl;
        }
    };

    template <class TCallBack>
    class Permission
    {
    public:
        Permission(const Example& iClass, TCallBack&& iCallback)
        {
            if constexpr (std::is_same_v<void(Example::*)(), TCallBack>)
            {
                _callback.reset(new Callback(iClass, iCallback));
            }
            else if constexpr (std::is_same_v<void(Example::*)(bool), TCallBack>)
            {
                bool flag = true;
                _callback.reset(new Callback(iClass, iCallback, std::ref(flag)));
            }
            else if constexpr (std::is_same_v<void(Example::*)(bool, bool), TCallBack>)
            {
                bool flag = true;
                _callback.reset(new Callback(iClass, iCallback, std::ref(flag), std::ref(flag)));
            }
        }

        void Run()
        {
            if (_callback)
            {
                (*_callback)();
            }
        }

    private:
        std::unique_ptr<ICallback> _callback;
    };
}

#endif /* Callback_h */
