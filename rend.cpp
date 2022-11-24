/* CS580 Homework 5 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI (float) 3.14159265358979323846

#define ROUGHNESS 0.5f

extern GzColor envtex_fun(GzCoord ref);
GzColor tex_fun(const std::string textureName, float u, float v);

float inline clamp(float val, float lowerBound, float upperBound)
{
	return max(lowerBound, min(upperBound, val));
}

void float3::normalize()
{
	*this = *this / sqrtf(x * x + y * y + z * z);
}

float3 operator+(float3 v1, float3 v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

float3 operator-(float3 v1, float3 v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

float3 operator*(float3 v1, float3 v2)
{
	return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

float3 operator*(float a, float3 v)
{
	return { a * v.x, a * v.y, a * v.z };
}

float3 operator*(float3 v, float a)
{
	return { a * v.x, a * v.y, a * v.z };
}

float3 operator/(float3 v, float a)
{
	return { v.x / a, v.y / a, v.z / a };
}

float3 operator-(float3 v)
{
	return { -v.x, -v.y, -v.z };
}

float dot(float3 v1, float3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float3 cross(float3 v1, float3 v2)
{
	return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

float3 normalize(float3 v)
{
	v.normalize();
	return v;
}

matrix::matrix()
{
	data[0][0] = 1.0f; data[0][1] = 0.0f; data[0][2] = 0.0f; data[0][3] = 0.0f;
	data[1][0] = 0.0f; data[1][1] = 1.0f; data[1][2] = 0.0f; data[1][3] = 0.0f;
	data[2][0] = 0.0f; data[2][1] = 0.0f; data[2][2] = 1.0f; data[2][3] = 0.0f;
	data[3][0] = 0.0f; data[3][1] = 0.0f; data[3][2] = 0.0f; data[3][3] = 1.0f;
}

matrix::matrix(
	float a00, float a01, float a02, float a03,
	float a10, float a11, float a12, float a13,
	float a20, float a21, float a22, float a23,
	float a30, float a31, float a32, float a33)
{
	data[0][0] = a00; data[0][1] = a01; data[0][2] = a02; data[0][3] = a03;
	data[1][0] = a10; data[1][1] = a11; data[1][2] = a12; data[1][3] = a13;
	data[2][0] = a20; data[2][1] = a21; data[2][2] = a22; data[2][3] = a23;
	data[3][0] = a30; data[3][1] = a31; data[3][2] = a32; data[3][3] = a33;
}

float* matrix::operator[](int row)
{
	return data[row];
}

matrix operator*(matrix m1, matrix m2)
{
	matrix m;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
	return m;
}

float det(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
{
	return m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m00 * m12 * m21 - m01 * m10 * m22 - m02 * m11 * m20;
}

matrix inverse(matrix m)
{
	matrix c;
	c[0][0] = +det(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
	c[0][1] = -det(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
	c[0][2] = +det(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
	c[0][3] = -det(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
	c[1][0] = -det(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
	c[1][1] = +det(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
	c[1][2] = -det(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
	c[1][3] = +det(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
	c[2][0] = +det(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
	c[2][1] = -det(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
	c[2][2] = +det(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
	c[2][3] = -det(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);
	c[3][0] = -det(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);
	c[3][1] = +det(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);
	c[3][2] = -det(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);
	c[3][3] = +det(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

	float D = m[0][0] * c[0][0] + m[0][1] * c[0][1] + m[0][2] * c[0][2] + m[0][3] * c[0][3];

	return matrix(
		c[0][0] / D, c[1][0] / D, c[2][0] / D, c[3][0] / D,
		c[0][1] / D, c[1][1] / D, c[2][1] / D, c[3][1] / D,
		c[0][2] / D, c[1][2] / D, c[2][2] / D, c[3][2] / D,
		c[0][3] / D, c[1][3] / D, c[2][3] / D, c[3][3] / D
	);
}

// Create rotate matrix : rotate along x axis
GzMatrix GzRotXMat(float degree)
{
	GzMatrix matrix;
	float theta = degree * PI / 180;
	matrix[1][1] = cosf(theta);
	matrix[1][2] = -sinf(theta);
	matrix[2][1] = sinf(theta);
	matrix[2][2] = cosf(theta);
	return matrix;
}

// Create rotate matrix : rotate along y axis
GzMatrix GzRotYMat(float degree)
{
	GzMatrix matrix;
	float theta = degree * PI / 180;
	matrix[0][0] = cosf(theta);
	matrix[0][2] = sinf(theta);
	matrix[2][0] = -sinf(theta);
	matrix[2][2] = cosf(theta);
	return matrix;
}

// Create rotate matrix : rotate along z axis
GzMatrix GzRotZMat(float degree)
{
	GzMatrix matrix;
	float theta = degree * PI / 180;
	matrix[0][0] = cosf(theta);
	matrix[0][1] = -sinf(theta);
	matrix[1][0] = sinf(theta);
	matrix[1][1] = cosf(theta);
	return matrix;
}

// Create translation matrix
GzMatrix GzTrxMat(GzCoord translate)
{
	GzMatrix matrix;
	matrix[0][3] = translate.x;
	matrix[1][3] = translate.y;
	matrix[2][3] = translate.z;
	return matrix;
}

// Create scaling matrix
GzMatrix GzScaleMat(GzCoord scale)
{
	GzMatrix matrix;
	matrix[0][0] = scale.x;
	matrix[1][1] = scale.y;
	matrix[2][2] = scale.z;
	return matrix;
}

GzRender::GzRender(int xRes, int yRes)
{
	// set resolution
	xres = xRes;
	yres = yRes;

	// allocate memory for pixel buffer and frame buffer
	pixelbuffer = new GzPixel[xRes * yRes];
	framebuffer = new char[3 * xRes * yRes];

	// set light
	numlights = 0;

	// set default parameter for rendering
	Ka = DEFAULT_AMBIENT;
	Kd = DEFAULT_DIFFUSE;
	Ks = DEFAULT_SPECULAR;


/* HW 3.6
- setup Xsp and anything only done once 
- init default camera 
*/ 
	Xsp[0][0] = xres / 2;	Xsp[0][1] = 0;			Xsp[0][2] = 0;			Xsp[0][3] = xres / 2;
	Xsp[1][0] = 0;			Xsp[1][1] = -yres / 2;	Xsp[1][2] = 0;			Xsp[1][3] = yres / 2;
	Xsp[2][0] = 0;			Xsp[2][1] = 0;			Xsp[2][2] = INT_MAX;	Xsp[2][3] = 0;
	Xsp[3][0] = 0;			Xsp[3][1] = 0;			Xsp[3][2] = 0;			Xsp[3][3] = 1;

	m_camera.position.x = DEFAULT_IM_X;
	m_camera.position.y = DEFAULT_IM_Y;
	m_camera.position.z = DEFAULT_IM_Z;
	m_camera.lookat.x = 0;
	m_camera.lookat.y = 0;
	m_camera.lookat.z = 0;
	m_camera.worldup.x = 0;
	m_camera.worldup.y = 1;
	m_camera.worldup.z = 0;
	m_camera.FOV = DEFAULT_FOV;
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	delete pixelbuffer;
	delete framebuffer;
}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	for (int j = 0; j < yres; j++)
		for (int i = 0; i < xres; i++)
		{
			pixelbuffer[i + j * xres].blue = 0x60 << 4;
			pixelbuffer[i + j * xres].green = 0x70 << 4;
			pixelbuffer[i + j * xres].red = 0x80 << 4;
			pixelbuffer[i + j * xres].z = INT_MAX;
		}
	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
