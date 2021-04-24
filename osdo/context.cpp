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

bool Context::load_shader(const char *name, const Shader::shader_map& shaders) {
    auto shader = Shader::create(shaders);
    if (!shader)
        return false;
    this->shaders[string(name)] = shader;
    return true;
}
