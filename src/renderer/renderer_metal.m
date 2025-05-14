#include "core/camera.h"
#include "defines.h"
#include "math/math.h"
#include "renderer/renderer.h"
#include "resources/image.h"
#include "resources/light.h"
#include "resources/mesh.h"
#include "resources/shader.h"
#include "resources/texture.h"
#include "shaders/metal/metallib_data.h"

#include <CoreFoundation/CFString.h>
#include <Foundation/Foundation.h>
#include <Foundation/NSObjCRuntime.h>
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

typedef struct {
    id<MTLBuffer> vertex_buffer;
    id<MTLBuffer> index_buffer;

    Mesh_Data *data;
} Metal_Mesh;

typedef struct {
    id<MTLRenderPipelineState> pipeline_state;
    id<MTLTexture> albedo_texture;

    Material_Data *data;
} Metal_Material;

struct Renderer {
    id<MTLDevice> device;
    id<MTLCommandQueue> command_queue;
    id<CAMetalDrawable> drawable;
    CAMetalLayer *metal_layer;

    MTLRenderPassDescriptor *pass;
    id<MTLBuffer> uniform_buffer;
    id<MTLBuffer> light_buffer;
    id<MTLBuffer> light_count_buffer;
    id<MTLTexture> depth_texture;
    id<MTLDepthStencilState> depth_state;

    Metal_Mesh *meshes;
    u32 mesh_count;
    u32 mesh_capacity;
    Metal_Material *materials;
    u32 material_count;
    u32 material_capacity;
    u32 default_material_id;
};

typedef struct Metal_Shader {
    id<MTLLibrary> library;
    id<MTLFunction> vertex_function;
    id<MTLFunction> fragment_function;
} Metal_Shader;

typedef struct {
    Light lights[MAX_LIGHTS];
    u32 light_count;
} Light_Buffer;

