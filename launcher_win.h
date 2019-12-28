//
// Created by Glavo on 2019.12.27.
//

#ifndef LAUNCHER_LAUNCHER_WIN_H
#define LAUNCHER_LAUNCHER_WIN_H

#include <windows.h>
#include <launcher_config.h>
#include "launcher.h"

typedef struct _CMD {
    int nOptions;
    JavaVMOption *options;
    int nArguments;
    LPWSTR *arguments;
} CMD;

extern CMD cmd;

extern void parseCMD();

extern void destructCMD();


extern JavaVM *vm;
extern JNIEnv *env;

static inline jclass findClass(const char *name) {
    return (*env)->FindClass(env, name);
}

extern HMODULE loadJVM();

extern jint createJVM(HMODULE);

extern jobjectArray javaArguments();

#endif //LAUNCHER_LAUNCHER_WIN_H
