#ifndef BOE_NEW_ORDER_CROSS_FIELD_MACRO_H
#define BOE_NEW_ORDER_CROSS_FIELD_MACRO_H

#define MAKE_FIELD_FUNCTION(name, function, type)                           \
    inline unsigned char * get##name(unsigned char * p, const type & value) \
    {                                                                       \
        return function(p, value);                                          \
    }

#define MAKE_FIELD_FUNCTION_WITH_SIZE(name, function, type, size)           \
    inline unsigned char * get##name(unsigned char * p, const type & value) \
    {                                                                       \
        return function(p, value, size);                                    \
    }

#define MAKE_FIELD_FUNCTION_STATIC(name, function, value) \
    inline unsigned char * get##name(unsigned char * p)   \
    {                                                     \
        return function(p, value);                        \
    }

#define FUNCTION_STATIC(name) p = get##name(p);

#define FUNCTION(name, value) p = get##name(p, value);

#define EXPAND_FUNC(...) __VA_ARGS__

#endif // BOE_NEW_ORDER_CROSS_FIELD_MACRO_H
