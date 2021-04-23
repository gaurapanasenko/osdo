#include "context.h"

Context::Context() : active(models.end())
{

}

Context::Models::iterator &Context::next_active() {
    if (active == models.end()) {
        active = models.begin();
    } else active++;
    return active;
}

bool Context::load_shader(const char *name, const char *vertex_path,
                          const char *fragment_path) {
    auto shader = Shader::create(vertex_path, fragment_path);
    if (!shader)
        return false;
    this->shaders[string(name)] = shader;
    return true;
}