Renderer *renderer_init(void *platform_data, u32 width, u32 height) {
    Renderer *r = (Renderer *)calloc(1, sizeof(Renderer));

    // Инициализация Metal.
    r->metal_layer = (__bridge CAMetalLayer *)platform_data;
    r->device = MTLCreateSystemDefaultDevice();
    if (!r->device) {
        free(r);
        return NULL;
    }
    r->metal_layer.device = r->device;

    r->command_queue = [r->device newCommandQueue];
    r->pass = [MTLRenderPassDescriptor renderPassDescriptor];

    // Инициализация буферов.
    r->uniform_buffer = [r->device newBufferWithLength:sizeof(Uniforms) options:MTLResourceStorageModeShared];
    r->light_buffer = [r->device newBufferWithLength:sizeof(Light_Buffer) options:MTLResourceStorageModeShared];
    r->mesh_capacity = 16;
    r->meshes = calloc(r->mesh_capacity, sizeof(Metal_Mesh));
    r->material_capacity = 16;
    r->materials = calloc(r->material_capacity, sizeof(Metal_Material));

    // Создание текстуры глубины
    MTLTextureDescriptor *depth_texture_desc =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                           width:width
                                                          height:height
                                                       mipmapped:NO];
    depth_texture_desc.usage = MTLTextureUsageRenderTarget;
    depth_texture_desc.storageMode = MTLStorageModePrivate;
    r->depth_texture = [r->device newTextureWithDescriptor:depth_texture_desc];
    r->depth_state = [r->device newDepthStencilStateWithDescriptor:({
                                    MTLDepthStencilDescriptor *desc = [[MTLDepthStencilDescriptor alloc] init];
                                    desc.depthCompareFunction = MTLCompareFunctionLess;
                                    desc.depthWriteEnabled = YES;
                                    desc;
                                })];

    // Загрузка материалов по умолчанию.
    NSError *error = NULL;
    NSData *nsdata = [NSData dataWithBytes:build_shaders_metal_default_metallib
                                    length:build_shaders_metal_default_metallib_len];
    dispatch_data_t dispatch_data =
        dispatch_data_create([nsdata bytes], [nsdata length], nil, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    id<MTLLibrary> lib = [r->device newLibraryWithData:dispatch_data error:&error];
    if (error != NULL) {
        NSLog(@"new library error: %@", [error localizedDescription]);
        exit(1);
    }

    id<MTLFunction> vertex_fn = [lib newFunctionWithName:@"mesh_vertex"];
    id<MTLFunction> fragment_fn = [lib newFunctionWithName:@"mesh_fragment"];
    if (!vertex_fn || !fragment_fn) {
        NSLog(@"shader functions error: %@", [error localizedDescription]);
        exit(1);
    }

    // Создание материала по умолчанию,
    MTLVertexDescriptor *vertex_desc = [[MTLVertexDescriptor alloc] init];
    vertex_desc.attributes[0].format = MTLVertexFormatFloat3;
    vertex_desc.attributes[1].format = MTLVertexFormatFloat3;
    vertex_desc.attributes[2].format = MTLVertexFormatFloat2;
    vertex_desc.attributes[0].bufferIndex = 0;
    vertex_desc.attributes[1].bufferIndex = 0;
    vertex_desc.attributes[2].bufferIndex = 0;
    vertex_desc.attributes[0].offset = offsetof(vertex3d, position);
    vertex_desc.attributes[1].offset = offsetof(vertex3d, normal);
    vertex_desc.attributes[2].offset = offsetof(vertex3d, uv);
    vertex_desc.layouts[0].stride = sizeof(vertex3d);
    vertex_desc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

    MTLRenderPipelineDescriptor *desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.rasterizationEnabled = YES;
    desc.vertexDescriptor = vertex_desc;
    desc.vertexFunction = vertex_fn;
    desc.fragmentFunction = fragment_fn;
    desc.colorAttachments[0].pixelFormat = r->metal_layer.pixelFormat;
    desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    r->materials[0].pipeline_state = [r->device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (error != NULL) {
        NSLog(@"pipeline state: %@", [error localizedDescription]);
        exit(1);
    }
    r->materials[0].data = calloc(1, sizeof(Material_Data));
    r->materials[0].data->color = vec3_one();
    r->materials[0].albedo_texture = nil;
    r->material_count = 1;
    r->default_material_id = 0;

    printf("Renderer initialized: device=%p, command_queue=%p\n", r->device, r->command_queue);

    return r;
}

void renderer_destroy(Renderer *r) {
    // free default material
    if (r->materials[0].data) {
        free(r->materials[0].data);
    }

    free(r->meshes);
    free(r->materials);
    free(r);
}

void renderer_begin_frame(Renderer *r, Camera *cam, Scene *scene) {
    mat4 proj = mat4_perspective(cam->fov * DEG2RAD, cam->aspect, cam->near_clip, cam->far_clip);

    Uniforms u = {
        .view = cam->view,
        .proj = proj,
        .camera_position = vec4_new(cam->position.x, cam->position.y, cam->position.z, 1.0),
    };
    memcpy([r->uniform_buffer contents], &u, sizeof(Uniforms));

    // Настройка Light_Buffer.
    Light_Buffer lb = {.light_count = scene->light_count};
    for (u32 i = 0; i < scene->light_count; i++) {
        lb.lights[i] = *(scene->lights[i]);
    }
    memcpy([r->light_buffer contents], &lb, sizeof(Light_Buffer));

    r->drawable = [r->metal_layer nextDrawable];
    if (!r->drawable) {
        printf("No drawable available\n");
        return;
    }

    r->pass.colorAttachments[0].texture = r->drawable.texture;
    r->pass.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
    r->pass.colorAttachments[0].loadAction = MTLLoadActionClear;
    r->pass.colorAttachments[0].storeAction = MTLStoreActionStore;
    r->pass.depthAttachment.texture = r->depth_texture;
    r->pass.depthAttachment.clearDepth = 1.0;
    r->pass.depthAttachment.loadAction = MTLLoadActionClear;
    r->pass.depthAttachment.storeAction = MTLStoreActionDontCare;
}

void renderer_draw(Renderer *r, Render_Command *commands, u32 command_count) {
    // Проверка валидности рендерера и очереди команд
    assert(r != NULL);
    assert(r->command_queue != nil);

    @autoreleasepool {
        id<MTLCommandBuffer> command_buffer = [r->command_queue commandBuffer];
        id<MTLRenderCommandEncoder> encoder = [command_buffer renderCommandEncoderWithDescriptor:r->pass];

        // Включаем тест глубины.
        [encoder setDepthStencilState:r->depth_state];

        // Устанавливаем uniform буфер только один раз перед циклом.
        [encoder setVertexBuffer:r->uniform_buffer offset:0 atIndex:1];
        [encoder setFragmentBuffer:r->uniform_buffer offset:0 atIndex:0];
        [encoder setFragmentBuffer:r->light_buffer offset:0 atIndex:1];

        for (u32 i = 0; i < command_count; i++) {
            Render_Command *cmd = &commands[i];

            // Проверка валидности индексов
            if (cmd->mesh_id >= r->mesh_count || cmd->material_id >= r->material_count) {
                printf("skip command\n");
                continue;
            }

            Metal_Mesh *mesh = &r->meshes[cmd->mesh_id];
            Metal_Material *material = &r->materials[cmd->material_id];

            [encoder setRenderPipelineState:material->pipeline_state];
            [encoder setVertexBuffer:mesh->vertex_buffer offset:0 atIndex:0];
            [encoder setVertexBytes:&cmd->model length:sizeof(mat4) atIndex:2];
            [encoder setFragmentBytes:material->data length:sizeof(Material_Data) atIndex:2];
            if (material->albedo_texture) {
                [encoder setFragmentTexture:material->albedo_texture atIndex:0];
            } else {
                [encoder setFragmentTexture:nil atIndex:0];
            }

            if (mesh->data->topology == TOPOLOGY_LINE_LIST) {
                [encoder drawIndexedPrimitives:MTLPrimitiveTypeLine
                                    indexCount:mesh->data->index_count
                                     indexType:MTLIndexTypeUInt32
                                   indexBuffer:mesh->index_buffer
                             indexBufferOffset:0];
            } else if (mesh->data->topology == TOPOLOGY_TRIANGLE_LIST) {
                if (mesh->data->is_wireframe) {
                    [encoder setTriangleFillMode:MTLTriangleFillModeLines];
                } else {
                    [encoder setTriangleFillMode:MTLTriangleFillModeFill];
                }
                [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                    indexCount:mesh->data->index_count
                                     indexType:MTLIndexTypeUInt32
                                   indexBuffer:mesh->index_buffer
                             indexBufferOffset:0];
            }
        }

        [encoder endEncoding];
        [command_buffer presentDrawable:r->drawable];
        [command_buffer commit];
    }
}

void renderer_end_frame(Renderer *r) {
}

u32 renderer_load_mesh(Renderer *r, Mesh_Data *md) {
    if (r->mesh_count >= r->mesh_capacity) {
        r->mesh_capacity *= 2;
        r->meshes = (Metal_Mesh *)realloc(r->meshes, sizeof(Metal_Mesh) * r->mesh_capacity);
    }

    Metal_Mesh *mesh = &r->meshes[r->mesh_count];
    mesh->data = md;
    mesh->vertex_buffer = [r->device newBufferWithBytes:md->vertices
                                                 length:md->vertex_count * sizeof(vertex3d)
                                                options:MTLResourceStorageModeShared];
    mesh->index_buffer = [r->device newBufferWithBytes:md->indices
                                                length:md->index_count * sizeof(u32)
                                               options:MTLResourceStorageModeShared];
    return r->mesh_count++;
}

void renderer_destroy_mesh(Renderer *r, u32 mesh_id) {
    Metal_Mesh *mesh = &r->meshes[mesh_id];
    if (mesh->data) free(mesh->data);
}

u32 renderer_load_material(Renderer *r, Material_Data *mat) {
    if (r->material_count >= r->material_capacity) {
        r->material_capacity *= 2;
        r->materials = (Metal_Material *)realloc(r->materials, sizeof(Metal_Material) * r->material_capacity);
    }

    Metal_Material *material = &r->materials[r->material_count];
    material->pipeline_state = r->materials[r->default_material_id].pipeline_state;
    material->data = mat;

    if (mat->albedo_texture) {
        material->albedo_texture = (__bridge id<MTLTexture>)(mat->albedo_texture->gpu_handle);
        mat->has_texture = true;
    }

    return r->material_count++;
}

void renderer_destroy_material(Renderer *r, u32 material_id) {
    Metal_Material *material = &r->materials[material_id];
    if (material->data) free(material->data);
}

void *renderer_load_shader(Renderer *r, const char *filename, const char *v_entry, const char *f_entry) {
    assert(r != nil && r->device != nil);

    NSError *error = nil;
    NSString *path = [NSString stringWithUTF8String:filename];
    NSString *source = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
    if (error != NULL) {
        NSLog(@"shader source error: %@", [error localizedDescription]);
        exit(1);
    }

    id<MTLLibrary> lib = [r->device newLibraryWithSource:source options:nil error:&error];
    if (error != NULL) {
        NSLog(@"shader compile error: %@", [error localizedDescription]);
        exit(1);
    }

    Metal_Shader *shader = malloc(sizeof(Metal_Shader));
    shader->library = lib;
    shader->vertex_function = [lib newFunctionWithName:[NSString stringWithUTF8String:v_entry]];
    shader->fragment_function = [lib newFunctionWithName:[NSString stringWithUTF8String:f_entry]];
    if (!shader->vertex_function || !shader->fragment_function) {
        NSLog(@"Failed to load shader functions");
        exit(1);
    }

    return shader;
}

void renderer_destroy_shader(void *s) {
    if (!s) return;

    Metal_Shader *shader = (Metal_Shader *)s;
    shader->library = nil;
    shader->vertex_function = nil;
    shader->fragment_function = nil;

    free(shader);
}

Texture *renderer_create_texture(Renderer *r, Image_Data *img) {
    MTLPixelFormat format = MTLPixelFormatRGBA8Unorm;
    u8 *data = img->pixels;
    int stride = img->width * 4;

    u8 *temp_data = NULL;
    if (img->channels == 3) {
        temp_data = malloc(img->width * img->height * 4);
        for (int i = 0; i < img->width * img->height; i++) {
            temp_data[i * 4 + 0] = img->pixels[i * 3 + 0];
            temp_data[i * 4 + 1] = img->pixels[i * 3 + 1];
            temp_data[i * 4 + 2] = img->pixels[i * 3 + 2];
            temp_data[i * 4 + 3] = 255;
        }
        memcpy(data, temp_data, img->width * img->height * 4);
    } else {
        printf("unsupported image format: %d channels", img->channels);
        return nil;
    }

    MTLTextureDescriptor *desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:format
                                                                                    width:img->width
                                                                                   height:img->height
                                                                                mipmapped:NO];
    id<MTLTexture> texture = [r->device newTextureWithDescriptor:desc];

    MTLRegion region = {{0, 0, 0}, {(NSUInteger)img->width, (NSUInteger)img->height, 1}};

    [texture replaceRegion:region mipmapLevel:0 withBytes:data bytesPerRow:stride];

    if (temp_data) free(temp_data);

    Texture *out = calloc(1, sizeof(Texture));
    out->gpu_handle = (__bridge_retained void *)texture;
    out->width = texture.width;
    out->height = texture.height;
    return out;
}

void renderer_destroy_texture(Renderer *r, Texture *texture) {
    // id<MTLTexture> mtl_texture = (__bridge id<MTLTexture>) texture->gpu_handle;
    // [mtl_texture release];
    //
    free(texture->gpu_handle);
    free(texture);
}
