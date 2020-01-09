#include "launcher_win.h"

#ifdef APPLICATION_SHOW_CONSOLE
int main() {
#else

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#endif
    HMODULE jvm = loadJVM();
    if (jvm == NULL) {
        MessageBoxA(NULL, "Not found " APPLICATION_JRE_PATH "\\bin\\" APPLICATION_JVM_TYPE "\\jvm.dll", NULL, MB_OK);
        return EXIT_FAILURE;
    }

    // jvm methods
    jint ans = createJVM(jvm);

    if (ans != JNI_OK) {
        MessageBoxA(NULL, "Create JVM Failed", NULL, MB_OK);
        return ans;
    }

    jthrowable ex;

    jclass mainClass = findClass(APPLICATION_MAIN_CLASS);
    ex = (*env)->ExceptionOccurred(env);
    if (mainClass == NULL) {
        MessageBoxA(NULL, "Main class '" APPLICATION_MAIN_CLASS"' not found", NULL, MB_OK);
        return EXIT_FAILURE;
    }
    if (ex != NULL) {
        (*env)->ExceptionDescribe(env);
        return EXIT_FAILURE;
    }


    jmethodID mainMethodID = (*env)->GetStaticMethodID(env, mainClass, "main", "([Ljava/lang/String;)V");
    ex = (*env)->ExceptionOccurred(env);
    if (mainMethodID == NULL) {
        MessageBoxA(NULL, "Main method not found", NULL, MB_OK);
        return EXIT_FAILURE;
    }
    if (ex != NULL) {
        (*env)->ExceptionDescribe(env);
        return EXIT_FAILURE;
    }

    parseCMD();
    jobjectArray args = javaArguments();
    ex = (*env)->ExceptionOccurred(env);
    if (args == NULL) {
        MessageBoxA(NULL, "Failed to initialization arguments", NULL, MB_OK);
        return EXIT_FAILURE;
    }
    if (ex != NULL) {
        (*env)->ExceptionDescribe(env);
        return EXIT_FAILURE;
    }

    ex = (*env)->ExceptionOccurred(env);
    (*env)->CallStaticVoidMethod(env, mainClass, mainMethodID, args);
    if (ex != NULL) {
        (*env)->ExceptionDescribe(env);
        return EXIT_FAILURE;
    }

    (*vm)->DestroyJavaVM(vm);
    return 0;
}


