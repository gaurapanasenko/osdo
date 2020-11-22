#include "scene.h"
#include "conf.h"

Scene scene_init(void) {
    Scene new_scene = {
        {
            basis_init(true),
            basis_init(false),
            basis_init(false),
        },
        true, 0, 0, 0, 1, 1, 1, 1, (float)M_PI / 4
    };
    basis_translate(&new_scene.basis[0], BASIS0POS);
    basis_translate(&new_scene.basis[1], BASIS1POS);
    basis_translate(&new_scene.basis[2], BASIS2POS);

    basis_rotate_all(&new_scene.basis[1], BASIS1ROT);
    basis_rotate_all(&new_scene.basis[2], BASIS2ROT);

    return new_scene;
}
