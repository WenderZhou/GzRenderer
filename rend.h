#include	"gz.h"
#ifndef GZRENDER_
#define GZRENDER_


/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE	{0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */

#define	AAKERNEL_SIZE	6   // antialiasing kernel size

class GzRender{			/* define a renderer */

public:
	unsigned short	xres;
	unsigned short	yres;
	GzPixel		*pixelbuffer;		/* frame buffer array */
	char* framebuffer;

	GzCamera		m_camera;
	short		    matlevel;	        /* top of stack - current xform */
	GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
	GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
	GzMatrix		XnormInv;
	GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
	GzColor		flatColor;          /* color state for flat shaded triangles */
	int			interpMode;
	int			numlights;
	GzLight		lights[MAX_LIGHTS];
	GzLight		ambientlight;
	GzColor		Ka, Kd, Ks;
	float		    spec;		/* specular power */

	// Antialiasing
	float		xOffset;
	float		yOffset;
	float		weight;

  	// Constructors
	GzRender(int xRes, int yRes);
	~GzRender();

	// Function declaration

	// HW1: Display methods
	int GzDefault();
	int GzBeginRender();
	int GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);
	int GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth	*z);

	int GzFlushDisplay2File(FILE* outfile);
	int GzFlushDisplay2FrameBuffer();

	// HW2: Render methods
	int SetFlatColor(float3 flatColor);

	int SetInterpMode(int interpMode);

	int AddDirectionalLight(GzLight light);
	int AddAmbientLight(GzLight light);

	int SetAmbientCofficient(float3 Ka);
	int SetDiffuseCofficient(float3 Kd);
	int SetSpecularCofficient(float3 Ks);

	int SetDistributionCofficient(float spec);

	int SetAntiAliasingParameter(float xOffset, float yOffset, float weight);

	int GzPutTriangle(float3 vertices[], float3 normals[], float2 texCoords[], const std::string textureName);

	// HW3
	int GzPutCamera(GzCamera camera);
	int GzPushMatrix(GzMatrix	matrix);
	int GzPopMatrix();

	// Extra methods: NOT part of API - just for general assistance */
	inline int ARRAY(int x, int y){return (x+y*xres);}	/* simplify fbuf indexing */
	inline short	ctoi(float color) {return(short)((int)(color * ((1 << 12) - 1)));}		/* convert float color to GzIntensity short */

	bool isPointInsideTriangle(float x0, float y0, float x1, float y1, float x2, float y2, float x, float y);

	float GzInterpolate(float x0, float y0, float parameter0,float x1, float y1, float parameter1,float x2, float y2, float parameter2, float x, float y);
	GzColor GzShading(GzCoord normal, GzTextureIndex uv, const std::string textureName);
};
#endif