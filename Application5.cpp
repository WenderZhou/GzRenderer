// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #5
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application5.h"
#include "Gz.h"
#include "rend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INFILE  "pot.asc"
#define OUTFILE "output.ppm"

extern void initTexture();
extern int GzFreeTexture();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application5::Application5()
{

}

Application5::~Application5()
{
	Clean();
}

int Application5::Initialize()
{
	GzCamera	camera;  
	int		    xRes, yRes;	/* display parameters */ 

	int			shaderType, interpStyle;
	float		specpower;
	int		status; 
 
	status = 0; 

	/* 
	 * Allocate memory for user input
	 */
	m_pUserInput = new GzInput;

	/* 
	 * initialize the display and the renderer 
	 */ 
 	m_nWidth = 256;		// frame buffer and display width
	m_nHeight = 256;    // frame buffer and display height

	for (int k = 0; k < AAKERNEL_SIZE + 1; k++)
	{
		m_pRender[k] = new GzRender(m_nWidth, m_nHeight);
	}

#if 1 	/* set up app-defined camera if desired, else use camera defaults */
	camera.position = { 0.0f, 1.0f, 5.0f };
	camera.lookat = { 0.0f, 1.0f, 0.0f };
	camera.worldup = { 0.0f, 1.0f, 0.0f };
    camera.FOV = 53.7;              /* degrees */

	for (int k = 0; k < AAKERNEL_SIZE; k++)
	{
		status |= m_pRender[k]->GzPutCamera(camera);
	}
#endif 
	/* Light */
	GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.6, 0.6, 0.6} };
	GzLight	light2 = { {0, -0.7071, -0.7071}, {0.6, 0.6, 0.6} };
	GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.6, 0.6, 0.6} };

	/*GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
	GzLight	light2 = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
	GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };*/

	GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = { 0.7, 0.7, 0.7 };

	float AAFilter[AAKERNEL_SIZE][3]
	{
		-0.52f, 0.38f, 0.128f,
		0.41f, 0.56f, 0.119f,
		0.27f, 0.08f, 0.294f,
		-0.17f, -0.29f, 0.249f,
		0.58f, -0.55f, 0.104f,
		-0.31f, -0.71f, 0.106f
	};

	for (int k = 0; k < AAKERNEL_SIZE; k++)
	{
		status |= m_pRender[k]->GzBeginRender();
	
		m_pRender[k]->SetInterpMode(GZ_NORMALS);
    
		m_pRender[k]->AddDirectionalLight(light1);
		m_pRender[k]->AddDirectionalLight(light2);
		m_pRender[k]->AddDirectionalLight(light3);

		m_pRender[k]->AddAmbientLight(ambientlight);

		m_pRender[k]->SetAmbientCofficient(ambientCoefficient);
		m_pRender[k]->SetDiffuseCofficient(diffuseCoefficient);
		m_pRender[k]->SetSpecularCofficient(specularCoefficient);
		m_pRender[k]->SetDistributionCofficient(32);

		m_pRender[k]->SetAntiAliasingParameter(AAFilter[k][0], AAFilter[k][1], AAFilter[k][2]);
	}

	initTexture();

	if (status) exit(GZ_FAILURE); 

	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Render() 
{
	GzCoord		vertices[3];	/* vertex position coordinates */ 
	GzCoord		normals[3];	/* vertex normals */ 
	GzTextureIndex  	texCoords[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int			status = GZ_SUCCESS; 

	/* Initialize Display */
	for (int k = 0; k < AAKERNEL_SIZE; k++)
	{
		status |= m_pRender[k]->GzDefault();  /* init for new frame */
	}
	
	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
		 return GZ_FAILURE;
	}

	/* 
	* Walk through the list of triangles, set color 
	* and render each triangle 
	*/ 
	while( fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertices[0].x), &(vertices[0].y),  
		&(vertices[0].z), 
		&(normals[0].x), &(normals[0].y), 	
		&(normals[0].z), 
		&(texCoords[0].u), &(texCoords[0].v) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertices[1].x), &(vertices[1].y), 	
		&(vertices[1].z), 
		&(normals[1].x), &(normals[1].y), 	
		&(normals[1].z), 
		&(texCoords[1].u), &(texCoords[1].v) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertices[2].x), &(vertices[2].y), 	
		&(vertices[2].z), 
		&(normals[2].x), &(normals[2].y), 	
		&(normals[2].z), 
		&(texCoords[2].u), &(texCoords[2].v) ); 

		std::string textureName;

		if(dummy[3] == '0')
			textureName = "left";
		else if(dummy[3] == '1')
			textureName = "right";
		else if (dummy[3] == '2')
			textureName = "bottom";
		else if (dummy[3] == '3')
			textureName = "top";
		else if (dummy[3] == '4')
			textureName = "front";
		else if (dummy[3] == '5')
			textureName = "back";
		else
			textureName = "";

		for (int k = 0; k < AAKERNEL_SIZE; k++)
			m_pRender[k]->GzPutTriangle(vertices, normals, texCoords, textureName);
	} 

	// store result to render 0
	for (int j = 0; j < m_nHeight; j++)
	{
		for (int i = 0; i < m_nWidth; i++)
		{
			GzPixel result = { 0,0,0,0,0 };
			for (int k = 0; k < AAKERNEL_SIZE; k++)
			{
				GzPixel& pixel = m_pRender[k]->pixelbuffer[i + j * m_nWidth];
				result.red += pixel.red * m_pRender[k]->weight;
				result.green += pixel.green * m_pRender[k]->weight;
				result.blue += pixel.blue * m_pRender[k]->weight;
			}
			m_pRender[AAKERNEL_SIZE]->pixelbuffer[i + j * m_nWidth] = result;
		}
	}

	m_pFrameBuffer = m_pRender[AAKERNEL_SIZE]->framebuffer;

	m_pRender[AAKERNEL_SIZE]->GzFlushDisplay2File(outfile); 	/* write out or update display to file*/
	m_pRender[AAKERNEL_SIZE]->GzFlushDisplay2FrameBuffer();	// write out or update display to frame buffer

	/* 
	 * Close file
	 */ 

	if( fclose( infile ) )
      AfxMessageBox(_T( "The input file was not closed\n" ));

	if( fclose( outfile ) )
      AfxMessageBox(_T( "The output file was not closed\n" ));
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Clean()
{
	/* 
	 * Clean up and exit 
	 */ 
	int	status = 0; 

	for (int k = 0; k < AAKERNEL_SIZE + 1; k++)
		delete(m_pRender[k]);

	status |= GzFreeTexture();
	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}



