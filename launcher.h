//
// Created by Glavo on 2019.12.24.
//

#ifndef LAUNCHER_LAUNCHER_H
#define LAUNCHER_LAUNCHER_H

#include <jni.h>


#ifdef WIN32

#include <windows.h>

typedef HMODULE LIBRARY_HANDLE;
#else // WIN32
typedef void *LIBRARY_HANDLE;
#endif

typedef jint (JNICALL *CreateJavaVM_t)(JavaVM **pvm, void **penv, void *args);

extern JavaVM *vm;
extern JNIEnv *env;

extern void parseCMD();

extern void destructCMD();

static inline jclass findClass(const char *name) {
    return (*env)->FindClass(env, name);
}

extern LIBRARY_HANDLE loadJVM();

extern jint createJVM(LIBRARY_HANDLE);

extern jobjectArray javaArguments();

#endif //LAUNCHER_LAUNCHER_H
