/*
 * Gz.h - include file for the cs580 rendering library
 */
#ifndef GZ_H
#define GZ_H

/*
 * universal constants
 */
#define GZ_SUCCESS      0
#define GZ_FAILURE      1

/*
 * value-list attributes
 */

/* select interpolation mode of the shader */
#define GZ_FLAT			0	/* do flat shading with GZ_RBG_COLOR */
#define	GZ_COLOR		1	/* interpolate vertex color */
#define	GZ_NORMALS		2	/* interpolate normals */

struct float3
{
	union { float x; float r; };
	union { float y; float g; };
	union { float z; float b; };

	void normalize();
};

struct float2
{
	union { float x; float u; };
	union { float y; float v; };
};

float3 operator+(float3 v1, float3 v2);
float3 operator-(float3 v1, float3 v2);
float3 operator*(float3 v1, float3 v2);
float3 operator*(float a, float3 v);
float3 operator*(float3 v, float a);
float3 operator/(float3 v, float a);

float3 operator-(float3 v);

float dot(float3 v1, float3 v2);
float3 cross(float3 v1, float3 v2);
float3 normalize(float3 v);

struct matrix
{
	float data[4][4];

	matrix();
	matrix(
		float a00, float a01, float a02, float a03,
		float a10, float a11, float a12, float a13,
		float a20, float a21, float a22, float a23,
		float a30, float a31, float a32, float a33);
	float* operator[](int row);
};

matrix operator*(matrix m1, matrix m2);

typedef float3 GzColor;
typedef short  GzIntensity;	/* 0-4095 in lower 12-bits for RGBA */
typedef float3 GzCoord;
typedef float2 GzTextureIndex;
typedef matrix GzMatrix;
typedef int	GzDepth;		/* signed z for clipping */

// Object Translation
GzMatrix GzRotXMat(float degree);
GzMatrix GzRotYMat(float degree);
GzMatrix GzRotZMat(float degree);
GzMatrix GzTrxMat(GzCoord translate);
GzMatrix GzScaleMat(GzCoord scale);

typedef	int	(*GzTexture)(float u, float v, GzColor color);	/* pointer to texture lookup method */
/* u,v parameters [0,1] are defined tex_fun(float u, float v, GzColor color) */

struct GzCamera
{
	GzMatrix			Xiw;  	/* xform from world to image space */
	GzMatrix			Xpi;     /* perspective projection xform */
	GzCoord			position;  /* position of image plane origin */
	GzCoord			lookat;         /* position of look-at-point */
	GzCoord			worldup;   /* world up-vector (almost screen up) */
	float				FOV;            /* horizontal field of view */
};

struct GzLight
{
	GzCoord        direction;    /* vector from surface to light */
	GzColor        color;		/* light color intensity */
};

struct GzInput
{
	GzCoord         rotation;       /* object rotation */
	GzCoord			translation;	/* object translation */
	GzCoord			scale;			/* object scaling */
	GzCamera		camera;			/* camera */
};

struct GzPixel{
	GzIntensity    red;	
	GzIntensity    green;
	GzIntensity    blue;
	GzIntensity    alpha;
	GzDepth	 z;
};

#define	MAXXRES	1024	/* put some bounds on size in case of error */
#define	MAXYRES	1024

#endif
