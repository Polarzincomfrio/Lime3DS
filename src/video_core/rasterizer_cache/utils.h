// Copyright 2023 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <span>
#include "common/hash.h"
#include "common/math_util.h"
#include "common/slot_vector.h"
#include "common/vector_math.h"
#include "video_core/regs_texturing.h"

namespace VideoCore {

using SurfaceId = Common::SlotId;
using SamplerId = Common::SlotId;

/// Fake surface ID for null surfaces
constexpr SurfaceId NULL_SURFACE_ID{0};
/// Fake surface ID for null cube surfaces
constexpr SurfaceId NULL_SURFACE_CUBE_ID{1};
/// Fake sampler ID for null samplers
constexpr SamplerId NULL_SAMPLER_ID{0};

struct Offset {
    u32 x = 0;
    u32 y = 0;
};

struct Extent {
    u32 width = 1;
    u32 height = 1;
};

union ClearValue {
    Common::Vec4f color;
    struct {
        float depth;
        u8 stencil;
    };
};

struct TextureClear {
    u32 texture_level;
    Common::Rectangle<u32> texture_rect;
    ClearValue value;
};

struct TextureCopy {
    u32 src_level;
    u32 dst_level;
    u32 src_layer;
    u32 dst_layer;
    Offset src_offset;
    Offset dst_offset;
    Extent extent;
};

struct TextureBlit {
    u32 src_level;
    u32 dst_level;
    u32 src_layer;
    u32 dst_layer;
    Common::Rectangle<u32> src_rect;
    Common::Rectangle<u32> dst_rect;
};

struct BufferTextureCopy {
    u32 buffer_offset;
    u32 buffer_size;
    Common::Rectangle<u32> texture_rect;
    u32 texture_level;
};

struct StagingData {
    u32 size;
    std::span<u8> mapped;
    u64 buffer_offset;
};

struct TextureCubeConfig {
    PAddr px;
    PAddr nx;
    PAddr py;
    PAddr ny;
    PAddr pz;
    PAddr nz;
    u32 width;
    u32 levels;
    Pica::TexturingRegs::TextureFormat format;

    bool operator==(const TextureCubeConfig& rhs) const {
        return std::memcmp(this, &rhs, sizeof(TextureCubeConfig)) == 0;
    }

    bool operator!=(const TextureCubeConfig& rhs) const {
        return std::memcmp(this, &rhs, sizeof(TextureCubeConfig)) != 0;
    }

    const u64 Hash() const {
        return Common::ComputeHash64(this, sizeof(TextureCubeConfig));
    }
};

class SurfaceParams;

u32 MipLevels(u32 width, u32 height, u32 max_level);

/**
 * Encodes a linear texture to the expected linear or tiled format.
 *
 * @param surface_info Structure used to query the surface information.
 * @param start_addr The start address of the dest data. Used if tiled.
 * @param end_addr The end address of the dest data. Used if tiled.
 * @param source_tiled The source linear texture data.
 * @param dest_linear The output buffer where the encoded linear or tiled data will be written to.
 * @param convert Whether the pixel format needs to be converted.
 */
void EncodeTexture(const SurfaceParams& surface_info, PAddr start_addr, PAddr end_addr,
                   std::span<u8> source, std::span<u8> dest, bool convert = false);

/**
 * Decodes a linear or tiled texture to the expected linear format.
 *
 * @param surface_info Structure used to query the surface information.
 * @param start_addr The start address of the source data. Used if tiled.
 * @param end_addr The end address of the source data. Used if tiled.
 * @param source_tiled The source linear or tiled texture data.
 * @param dest_linear The output buffer where the decoded linear data will be written to.
 * @param convert Whether the pixel format needs to be converted.
 */
void DecodeTexture(const SurfaceParams& surface_info, PAddr start_addr, PAddr end_addr,
                   std::span<u8> source, std::span<u8> dest, bool convert = false);

} // namespace VideoCore

namespace std {
template <>
struct hash<VideoCore::TextureCubeConfig> {
    std::size_t operator()(const VideoCore::TextureCubeConfig& config) const noexcept {
        return config.Hash();
    }
};
} // namespace std
