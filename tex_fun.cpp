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

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

