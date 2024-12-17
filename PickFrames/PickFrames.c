#include <stdlib.h>
#include "VapourSynth4.h"

typedef struct {
    VSNode *node;
    int *indices;
    int num_frames;
} PickFramesData;

static const VSFrame *VS_CC pickFramesGetFrame(int n, int activationReason, void *instanceData, 
    void **frameData, VSFrameContext *frameCtx, VSCore *core, const VSAPI *vsapi) {
    
    PickFramesData *d = (PickFramesData *)instanceData;

    if (activationReason == arInitial) {
        vsapi->requestFrameFilter(d->indices[n], d->node, frameCtx);
    } else if (activationReason == arAllFramesReady) {
        const VSFrame *src = vsapi->getFrameFilter(d->indices[n], d->node, frameCtx);
        return src;
    }

    return NULL;

    (void) frameData, core;
}

static void VS_CC pickFramesFree(void *instanceData, VSCore *core, const VSAPI *vsapi) {
    PickFramesData *d = (PickFramesData *)instanceData;
    vsapi->freeNode(d->node);
    free(d->indices);
    free(d);

    (void) core;
}

static void VS_CC pickFramesCreate(const VSMap *in, VSMap *out, void *userData, 
    VSCore *core, const VSAPI *vsapi) {
    
    PickFramesData d;
    PickFramesData *data;

    d.node = vsapi->mapGetNode(in, "clip", 0, 0);
    const VSVideoInfo *vi = vsapi->getVideoInfo(d.node);

    int err;
    int num_indices = vsapi->mapNumElements(in, "indices");
    if (num_indices <= 0) {
        vsapi->mapSetError(out, "PickFrames: indices array must not be empty");
        vsapi->freeNode(d.node);
        return;
    }

    d.indices = (int *)malloc(num_indices * sizeof(int));
    d.num_frames = num_indices;

    for (int i = 0; i < num_indices; i++) {
        d.indices[i] = vsapi->mapGetInt(in, "indices", i, &err);
        if (d.indices[i] < 0 || d.indices[i] >= vi->numFrames) {
            vsapi->mapSetError(out, "PickFrames: frame index out of range");
            free(d.indices);
            vsapi->freeNode(d.node);
            return;
        }
    }

    VSVideoInfo vi_out = *vi;
    vi_out.numFrames = num_indices;

    data = (PickFramesData *)malloc(sizeof(d));
    *data = d;

    VSFilterDependency deps[] = {{d.node, rpStrictSpatial}};
    vsapi->createVideoFilter(out, "PickFrames", &vi_out, pickFramesGetFrame, 
        pickFramesFree, fmParallel, deps, 1, data, core);
    
    (void) userData;
}

VS_EXTERNAL_API(void) VapourSynthPluginInit2(VSPlugin *plugin, const VSPLUGINAPI *vspapi) {
    vspapi->configPlugin("com.yuygfgg.pickframes", "pickframes", 
        "VapourSynth PickFrames Plugin", VS_MAKE_VERSION(1, 0), 
        VAPOURSYNTH_API_VERSION, 0, plugin);
    vspapi->registerFunction("PickFrames", 
        "clip:vnode;indices:int[];", "clip:vnode;", 
        pickFramesCreate, NULL, plugin);
}