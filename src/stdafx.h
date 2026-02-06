#pragma once

// DLL Export/Import macro for Windows
#ifdef _MSC_VER
    #ifdef FLUENTUI_BUILD_STATIC_LIB
        #define FLUENTUI_EXPORT
    #else
        #if defined(fluentuiplugin_EXPORTS) || defined(fluentui_EXPORTS)
            #define FLUENTUI_EXPORT __declspec(dllexport)
        #else
            #define FLUENTUI_EXPORT __declspec(dllimport)
        #endif
    #endif
#else
    #define FLUENTUI_EXPORT
#endif

#define Q_PROPERTY_AUTO_P(TYPE, M)                                                                 \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed)                                               \
public:                                                                                            \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(TYPE in_##M) {                                                                          \
        _##M = in_##M;                                                                             \
        Q_EMIT M##Changed();                                                                       \
    }                                                                                              \
    TYPE M() const {                                                                               \
        return _##M;                                                                               \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    TYPE _##M;

#define Q_PROPERTY_AUTO(TYPE, M)                                                                   \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed)                                               \
public:                                                                                            \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(const TYPE &in_##M) {                                                                   \
        if (_##M != in_##M) {                                                                      \
            _##M = in_##M;                                                                         \
            Q_EMIT M##Changed();                                                                   \
        }                                                                                          \
    }                                                                                              \
    TYPE M() const {                                                                               \
        return _##M;                                                                               \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    TYPE _##M;


#define Q_PROPERTY_READONLY_AUTO(TYPE, M)                                                          \
    Q_PROPERTY(TYPE M READ M NOTIFY M##Changed FINAL)                                              \
public:                                                                                            \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(const TYPE &in_##M) {                                                                   \
        if (_##M != in_##M) {                                                                      \
            _##M = in_##M;                                                                         \
            Q_EMIT M##Changed();                                                                   \
        }                                                                                          \
    }                                                                                              \
    TYPE M() const {                                                                               \
        return _##M;                                                                               \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    TYPE _##M;
