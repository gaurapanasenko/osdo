#ifndef MODEL_H
#define MODEL_H

#include "osdo.h"

class Model {
public:
    virtual void draw() {}
    virtual void generate() {}
    virtual ~Model() {}
};

#endif // MODEL_H