/* HW 3.7 
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	// build Xpi
	float t = tanf(m_camera.FOV / 2 * PI / 180);
	
	m_camera.Xpi =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, t,    0.0f,
		0.0f, 0.0f, t,    1.0f
	};

	// build Xiw
	GzCoord cameraZ = normalize(m_camera.lookat - m_camera.position);
	GzCoord cameraY = normalize(m_camera.worldup - dot(m_camera.worldup, cameraZ) * cameraZ);// NOTE: Z == worldup is not checked
	GzCoord cameraX = cross(cameraY, cameraZ);

	m_camera.Xiw =
	{
		cameraX.x, cameraX.y, cameraX.z, -dot(cameraX, m_camera.position),
		cameraY.x, cameraY.y, cameraY.z, -dot(cameraY, m_camera.position),
		cameraZ.x, cameraZ.y, cameraZ.z, -dot(cameraZ, m_camera.position),
		0.0f,      0.0f,      0.0f,      1.0f
	};

	// clear matrix stack
	matlevel = 0;

	Ximage[0] = GzMatrix();
	Xnorm[0] = GzMatrix();

	GzPushMatrix(Xsp);
	GzPushMatrix(m_camera.Xpi);
	GzPushMatrix(m_camera.Xiw);

	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
/* HW 3.8 
/*- overwrite renderer camera structure with new camera definition
*/
	m_camera.position = camera.position;
	m_camera.lookat = camera.lookat;
	m_camera.worldup = camera.worldup;
	m_camera.FOV = camera.FOV;

	return GZ_SUCCESS;
}


