/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
    unsigned char		pixel[3];
    unsigned char     dummy;
    char  		foo[8];
    int   		i, j;
    FILE			*fd;

    if (reset) 
    {          /* open and load texture file */
        fd = fopen ("texture", "rb");
        if (fd == NULL) 
        {
            fprintf (stderr, "texture file not found\n");
            exit(-1);
        }
        fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
        image = (GzColor*)malloc(sizeof(GzColor) * (xs + 1) * (ys + 1));

        if (image == NULL) 
        {
            fprintf (stderr, "malloc for texture image failed\n");
            exit(-1);
        }

        for (i = 0; i < xs * ys; i++)/* create array of GzColor values */
        {
            fread(pixel, sizeof(pixel), 1, fd);
            image[i].r = (float)((int)pixel[0]) * (1.0 / 255.0);
            image[i].g = (float)((int)pixel[1]) * (1.0 / 255.0);
            image[i].b = (float)((int)pixel[2]) * (1.0 / 255.0);
        }

        reset = 0;          /* init is done */
        fclose(fd);
    }

    /* bounds-test u,v to make sure nothing will overflow image array bounds */
    /* determine texture cell corner values and perform bilinear interpolation */
    /* set color to interpolated GzColor value and return */

    if(u < 0 || u >= 1 || v < 0 || v >= 1)
        return GZ_FAILURE;

    int left = floorf(u * (xs - 1));
    int right = left + 1;
    int top = floorf(v * (ys - 1));
    int bottom = top + 1;

    float s = u * (xs - 1) - left;
    float t = v * (ys - 1) - top;

    color = (1 - s) * (1 - t) * image[left + top * xs] + s * (1 - t) * image[right + top * xs] + (1 - s) * t * image[left + bottom * xs] + s * t * image[right + bottom * xs];
    
	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
    int s = (int)floorf(u / 0.1f);
    int t = (int)floorf(v / 0.1f);
    if (s % 2 == t % 2)
        color = { 1.0f, 0.0f, 0.0f };
    else
        color = { 0.0f, 0.0f, 1.0f };
	return GZ_SUCCESS;
}

// environment mapping texture
GzColor* envs[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
int env_xs[6];
int env_ys[6];
int env_reset = 1;

typedef enum {
    back, bottom, front, left, right, top
} Face;

GzColor envtex_fun(GzCoord ref)
{
    unsigned char		pixel[3];
    unsigned char     dummy;
    char  		foo[8];
    int   		i, j;

    char env_fileName[6][20] = { "skybox/back.ppm", "skybox/bottom.ppm", "skybox/front.ppm", "skybox/left.ppm", "skybox/right.ppm", "skybox/top.ppm" };

    if (env_reset)
    {
        for (int i = 0; i < 6; i++)
        {
            FILE* fd = fopen(env_fileName[i], "rb");

            if (fd == NULL)
            {
                fprintf(stderr, "texture file not found\n");
                exit(-1);
            }

            fscanf(fd, "%s %d %d %c", foo, &env_xs[i], &env_ys[i], &dummy);

            envs[i] = (GzColor*)malloc(sizeof(GzColor) * (env_xs[i] + 1) * (env_ys[i] + 1));

            if (envs[i] == NULL)
            {
                fprintf(stderr, "malloc for texture image failed\n");
                exit(-1);
            }

            for (j = 0; j < env_xs[i] * env_ys[i]; j++)
            {
                fread(pixel, sizeof(pixel), 1, fd);
                envs[i][j].r = (float)((int)pixel[0]) * (1.0 / 255.0);
                envs[i][j].g = (float)((int)pixel[1]) * (1.0 / 255.0);
                envs[i][j].b = (float)((int)pixel[2]) * (1.0 / 255.0);
            }

            fclose(fd);
        }
        env_reset = 0;          /* init is done */
    }

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
        face = Face::right;
        u = 0.5f * (1.0f + ref.z / absX);
        v = 0.5f * (1.0f - ref.y / absX);
    }
    else if (!isXPositive && absX >= absY && absX >= absZ)
    {
        face = Face::left;
        u = 0.5f * (1.0f - ref.z / absX);
        v = 0.5f * (1.0f - ref.y / absX);
    }
    else if (isYPositive && absY >= absX && absY >= absZ)
    {
        face = Face::top;
        u = 0.5f * (1.0f + ref.x / absY);
        v = 0.5f * (1.0f - ref.z / absY);
    }
    else if (!isYPositive && absY >= absX && absY >= absZ)
    {
        face = Face::bottom;
        u = 0.5f * (1.0f + ref.x / absY);
        v = 0.5f * (1.0f + ref.z / absY);
    }
    else if (isZPositive && absZ >= absX && absZ >= absY)
    {
        face = Face::back;
        u = 0.5f * (1.0f - ref.x / absZ);
        v = 0.5f * (1.0f - ref.y / absZ);
    }
    else if (!isZPositive && absZ >= absX && absZ >= absY) // front
    {
        face = Face::front;
        u = 0.5f * (1.0f + ref.x / absZ);
        v = 0.5f * (1.0f - ref.y / absZ);
    }
    else
    {
        fprintf(stderr, "environment mapping error\n");
        exit(-1);
    }

    if (u < 0 || u >= 1 || v < 0 || v >= 1)
    {
        fprintf(stderr, "environment mapping uv out of boundary\n");
        exit(-1);
    }

    int left = floorf(u * (env_xs[face] - 1));
    int right = left + 1;
    int top = floorf(v * (env_ys[face] - 1));
    int bottom = top + 1;

    float s = u * (env_xs[face] - 1) - left;
    float t = v * (env_ys[face] - 1) - top;

    return (1 - s) * (1 - t) * envs[face][left + top * env_xs[face]] + s * (1 - t) * envs[face][right + top * env_xs[face]] + (1 - s) * t * envs[face][left + bottom * env_xs[face]] + s * t * envs[face][right + bottom * env_xs[face]];
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

