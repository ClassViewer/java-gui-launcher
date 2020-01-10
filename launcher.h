//
// Created by Glavo on 2019.12.24.
//

#ifndef LAUNCHER_LAUNCHER_H
#define LAUNCHER_LAUNCHER_H

#include <jni.h>
#include <launcher_config.h>


#ifdef WIN32

#include <windows.h>

typedef HMODULE LIBRARY_HANDLE;
#else // WIN32
typedef void *LIBRARY_HANDLE;
#endif

typedef jint (JNICALL *CreateJavaVM_t)(JavaVM **pvm, void **penv, void *args);

extern JavaVM *vm;
extern JNIEnv *env;

typedef struct _CMD {
    int nOptions;
    JavaVMOption *options;
    int nArguments;
#ifdef WIN32
    LPWSTR *arguments;
#else 
    const char **arguments;
#endif
} CMD;

extern CMD cmd;

#ifdef WIN32
extern void parseCMD(void);
#else  // WIN32
extern void parseCMD(int argc, char *argv[]);
#endif

extern void destructCMD(void);

static inline jclass findClass(const char *name) {
    return (*env)->FindClass(env, name);
}

extern LIBRARY_HANDLE libjvm;

extern void loadJVM(void);

extern jint createJVM(void);

extern jobjectArray javaArguments(void);

#endif //LAUNCHER_LAUNCHER_H
