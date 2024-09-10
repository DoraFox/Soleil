#ifndef BASEDEFINE_H
#define BASEDEFINE_H

#include <QDebug>

#define DEBUGPREFIX qDebug() << QString("Soleil-(%1:%2)").arg(__FUNCTION__).arg(__LINE__)
#define DEBUGPREFIX_ERROR qDebug() << QString("Soleil-ERROR:(%1:%2)").arg(__FUNCTION__).arg(__LINE__)

/* This will let macros expand before concating them */
#define STR(s) #s

#define _STR(s) STR(s)

/* This will let macros expand before concating them */
#define CONTACT(a,b) a ## b

#define _CONTACT(a,b) CONTACT(a,b)


/* This counts the number of args */
#define NARGS_SEQ(_1,_2,_3,_4,_5,N,...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 5, 4, 3, 2, 1)


/* This will return the last argument */
#define CONTACT_STR(...) _CONTACT(CONTACT_STR_, NARGS(__VA_ARGS__))(__VA_ARGS__)

#define CONTACT_STR_2(a,b) _STR(a) _STR(b)
#define CONTACT_STR_3(a,b,c) _STR(a) _STR(b) _STR(c)

#define CONTACT_PATH(a,b) _STR(a) "/" _STR(b)

/* This will let macros expand before concating them */
#define  ANONYMOUS(type, var, line)  type  var##line
#define  _ANONYMOUS(type, line)  ANONYMOUS(type, _anonymous, line)
#define  __ANONYMOUS(type)  _ANONYMOUS(type, __LINE__)

#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)

#define my_print2(fmt,...) printf(fmt, ##__VA_ARGS__)


#define DECLARE_PROPERTY(aType, aProperty)\
    private: aType m_ ## aProperty;\
    public: aType aProperty() const { return m_ ## aProperty; }\
    void set ## _ ## aProperty(aType _ ## aProperty)\
    {\
        m_ ## aProperty = _ ## aProperty;\
    }

#define DECLARE_PROPERTY_PROTECTED(aType, aProperty)\
    protected: aType m_ ## aProperty;\
    public: aType aProperty() const { return m_ ## aProperty; }\
    void set ## aProperty(aType _ ## aProperty)\
    {\
        m_ ## aProperty = _ ## aProperty;\
    }


#define DECLARE_PROPERTY_SIGNAL(aType, aProperty)\
    void aProperty ## Changed(aType _ ## aProperty);

#define DECLARE_QML_PROPERTY(aType, aProperty)\
    private: aType m_ ## aProperty;\
    public: aType aProperty() const { return m_ ## aProperty; }\
    void set ## _ ## aProperty(aType _ ## aProperty)\
    {\
        if(m_ ## aProperty != _ ## aProperty)\
        {\
            m_ ## aProperty = _ ## aProperty;\
            emit aProperty ## Changed(_ ## aProperty);\
        }\
    }

#define DECLARE_PROPERTY_READ_ONLY(aType, aProperty)\
    private: aType m_ ## aProperty;\
    public: aType aProperty() const { return m_ ## aProperty; }

#define RegisterQmlType(aType, versionMaj, versionMin)\
    qmlRegisterType<aType>(#aType, versionMaj, versionMin, #aType)


/* This will return the last argument */
#define RegisterQmlComponentType(...) _CONTACT(RegisterQmlComponentType_, NARGS(__VA_ARGS__))(__VA_ARGS__)

#define RegisterQmlComponentType_3(aType, versionMaj, versionMin)\
    qmlRegisterType<aType>("cplusplus.Component", versionMaj, versionMin, #aType)

#define RegisterQmlComponentType_4(aType, uri, versionMaj, versionMin)\
    qmlRegisterType<aType>(#uri, versionMaj, versionMin, #aType)


#define TestMacro(str) "cplusplus" _STR(str)

#endif // BASEDEFINE_H
