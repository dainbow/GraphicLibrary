#pragma once

#include <SFML/Graphics.hpp>
#include <list>

#include "Vector.hpp"

template <class TArguments>
class BaseHandler {
    public:
        virtual void Call(const TArguments& args) = 0;

        virtual ~BaseHandler() {}
        BaseHandler() {}
    private:
};

template <class T, class TArguments>
class FuncCaller : public BaseHandler<TArguments> {
    public:
        FuncCaller(T* object, void (*callFunc)(T* obj, const TArguments& args)) :
        object_(object),
        callFunc_(callFunc)
        {}

        FuncCaller(const FuncCaller<T, TArguments>& toCpy) {
            object_   = toCpy.object_;
            callFunc_ = toCpy.callFunc_;
        }

        FuncCaller<T, TArguments>& operator=(const FuncCaller<T, TArguments>& toCpy) {
            object_   = toCpy.object_;
            callFunc_ = toCpy.callFunc_;
        }

        virtual void Call(const TArguments& args) override {
            callFunc_(object_, args);
        }

    private:
        T* object_;
        void (*callFunc_)(T* obj, const TArguments& args);
};

template <class T, class TArguments>
class MethodCaller : public BaseHandler<TArguments> {
        using TMethod = void (T::*)(const TArguments& args);

    public:
        MethodCaller(T* object, TMethod callMethod) :
        object_(object),
        callMethod_(callMethod)
        {}

        MethodCaller(const MethodCaller<T, TArguments>& toCpy) {
            object_   = toCpy.object_;
            callMethod_ = toCpy.callMethod_;
        }

        MethodCaller<T, TArguments>& operator=(const MethodCaller<T, TArguments>& toCpy) {
            object_     = toCpy.object_;
            callMethod_ = toCpy.callMethod_;
        }

        virtual void Call(const TArguments& args) override {
            (object_->*callMethod_)(args);
        }

    private:
        T* object_;
        TMethod callMethod_;
};

template <class TArguments>
class Event {
    private:
        std::list<BaseHandler<TArguments>*> handlers;
    
    public:
        Event() :
        handlers() 
        {}    

        ~Event() {
            for (auto& curHandler : handlers) {
                delete curHandler;
            }
        }

        void operator() (const TArguments& args) {
            for (auto& curHandler : handlers) {
                curHandler->Call(args);
            }
        }

        void operator+=(BaseHandler<TArguments>* handler) {
            handlers.push_back(handler);
        }
};  
