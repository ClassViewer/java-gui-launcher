//
// Created by Glavo on 2020.01.10.
//
#include <stdlib.h>
#include "launcher.h"

int main(int argc, char *argv[]) {
    loadJVM();
    if (libjvm == NULL) {
        return EXIT_FAILURE;
    }

    // jvm methods
    jint ans = createJVM();

    if (ans != JNI_OK) {
        fprintf(stderr, "Create JVM failed");
        return ans;
    }

    jthrowable ex;

    jclass mainClass = findClass(APPLICATION_MAIN_CLASS);
    ex = (*env)->ExceptionOccurred(env);
    if (mainClass == NULL) {
        fprintf(stderr, "Main class '" APPLICATION_MAIN_CLASS"' not found");
        return EXIT_FAILURE;
    }
    if (ex != NULL) {
        (*env)->ExceptionDescribe(env);
        return EXIT_FAILURE;
    }


    jmethodID mainMethodID = (*env)->GetStaticMethodID(env, mainClass, "main", "([Ljava/lang/String;)V");
    ex = (*env)->ExceptionOccurred(env);
    if (mainMethodID == NULL) {
        fprintf(stderr, "Main method not found");
        return EXIT_FAILURE;
    }
    if (ex != NULL) {
        (*env)->ExceptionDescribe(env);
        return EXIT_FAILURE;
    }

    parseCMD(argc, argv);
    jobjectArray args = javaArguments();
    ex = (*env)->ExceptionOccurred(env);
    if (args == NULL) {
        fprintf(stderr, "Failed to initialization arguments");
        return EXIT_FAILURE;
    }
    if (ex != NULL) {
        (*env)->ExceptionDescribe(env);
        return EXIT_FAILURE;
    }

    destructCMD();

    ex = (*env)->ExceptionOccurred(env);
    (*env)->CallStaticVoidMethod(env, mainClass, mainMethodID, args);
    if (ex != NULL) {
        (*env)->ExceptionDescribe(env);
        return EXIT_FAILURE;
    }

    (*vm)->DestroyJavaVM(vm);
    return 0;
}


