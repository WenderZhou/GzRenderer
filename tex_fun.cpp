/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

#include <vector>
#include <cstring>
#include <unordered_map>

struct textureInfo
{
    GzColor* image;
    int xs;
    int ys;
};

std::unordered_map<std::string, textureInfo> textureInfos;

void loadTexture(const std::string texturePath, const std::string textureName)
{
    unsigned char		pixel[3];
    unsigned char     dummy;
    char  		foo[20];
    char        absd[50];
    int   		i, j;
    FILE* fd;

    textureInfo info;

    if (textureInfos.find(textureName) != textureInfos.end())
    {
        fprintf(stderr, "texture file %s already loaded\n", texturePath);
        exit(-1);
    }

    fd = fopen(texturePath.c_str(), "rb");
    if (fd == NULL)
    {
        fprintf(stderr, "texture file %s not found\n", texturePath);
        exit(-1);
    }

    fscanf(fd, "%s", foo);

    // get rid of annoying "# Created by IrfanView"
    /*fscanf(fd, "%s", foo);
    fscanf(fd, "%s", foo);
    fscanf(fd, "%s", foo);
    fscanf(fd, "%s", foo);*/

    fscanf(fd, "%d %d %c", &info.xs, &info.ys, &dummy);

    info.image = new GzColor[(info.xs + 1) * (info.ys + 1)];

    if (info.image == NULL)
    {
        fprintf(stderr, "malloc for texture image failed\n");
        exit(-1);
    }

    for (i = 0; i < info.xs * info.ys; i++)/* create array of GzColor values */
    {
        fread(pixel, sizeof(pixel), 1, fd);
        info.image[i].r = (float)((int)pixel[0]) * (1.0 / 255.0);
        info.image[i].g = (float)((int)pixel[1]) * (1.0 / 255.0);
        info.image[i].b = (float)((int)pixel[2]) * (1.0 / 255.0);
    }

    fclose(fd);

    textureInfos.insert(make_pair(textureName, info));
}

void initTexture()
{
    loadTexture("skybox/back.ppm", "back");
    loadTexture("skybox/bottom.ppm", "bottom");
    loadTexture("skybox/front.ppm", "front");
    loadTexture("skybox/left.ppm", "left");
    loadTexture("skybox/right.ppm", "right");
    loadTexture("skybox/top.ppm", "top");
}

/* Image texture function */
GzColor tex_fun(const std::string textureName, float u, float v)
{
    textureInfo info;
    if (textureInfos.find(textureName) != textureInfos.end())
        info = textureInfos[textureName];
    else
    {
        fprintf(stderr, "texture %s not found\n", textureName);
        exit(-1);
    }

    GzColor* image = info.image;
    int xs = info.xs;
    int ys = info.ys;

    u = min(1.0f, max(0.0f, u));
    v = min(1.0f, max(0.0f, v));

    int left = floorf(u * (xs - 1));
    int right = left + 1;
    int top = floorf(v * (ys - 1));
    int bottom = top + 1;

    float s = u * (xs - 1) - left;
    float t = v * (ys - 1) - top;

    return (1 - s) * (1 - t) * image[left + top * xs] + s * (1 - t) * image[right + top * xs] + (1 - s) * t * image[left + bottom * xs] + s * t * image[right + bottom * xs];
}


GzColor envtex_fun(GzCoord ref)
{
    unsigned char		pixel[3];
    unsigned char     dummy;
    char  		foo[8];
    int   		i, j;

    float absX = fabsf(ref.x);
    float absY = fabsf(ref.y);
    float absZ = fabsf(ref.z);

    bool isXPositive = ref.x > 0 ? true : false;
    bool isYPositive = ref.y > 0 ? true : false;
    bool isZPositive = ref.z > 0 ? true : false;

    int face = 0;
    float u, v;

    if (isXPositive && absX >= absY && absX >= absZ)
    {
        u = 0.5f * (1.0f + ref.z / absX);
        v = 0.5f * (1.0f - ref.y / absX);
        return tex_fun("right", u, v);
    }
    else if (!isXPositive && absX >= absY && absX >= absZ)
    {
        u = 0.5f * (1.0f - ref.z / absX);
        v = 0.5f * (1.0f - ref.y / absX);
        return tex_fun("left", u, v);
    }
    else if (isYPositive && absY >= absX && absY >= absZ)
    {
        u = 0.5f * (1.0f + ref.x / absY);
        v = 0.5f * (1.0f - ref.z / absY);
        return tex_fun("top", u, v);
    }
    else if (!isYPositive && absY >= absX && absY >= absZ)
    {
        u = 0.5f * (1.0f + ref.x / absY);
        v = 0.5f * (1.0f + ref.z / absY);
        return tex_fun("bottom", u, v);
    }
    else if (isZPositive && absZ >= absX && absZ >= absY)
    {
        u = 0.5f * (1.0f - ref.x / absZ);
        v = 0.5f * (1.0f - ref.y / absZ);
        return tex_fun("back", u, v);
    }
    else if (!isZPositive && absZ >= absX && absZ >= absY) // front
    {
        u = 0.5f * (1.0f + ref.x / absZ);
        v = 0.5f * (1.0f - ref.y / absZ);
        return tex_fun("front", u, v);
    }

    fprintf(stderr, "environment mapping error\n");
    exit(-1);
}

/* Free texture memory */
int GzFreeTexture()
{
    for (auto info : textureInfos)
        delete info.second.image;

	return GZ_SUCCESS;
}

