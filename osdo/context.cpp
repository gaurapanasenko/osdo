#include "context.h"
#include "glbinder.h"
#include "image.h"
#include "texture.h"

Context::Context() : active(models.end()), active_texture(textures.end()) {

}

Context::Models::iterator &Context::next_active() {
    if (active == models.end()) {
        active = models.begin();
    } else active++;
    return active;
}

void Context::load_texture(const char *path) {
    Image img = Image::fromFile(path);
    if (img.data) {
        auto tex = make_shared<Texture>();
        tex->update(img);
        textures[path] = tex;
    }
}

bool Context::load_shader(const char *name, const Shader::shader_map& shaders) {
    auto shader = Shader::create(shaders);
    if (!shader)
        return false;
    this->shaders[string(name)] = shader;
    return true;
}
