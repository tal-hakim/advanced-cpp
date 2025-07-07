//
// Created by talta on 07/07/2025.
//

#ifndef ASGN3_SOHANDLE_H
#define ASGN3_SOHANDLE_H

#include <string>
#include <dlfcn.h>

class SOHandle{
private:
    void* soHandle;
public:
    SOHandle(std::string soPath) {
        soHandle = dlopen(soPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    }

    bool isInitialized(){
        return soHandle != nullptr;
    }

    ~SOHandle(){
        if( isInitialized() ){
            dlclose(soHandle);
        }
    }
};
#endif //ASGN3_SOHANDLE_H
