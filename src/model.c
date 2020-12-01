#include "model.h"

void model_init(Model *model, const char *name, model_t model_child, const ModelType *type) {
    strcpy(model->name, name);
    model->model = model_child;
    model->type = type;
}

Model *model_create(const char *name, model_t model, const ModelType *type) {
    Model *m = calloc(1, sizeof(Model));
    model_init(m, name, model, type);
    return m;
}

void model_draw(Model *model) {
    model->type->draw(model->model);
}

void model_generate(Model *model) {
    model->type->generate(model->model);
}

void model_free(Model *model) {
    model->type->free(model->model);
    free(model);
}
