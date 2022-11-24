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

	m_pRender = new GzRender(m_nWidth, m_nHeight);
	m_pRender->GzDefault();

	m_pFrameBuffer = m_pRender->framebuffer; 

#if 1 	/* set up app-defined camera if desired, else use camera defaults */
	//camera.position = { 13.2f, -8.7f, -13.8f };
	camera.position = { 0.0f, 1.0f, 5.0f };
	//camera.lookat = { 0.8f, 0.7f, 4.5f };
	camera.lookat = { 0.0f, 1.0f, 0.0f };
	camera.worldup = { 0.0f, 1.0f, 0.0f };
    camera.FOV = 53.7;              /* degrees */

	status |= m_pRender->GzPutCamera(camera); 
#endif 

	/* Start Renderer */
	status |= m_pRender->GzBeginRender();

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

	 
	// renderer is ready for frame --- define lights and shader at start of frame 

	//m_pRender->SetInterpMode(GZ_COLOR);
	m_pRender->SetInterpMode(GZ_NORMALS);
    
	m_pRender->AddDirectionalLight(light1);
	m_pRender->AddDirectionalLight(light2);
	m_pRender->AddDirectionalLight(light3);

	m_pRender->AddAmbientLight(ambientlight);

	m_pRender->SetAmbientCofficient(ambientCoefficient);
	m_pRender->SetDiffuseCofficient(diffuseCoefficient);
	m_pRender->SetSpecularCofficient(specularCoefficient);
	m_pRender->SetDistributionCofficient(32);

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
	status |= m_pRender->GzDefault();  /* init for new frame */
	
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

		if(dummy[3] == '0')
			m_pRender->GzPutTriangle(vertices, normals, texCoords, "left");
		else if(dummy[3] == '1')
			m_pRender->GzPutTriangle(vertices, normals, texCoords, "right");
		else if (dummy[3] == '2')
			m_pRender->GzPutTriangle(vertices, normals, texCoords, "bottom");
		else if (dummy[3] == '3')
			m_pRender->GzPutTriangle(vertices, normals, texCoords, "top");
		else if (dummy[3] == '4')
			m_pRender->GzPutTriangle(vertices, normals, texCoords, "front");
		else if (dummy[3] == '5')
			m_pRender->GzPutTriangle(vertices, normals, texCoords, "back");
		else
			m_pRender->GzPutTriangle(vertices, normals, texCoords, "");
	} 

	m_pRender->GzFlushDisplay2File(outfile); 	/* write out or update display to file*/
	m_pRender->GzFlushDisplay2FrameBuffer();	// write out or update display to frame buffer

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

	delete(m_pRender);
	status |= GzFreeTexture();
	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}



