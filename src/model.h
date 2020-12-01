#ifndef MODEL_H
#define MODEL_H

#include "osdo.h"

typedef union __attribute__((transparent_union)) model_t {
   struct Beziator *beziator;
   struct Mesh *mesh;
} model_t;

typedef void (*draw_cb)(model_t model);
typedef bool (*generate_cb)(model_t model);
typedef void (*free_cb)(model_t model);

typedef struct ModelType {
    draw_cb draw;
    generate_cb generate;
    free_cb free;
} ModelType;

typedef struct Model {
    char name[64];
    model_t model;
    const ModelType *type;
    UT_hash_handle hh;
} Model;

static const UT_icd model_icd = {
    sizeof(Model), NULL, NULL, NULL
};

void model_init(Model* model, const char *name,
                model_t model_child, const ModelType *type);

Model *model_create(const char *name, model_t model,
                    const ModelType *type);

void model_draw(Model* model);

void model_generate(Model *model);

void model_free(Model *model);

#endif // MODEL_H
