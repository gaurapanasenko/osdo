#ifndef CONTEXT_H
#define CONTEXT_H

#include "osdo.h"

#include "model.h"
#include "shader.h"
#include "object.h"
#include "EASTL/hash_map.h"
#include "EASTL/string.h"
#include "EASTL/shared_ptr.h"
using eastl::hash_map;
using eastl::string;
using eastl::shared_ptr;
using eastl::pair;
using eastl::make_shared;

struct Context
{
    typedef hash_map<string, Object> Models;
    Models models;
    hash_map<string, shared_ptr<Shader>> shaders;
    Models::iterator active;

public:
    Context();

    Models::iterator &next_active();

    bool load_shader(const char *name, const char *vertex_path,
                     const char *fragment_path);
};

#endif // CONTEXT_H
