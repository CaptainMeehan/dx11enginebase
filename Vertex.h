#pragma once

struct Vertex {
    float x, y, z, w; // Position
    float r, g, b, a; // Color
    float u, v;       // Texture coordinates
    float nx, ny, nz; // Normal (for lighting)
    float tx, ty, tz; // Tangent
    float bx, by, bz; // Bitangent
};