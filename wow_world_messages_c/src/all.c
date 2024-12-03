/* clang-format off */

#include "util.h"

#include "wow_world_messages/all.h"

WowWorldResult all_Vector3d_read(WowWorldReader* reader, all_Vector3d* object) {
    READ_FLOAT(object->x);

    READ_FLOAT(object->y);

    READ_FLOAT(object->z);

    return WWM_RESULT_SUCCESS;
}

WowWorldResult all_Vector3d_write(WowWorldWriter* writer, const all_Vector3d* object) {
    WRITE_FLOAT(object->x);

    WRITE_FLOAT(object->y);

    WRITE_FLOAT(object->z);


    return WWM_RESULT_SUCCESS;
}

WowWorldResult all_Vector2d_read(WowWorldReader* reader, all_Vector2d* object) {
    READ_FLOAT(object->x);

    READ_FLOAT(object->y);

    return WWM_RESULT_SUCCESS;
}

WowWorldResult all_Vector2d_write(WowWorldWriter* writer, const all_Vector2d* object) {
    WRITE_FLOAT(object->x);

    WRITE_FLOAT(object->y);


    return WWM_RESULT_SUCCESS;
}

