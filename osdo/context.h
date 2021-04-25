#ifndef CONTEXT_H
#define CONTEXT_H

#include "osdo.h"

#include "model.h"
#include "shader.h"
#include "object.h"
#include "texture.h"
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
    typedef hash_map<string, shared_ptr<Texture>> Textures;
    Models models;
    hash_map<string, shared_ptr<Shader>> shaders;
    Textures textures;

    Models::iterator active;
    Textures::iterator active_texture;

public:
    Context();

    Models::iterator &next_active();

    void load_texture(const char *path);

    bool load_shader(const char *name, const Shader::shader_map& shaders);

    bool load_model(const string& path);
};

#endif // CONTEXT_H
