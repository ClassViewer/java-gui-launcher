//
// Created by Glavo on 2019.12.27.
//

#ifndef LAUNCHER_LAUNCHER_WIN_H
#define LAUNCHER_LAUNCHER_WIN_H

#include <launcher_config.h>
#include "launcher.h"

typedef struct _CMD {
    int nOptions;
    JavaVMOption *options;
    int nArguments;
    LPWSTR *arguments;
} CMD;

extern CMD cmd;

#endif //LAUNCHER_LAUNCHER_WIN_H