// push a matrix onto the Ximage stack
int GzRender::GzPushMatrix(GzMatrix	matrix)
{
	if (matlevel + 1 >= MATLEVELS)
		return GZ_FAILURE;

	// push x form
	Ximage[matlevel + 1] = Ximage[matlevel] * matrix;
			
	// push x norm
	if(matlevel >= 2)
	{
		matrix[0][3] = 0.0f;	matrix[1][3] = 0.0f;	matrix[2][3] = 0.0f;	matrix[3][3] = 1.0f;

		float K = sqrtf(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matrix[i][j] = matrix[i][j] / K;

		Xnorm[matlevel + 1] = Xnorm[matlevel] * matrix;
		XnormInv = inverse(Xnorm[matlevel + 1]);
	}
	else
		Xnorm[matlevel + 1] = Xnorm[matlevel];

	matlevel++;

	return GZ_SUCCESS;
}


int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	matlevel--;
	XnormInv = inverse(Xnorm[matlevel]);

	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres)
	{
		GzPixel& pixel = pixelbuffer[i + j * xres];

		if (z < pixel.z)
		{
			pixel.red = clamp(r, 0, 4095);
			pixel.green = clamp(g, 0, 4095);
			pixel.blue = clamp(b, 0, 4095);
			pixel.alpha = a;
			pixel.z = z;
		}
		return GZ_SUCCESS;
	}
	else
		return GZ_FAILURE;
}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres)
	{
		GzPixel& pixel = pixelbuffer[i + j * xres];
		*r = pixel.red;
		*g = pixel.green;
		*b = pixel.blue;
		*a = pixel.alpha;
		*z = pixel.z;
		return GZ_SUCCESS;
	}
	else
		return GZ_FAILURE;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\r", xres, yres);
	for (int j = 0; j < yres; j++)
		for (int i = 0; i < xres; i++)
		{
			GzPixel& pixel = pixelbuffer[i + j * xres];
			fprintf(outfile, "%c%c%c", pixel.red >> 4, pixel.green >> 4, pixel.blue >> 4);
		}
	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	for (int j = 0; j < yres; j++)
		for (int i = 0; i < xres; i++)
		{
			GzPixel& pixel = pixelbuffer[i + j * xres];
			framebuffer[3 * (i + j * xres) + 0] = pixel.blue >> 4;
			framebuffer[3 * (i + j * xres) + 1] = pixel.green >> 4;
			framebuffer[3 * (i + j * xres) + 2] = pixel.red >> 4;
		}
	return GZ_SUCCESS;
}

int GzRender::SetFlatColor(float3 flatColor)
{
	this->flatColor = flatColor;
	return GZ_SUCCESS;
}

