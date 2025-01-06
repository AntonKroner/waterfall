#ifndef BindGroups_H
#define BindGroups_H
#include "webgpu.h"
#include "./TTexture.h"

typedef struct {
    size_t count;
    WGPUBindGroup* groups;
} Bindgroups;



#endif // BindGroups_H
