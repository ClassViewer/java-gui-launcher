//
// Created by Glavo on 2019.12.27.
//

#include "launcher_win.h"
#include <pathcch.h>
#include <strsafe.h>

#define MAX_LONG_PATH_SIZE 32768

const char APPLICATION_PREDEF_VM_OPTIONS[] = APPLICATION_PREDEF_OPTIONS;

CMD cmd;

void parseCMD() {
    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);


    int preOptionsSize = 0;
    for (int i = 0; i < sizeof(APPLICATION_PREDEF_VM_OPTIONS) / sizeof(char); ++i) {
        if (APPLICATION_PREDEF_VM_OPTIONS[i] == '\0') {
            ++preOptionsSize;
        }
    }

    int nOptions = 0;
    JavaVMOption *options = malloc((argc - 1 + preOptionsSize) * sizeof(JavaVMOption));
    int nArguments = 0;
    LPWSTR *arguments = malloc((argc - 1) * sizeof(LPSTR));

    const char *p = APPLICATION_PREDEF_VM_OPTIONS;
    while (p < APPLICATION_PREDEF_VM_OPTIONS + sizeof(APPLICATION_PREDEF_VM_OPTIONS)) {
        size_t len = strlen(p);
        if (len == 0) {
            ++p;
            continue;
        }
        LPSTR newStr = malloc(len + 1);
        memcpy(newStr, p, len + 1);
        options[nOptions].optionString = newStr;
        options[nOptions].extraInfo = NULL;
        ++nOptions;
        p += len + 1;
    }

    for (int i = 1; i < argc; ++i) {
        LPWSTR str = argv[i];
        int strLen = lstrlenW(str);
        if (strLen > 2 && str[0] == L'-' && str[1] == L'J') {
            str += 2;
            int len = WideCharToMultiByte(
                    CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
            LPSTR s = calloc(len, sizeof(char));
            WideCharToMultiByte(
                    CP_UTF8, 0, str, -1, s, len, NULL, NULL);
            options[nOptions].optionString = s;
            options[nOptions].extraInfo = NULL;
            ++nOptions;
        } else {
            size_t size = (strLen + 1) * sizeof(WCHAR);
            LPWSTR newStr = malloc(size);
            memcpy(newStr, str, size);
            arguments[nArguments++] = newStr;
        }
    }

    LocalFree(argv);

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

void destructCMD() {
    int nOptions = cmd.nOptions;
    JavaVMOption *options = cmd.options;
    int nArguments = cmd.nArguments;
    LPWSTR *arguments = cmd.arguments;

    cmd.nOptions = 0;
    cmd.options = NULL;
    cmd.nArguments = 0;
    cmd.arguments = NULL;

    for (int i = 0; i < nOptions; ++i) {
        free(options[i].optionString);
    }
    free(options);

    for (int i = 0; i < nArguments; ++i) {
        free(arguments[i]);
    }
    free(arguments);
}

HMODULE loadJVM() {
    WCHAR *buffer = calloc(MAX_LONG_PATH_SIZE, sizeof(WCHAR));
    if (GetModuleFileNameW(NULL, buffer, MAX_LONG_PATH_SIZE)) {
        PathCchRemoveFileSpec(buffer, MAX_LONG_PATH_SIZE);
        PathCchAppendEx(buffer, MAX_LONG_PATH_SIZE, APPLICATION_JRE_PATH "\\bin\\" APPLICATION_JVM_TYPE L"\\jvm.dll", PATHCCH_ALLOW_LONG_PATHS);
        HMODULE jvm = LoadLibraryW(buffer);
        free(buffer);
        return jvm;
    }
    free(buffer);
    return NULL;
}

jint createJVM(HMODULE lib) {
    CreateJavaVM_t createJavaVM = (CreateJavaVM_t) GetProcAddress(lib, "JNI_CreateJavaVM");

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

    jmethodID vf = (*env)->GetStaticMethodID(env, stringCls, "valueOf", "([C)Ljava/lang/String;");
    if (vf == NULL) {
        return NULL;
    }

    int nArguments = cmd.nArguments;
    LPWSTR *arguments = cmd.arguments;

    jobjectArray arr = (*env)->NewObjectArray(env, nArguments, stringCls, NULL);
    for (int i = 0; i < nArguments; ++i) {
        LPWSTR arg = arguments[i];
        size_t len = wcslen(arg);

        jcharArray argChars = (*env)->NewCharArray(env, len);
        (*env)->SetCharArrayRegion(env, argChars, 0, len, (const jchar *) arg);

        jstring str = (*env)->CallStaticObjectMethod(env, stringCls, vf, argChars);
        (*env)->SetObjectArrayElement(env, arr, i - 1, str);
    }
    return arr;
}