//
// Created by Glavo on 2019.12.24.
//

#ifndef LAUNCHER_LAUNCHER_H
#define LAUNCHER_LAUNCHER_H

#include <jni.h>

typedef jint (JNICALL *CreateJavaVM_t)(JavaVM **pvm, void **penv, void *args);

#endif //LAUNCHER_LAUNCHER_H
