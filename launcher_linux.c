//
// Created by Glavo on 2020.01.10.
//

#define _POSIX_C_SOURCE 1

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dlfcn.h>

#include "whereami/whereami.h"
#include "launcher.h"

const char APPLICATION_PREDEF_VM_OPTIONS[] = APPLICATION_PREDEF_OPTIONS;

void parseCMD(int argc, char *argv[]) {
    int preOptionsSize = 0;
    for (int i = 0; i < sizeof(APPLICATION_PREDEF_VM_OPTIONS) / sizeof(char); ++i) {
        if (APPLICATION_PREDEF_VM_OPTIONS[i] == '\0') {
            ++preOptionsSize;
        }
    }

    int nOptions = 0;
    JavaVMOption *options = malloc((argc - 1 + preOptionsSize) * sizeof(JavaVMOption));
    int nArguments = 0;
    const char **arguments = malloc((argc - 1) * sizeof(char *));

    const char *p = APPLICATION_PREDEF_VM_OPTIONS;
    while (p < APPLICATION_PREDEF_VM_OPTIONS + sizeof(APPLICATION_PREDEF_VM_OPTIONS)) {
        size_t len = strlen(p);
        if (len == 0) {
            ++p;
            continue;
        }
        options[nOptions].optionString = (char *) p;
        options[nOptions].extraInfo = NULL;
        ++nOptions;
        p += len + 1;
    }

    for (int i = 1; i < argc; ++i) {
        char *str = argv[i];
        int strLen = (int) strlen(str);
        if (strLen > 2 && str[0] == L'-' && str[1] == L'J') {
            str += 2;
            options[nOptions].optionString = str;
            options[nOptions].extraInfo = NULL;
            ++nOptions;
        } else {
            arguments[nArguments++] = str;
        }
    }

    cmd.nOptions = nOptions;
    if (nOptions == 0) {
        free(options);
        cmd.options = NULL;
    } else {
        cmd.options = options;
    }

    cmd.nArguments = nArguments;
    if (nArguments == 0) {
        free(arguments);
        cmd.arguments = NULL;
    } else {
        cmd.arguments = arguments;
    }
}

void destructCMD(void) {

}

void loadJVM() {
    int pathLen;
    char jrePath[PATH_MAX + 1] = {0};
    if (APPLICATION_JRE_PATH[0] == '/') {
        strcpy(jrePath, APPLICATION_JRE_PATH);
        pathLen = strlen(APPLICATION_JRE_PATH);
    } else if (wai_getExecutablePath(jrePath, PATH_MAX + 1, &pathLen) != -1) {
        jrePath[pathLen] = '/';
        strcpy(jrePath + pathLen + 1, APPLICATION_JRE_PATH);
        pathLen = (int) strlen(jrePath);
    } else {
        fprintf(stderr, "Failed to get file path\n");
        return;
    }

    if (jrePath[pathLen - 1] == '/') {
        --pathLen;
    }
    strcpy(jrePath + pathLen, "/lib/" APPLICATION_JVM_TYPE "/libjvm.so");
    void *jvm = dlopen(jrePath, RTLD_NOW);
    const char *err = dlerror();
    if (err) {
        fprintf(stderr, "Open libjvm.so failed: %s\n", err);
        return;
    }
    libjvm = jvm;
}

jint createJVM() {
    CreateJavaVM_t createJavaVM = (CreateJavaVM_t) dlsym(libjvm, "JNI_CreateJavaVM");

    if (createJavaVM == NULL) {
        return JNI_ERR;
    }

    // options
    JavaVMInitArgs initArgs;
    initArgs.version = JNI_VERSION_1_6;
    initArgs.nOptions = cmd.nOptions;
    initArgs.options = cmd.options;

    return createJavaVM(&vm, (void **) &env, &initArgs);
}

jobjectArray javaArguments() {
    jclass stringCls = findClass("java/lang/String");
    if (stringCls == NULL) {
        return NULL;
    }

    jmethodID vf = (*env)->GetMethodID(env, stringCls, "<init>", "([B)V");
    if (vf == NULL) {
        fprintf(stderr, "\n");
        return NULL;
    }

    int nArguments = cmd.nArguments;
    const char **arguments = cmd.arguments;

    jobjectArray arr = (*env)->NewObjectArray(env, nArguments, stringCls, NULL);
    for (int i = 0; i < nArguments; ++i) {
        const char *arg = arguments[i];
        size_t len = strlen(arg);

        jbyteArray argChars = (*env)->NewByteArray(env, len);
        (*env)->SetByteArrayRegion(env, argChars, 0, len, (const jbyte *) arg);

        jstring str = (*env)->NewObject(env, stringCls, vf, argChars);
        (*env)->SetObjectArrayElement(env, arr, i - 1, str);
    }
    return arr;
}