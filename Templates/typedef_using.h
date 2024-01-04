#ifndef typedef_using_h
#define typedef_using_h

class Class{};

/*
 typedef не позволяет сделать alias (псевдоним) для template.
 Лучше полностью отказаться от typedef и перейти на using
 */

template<class T>
using class_t = T;

//template<class T>
//typedef T T_t; // Ошибка: typedef caanot be a template

typedef Class Class_;
using class_ = Class;

#endif /* typedef_using_h */
