#ifndef MESHDATA_H
#define MESHDATA_H

struct VertexWithColor
{
    float x, y, z, w;
    float r, g, b, a;
};

struct VertexWithUV
{
    float x, y, z, w;
    float u, v;
};

static const VertexWithColor triangleData[] =
{
    {  0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0 },
    {  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0 },
    { -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0 },
};

#endif // MESHDATA_H
