#include "launcher_win.h"

#ifdef APPLICATION_SHOW_CONSOLE
int main() {
#else

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#endif
    HMODULE jvm = loadJVM();
    if (jvm == NULL) {
        MessageBoxA(NULL, "Not found jvm.dll", NULL, MB_OK);
        return EXIT_FAILURE;
    }

    // jvm methods
    jint ans = createJVM(jvm);

    if (ans != JNI_OK) {
        MessageBoxA(NULL, "Create JVM Failed", NULL, MB_OK);
        return ans;
    }

    jclass mainClass = findClass(APPLICATION_MAIN_CLASS);
    if (mainClass == NULL) {
        MessageBoxA(NULL, "Main class not found", NULL, MB_OK);
        return EXIT_FAILURE;
    }

    jmethodID mainMethodID = (*env)->GetStaticMethodID(env, mainClass, "main", "([Ljava/lang/String;)V");
    if (mainMethodID == NULL) {
        MessageBoxA(NULL, "Main method not found", NULL, MB_OK);
        return EXIT_FAILURE;
    }

    jobjectArray args = javaArguments();
    if (args == NULL) {
        MessageBoxA(NULL, "Failed to initialization arguments", NULL, MB_OK);
        return EXIT_FAILURE;
    }

    (*env)->CallStaticVoidMethod(env, mainClass, mainMethodID, args);
    (*vm)->DestroyJavaVM(vm);
    return 0;
}


