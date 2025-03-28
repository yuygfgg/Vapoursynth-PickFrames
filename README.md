# Vapoursynth PickFrames Plugin

**Archived because the filter is intergrated into [my fork of akarin plugin](https://github.com/yuygfgg/akarin-vapoursynth-plugin)**

The akarin plugin added PickFrames in its v0.96g release, but now (and since 2 years ago) the filter is only available from the pre-built x86 only release binaries.

## Usage

copied from https://github.com/AkarinVS/vapoursynth-plugin/releases.

pickframes.PickFrames(clip clip, int[] indices)

Return a new clip with frames picked from input clip from the indices array.

PickFrames(c, indices) is functionally equivalent to c[indices[0]] + c[indices[1]] + ... + c[indices[-1]], only more efficient as you only need to create one filter instance, especially when len(indices) is very large. As long as all indices are within range [0, c.num_frames-1], there is no limit on the the indices.

## Building

```bash
meson build
ninja -C build install
```
