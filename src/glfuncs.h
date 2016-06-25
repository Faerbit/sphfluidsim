#ifndef GLFUNCS_H
#define GLFUNCS_H

#include <memory>
#include <iostream>
#include <QOpenGLFunctions_4_3_Core>

class glFuncs {
public:
    static std::shared_ptr<QOpenGLFunctions_4_3_Core> funcs() {
        return getInstance()._funcs;
    }

    glFuncs(glFuncs const&)         = delete;
    void operator=(glFuncs const&)  = delete;
private:
    static glFuncs& getInstance() {
        static glFuncs instance;
        return instance;
    }
    glFuncs() {
        _funcs = std::shared_ptr<QOpenGLFunctions_4_3_Core>(
                new QOpenGLFunctions_4_3_Core());
        if (!_funcs->initializeOpenGLFunctions()) {
            std::cerr << "Failed to initialize OpenGL Functions." << std::endl;
            std::cerr << "Your host must support OpenGL 4.3." << std::endl;
            exit(1);
        }
    }
    std::shared_ptr<QOpenGLFunctions_4_3_Core> _funcs;
};

#endif // GLFUNCS_H