int GzRender::SetInterpMode(int interpMode)
{
	this->interpMode = interpMode;
	return GZ_SUCCESS;
}

int GzRender::AddDirectionalLight(GzLight light)
{
	lights[numlights++] = light;
	return GZ_SUCCESS;
}

int GzRender::AddAmbientLight(GzLight light)
{
	ambientlight = light;
	return GZ_SUCCESS;
}

int GzRender::SetAmbientCofficient(float3 Ka)
{
	this->Ka = Ka;
	return GZ_SUCCESS;
}

int GzRender::SetDiffuseCofficient(float3 Kd)
{
	this->Kd = Kd;
	return GZ_SUCCESS;
}

int GzRender::SetSpecularCofficient(float3 Ks)
{
	this->Ks = Ks;
	return GZ_SUCCESS;
}

int GzRender::SetDistributionCofficient(float spec)
{
	this->spec = spec;
	return GZ_SUCCESS;
}

int GzRender::SetAntiAliasingParameter(float xOffset, float yOffset, float weight)
{
	this->xOffset = xOffset;
	this->yOffset = yOffset;
	this->weight = weight;
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(float3 vertices[], float3 normals[], float2 texCoords[], const std::string textureName)
{
	for (int i = 0; i < 3; i++)
		normals[i].normalize();

	if (vertices == nullptr || normals == nullptr || texCoords == nullptr)
		return GZ_FAILURE;

	GzMatrix& xforms = Ximage[matlevel];

	float x[3], y[3], z[3];
	for (int j = 0; j < 3; j++)
	{
		float w = xforms[3][0] * vertices[j].x + xforms[3][1] * vertices[j].y + xforms[3][2] * vertices[j].z + xforms[3][3];

		x[j] = (xforms[0][0] * vertices[j].x + xforms[0][1] * vertices[j].y + xforms[0][2] * vertices[j].z + xforms[0][3]) / w;
		y[j] = (xforms[1][0] * vertices[j].x + xforms[1][1] * vertices[j].y + xforms[1][2] * vertices[j].z + xforms[1][3]) / w;

		z[j] = xforms[2][0] * vertices[j].x + xforms[2][1] * vertices[j].y + xforms[2][2] * vertices[j].z + xforms[2][3];

		if (z[j] < 0)	// discard triangle with negative z
			return GZ_SUCCESS;
		else
			z[j] = z[j] / w;
	}

	int xMin = floorf(min(min(x[0], x[1]), x[2]));
	int yMin = floorf(min(min(y[0], y[1]), y[2]));
	int xMax = ceilf(max(max(x[0], x[1]), x[2]));
	int yMax = ceilf(max(max(y[0], y[1]), y[2]));

	
	for (int j = yMin; j <= yMax; j++)
	{
		for (int i = xMin; i <= xMax; i++)
		{
			float sampleX = i + xOffset;
			float sampleY = j + yOffset;

			if (isPointInsideTriangle(x[0], y[0], x[1], y[1], x[2], y[2], sampleX, sampleY))
			{
				float zInterp = GzInterpolate(x[0], y[0], z[0], x[1], y[1], z[1], x[2], y[2], z[2], sampleX, sampleY);

				GzCoord normal = 
				{
					GzInterpolate(x[0], y[0], normals[0].x, x[1], y[1], normals[1].x, x[2], y[2], normals[2].x, sampleX, sampleY),
					GzInterpolate(x[0], y[0], normals[0].y, x[1], y[1], normals[1].y, x[2], y[2], normals[2].y, sampleX, sampleY),
					GzInterpolate(x[0], y[0], normals[0].z, x[1], y[1], normals[1].z, x[2], y[2], normals[2].z, sampleX, sampleY)
				};

				float factor[3] = { 1 + z[0] / (INT_MAX - z[0]), 1 + z[1] / (INT_MAX - z[1]), 1 + z[2] / (INT_MAX - z[2]) };

				GzTextureIndex uv_p = 
				{
					GzInterpolate(x[0], y[0], texCoords[0].u / factor[0], x[1], y[1], texCoords[1].u / factor[1], x[2], y[2], texCoords[2].u / factor[2], sampleX, sampleY),
					GzInterpolate(x[0], y[0], texCoords[0].v / factor[0], x[1], y[1], texCoords[1].v / factor[1], x[2], y[2], texCoords[2].v / factor[2], sampleX, sampleY)
				};

				GzTextureIndex uvInterp = { uv_p.u * (1 + zInterp / (INT_MAX - zInterp)), uv_p.v * (1 + zInterp / (INT_MAX - zInterp)) };

				GzColor color;
				if (textureName.length() > 0)
					color = tex_fun(textureName, uvInterp.u, uvInterp.v);
				else
					color = GzShading(normal, uvInterp, textureName);

				GzPut(i, j, ctoi(color.r), ctoi(color.g), ctoi(color.b), 1, zInterp);
			}
		}
	}

	return GZ_SUCCESS;
}

bool GzRender::isPointInsideTriangle(float x0, float y0, float x1, float y1, float x2, float y2, float x, float y)
{
	float A0 = y1 - y0;
	float A1 = y2 - y1;
	float A2 = y0 - y2;

	float B0 = x0 - x1;
	float B1 = x1 - x2;
	float B2 = x2 - x0;

	float C0 = -y0 * B0 - x0 * A0;
	float C1 = -y1 * B1 - x1 * A1;
	float C2 = -y2 * B2 - x2 * A2;

	float E0 = A0 * x + B0 * y + C0;
	float E1 = A1 * x + B1 * y + C1;
	float E2 = A2 * x + B2 * y + C2;

	return E0 * E1 >= 0 && E0 * E2 >= 0;
}

float GzRender::GzInterpolate(
	float x0, float y0, float parameter0,
	float x1, float y1, float parameter1,
	float x2, float y2, float parameter2,
	float x, float y)
{
	// For Z Interpolation
	GzCoord e1 = { x1 - x0, y1 - y0, parameter1 - parameter0 };
	GzCoord e2 = { x2 - x1, y2 - y1, parameter2 - parameter1 };

	float A = e1.y * e2.z - e1.z * e2.y;
	float B = e1.z * e2.x - e1.x * e2.z;
	float C = e1.x * e2.y - e1.y * e2.x;
	float D = -(A * x0 + B * y0 + C * parameter0);

	return -(A * x + B * y + D) / C;
}

float NDF(GzCoord N, GzCoord h)
{
	float Ndoth = max(dot(N, h), 0.0f);
	float Ndoth2 = Ndoth * Ndoth;
	float a2 = ROUGHNESS * ROUGHNESS;
	float denom = Ndoth2 * (a2 - 1.0f) + 1.0f;
	return a2 / (PI * denom * denom);
}

float3 fresnelSchlick(GzCoord h, GzCoord v, float3 F0)
{
	return F0 + (float3{ 1.0f, 1.0f, 1.0f } - F0) * powf(1 - dot(h, v), 5);
}

float Geometry(GzCoord N, GzCoord S)
{
	float NdotS = max(dot(N, S), 0.0f);
	float k = ROUGHNESS / 2.0f;
	return NdotS / (NdotS * (1 - k) + k);
}

float GeometrySmith(GzCoord N, GzCoord E, GzCoord L)
{
	return Geometry(N, E) * Geometry(N, L);
}

const GzColor F0_Titanium = { 194 / 255.0f, 187 / 255.0f, 179 / 255.0f };
const GzColor F0_Chromium = { 196 / 255.0f, 197 / 255.0f, 196 / 255.0f };
const GzColor F0_Iron = { 198 / 255.0f, 198 / 255.0f, 200 / 255.0f };
const GzColor F0_Nickel = { 212 / 255.0f, 205 / 255.0f, 192 / 255.0f };
const GzColor F0_Platinum = { 214 / 255.0f, 209 / 255.0f, 201 / 255.0f };
const GzColor F0_Copper = { 250 / 255.0f, 209 / 255.0f, 194 / 255.0f };
const GzColor F0_Palladium = { 222 / 255.0f, 217 / 255.0f, 211 / 255.0f };
const GzColor F0_Mercury = { 229 / 255.0f, 228 / 255.0f, 228 / 255.0f };
const GzColor F0_Brass = { 245 / 255.0f, 228 / 255.0f, 174 / 255.0f };
const GzColor F0_Zinc = { 213 / 255.0f, 234 / 255.0f, 237 / 255.0f };
const GzColor F0_Gold = { 255 / 255.0f, 229 / 255.0f, 158 / 255.0f };
const GzColor F0_Aluminum = { 245 / 255.0f, 246 / 255.0f, 246 / 255.0f };
const GzColor F0_Silver = { 252 / 255.0f, 250 / 255.0f, 245 / 255.0f };

GzColor GzRender::GzShading(GzCoord normal, GzTextureIndex uv, const std::string textureName)
{
	GzColor ambient = { 0.0f, 0.0f, 0.0f };
	GzColor diffuse = { 0.0f, 0.0f, 0.0f };
	GzColor specular = { 0.0f, 0.0f, 0.0f };

	GzColor K_a = Ka;
	GzColor K_d = Kd;
	GzColor K_s = Ks;

	ambient = ambientlight.color * K_a;

	GzMatrix& xnorm = Xnorm[matlevel];

	GzCoord N =
	{
		xnorm[0][0] * normal.x + xnorm[0][1] * normal.y + xnorm[0][2] * normal.z,
		xnorm[1][0] * normal.x + xnorm[1][1] * normal.y + xnorm[1][2] * normal.z,
		xnorm[2][0] * normal.x + xnorm[2][1] * normal.y + xnorm[2][2] * normal.z
	};

	N.normalize();

	GzCoord E = { 0.0f, 0.0f, -1.0f };

	for (int i = 0; i < numlights; i++)
	{
		GzCoord L = lights[i].direction;

		GzCoord h = normalize(L + E);

		GzCoord F = fresnelSchlick(h, E, F0_Silver);

		float D = NDF(N, h);
		float G = GeometrySmith(N, E, L);

		float NdotL = dot(N, L);
		float NdotE = dot(N, E);

		if (NdotL < 0 && NdotE < 0)
			N = -N;

		if (NdotL * NdotE > 0)
		{
			NdotL = dot(N, L);

			GzCoord R = normalize(2 * NdotL * N - L);
			
			diffuse = diffuse + NdotL * K_d * lights[i].color;

			float RdotE = max(0.0f, -R.z);

			//specular = specular + G * D * F * lights[i].color;

			specular = specular + K_s * powf(dot(R,E), spec) * lights[i].color;
		}
	}

	float3 color;
	color.r = min(1.0f, ambient.r + diffuse.r + specular.r);
	color.g = min(1.0f, ambient.g + diffuse.g + specular.g);
	color.b = min(1.0f, ambient.b + diffuse.b + specular.b);

	float NdotE = dot(N, E);
	if (NdotE < 0)
	{
		N = -N;
		NdotE = -NdotE;
	}

	GzCoord ref = 2 * NdotE * N - E;

	GzCoord ref_world =
	{
		XnormInv[0][0] * ref.x + XnormInv[0][1] * ref.y + XnormInv[0][2] * ref.z,
		XnormInv[1][0] * ref.x + XnormInv[1][1] * ref.y + XnormInv[1][2] * ref.z,
		XnormInv[2][0] * ref.x + XnormInv[2][1] * ref.y + XnormInv[2][2] * ref.z
	};

	GzMatrix I = xnorm * XnormInv;

	ref_world.normalize();

	color = envtex_fun(ref_world);

	/*color.r = min(1.0f, ambient.r + diffuse.r + specular.r);
	color.g = min(1.0f, ambient.g + diffuse.g + specular.g);
	color.b = min(1.0f, ambient.b + diffuse.b + specular.b);*/

	return color;
}