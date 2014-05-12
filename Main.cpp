#include "stdafx.h"
																	// WINDOW OPTIONS.

#define	WINWDTH				600												// Window width.
#define WINHGHT				800												// Window height.
#define WINTTL				"Flappy Bird v1.0"								// Window title.

																	// SCENE, PLAYER OPTIONS.

#define XSTPOSC				200												// Player start position by Ox.
#define YSTPOSC				500												// Player start position by Oy.
#define PLRSZ				40												// Player size.
#define SPD					3												// Player speed.
#define MAXSPD				-25.0f											// Max player vertical speed.
#define GRVT				0.8f											// Gravity power.
#define JMP					75												// Jump height.
#define JMPRT				6.9												// Jump value rate.
#define GRNDLVL				90												// Ground height.
#define SURFTHCKN			10												// Surface thickness.
#define SURFLVL				GRNDLVL + SURFTHCKN								// Surface height.
#define WLLSTRPOS			WINWDTH											// Walls start position.
#define WLLSTHCKN			2												// Walls thickness(in walls).
#define WLLSTP				2 * WLLSTHCKN									// Distance between walls(in walls).
#define WLLC				(WINWDTH / PLRSZ / WLLSTP) + 1					// Walls count.
#define HLSZRT				2.1f											// Hole size multiple rate.

																	// GLOBAL VARIABLES.

int		BEST,																// Player best scores.
		SCORES				= 0;											// Player scores.
bool	CHEAT				= false,										// Allow collisions.
		KEY					= false,										// If any key pressed.
		GAME				= false,										// If game goes.
		FAIL				= false,										// If player failed.
	   *ISLFT				= new bool[(int)WLLC];							// If wall at the left side of player.
float  *CNTR				= new float[(int)WLLC],							// Walls centers array.
	  **WLLS				= new float*[(int)WLLC],						// Walls array.
		YSTPOS				= YSTPOSC,										// Player start position by Oy.										
		YSPEED				= 0;											// Player vertical speed.
char	cBEST[3],															// Best scores string.
		cSCORES[3];															// Scores string.

																	// FUNCTIONS.

																			// Returns height of each lowest wall.											
GLfloat calculateHeight()
{
	return (SURFLVL + PLRSZ + rand() % (WINHGHT - (SURFLVL) - PLRSZ - 2 * JMP - JMP * (int)HLSZRT));
}
																			// Compare achieved scores with best.	
void	compareScores(char *scores)
{
	if(SCORES > BEST)																// If achieved more than best:
	{
		BEST = SCORES;																			// Best scores = achieved.
		ofstream file("best.txt", ios_base::out | ios_base::trunc);								// Open stream to clear file and write data.
		file.write(scores, 3);																	// Write scores.
		file.close();																			// Close stream.

		ifstream fileRead("best.txt");															// Open stream to read data.
		fileRead.read(cBEST, 3);																// Write best scores into cBEST.
	}
}
																			// Create resultsfile if need, or read file.
void	initReadBestFile()
{
	if(_access("best.txt", 0))														// If file not exists:
	{
		ofstream create("best.txt");														// Create file.
		create.write("000", 3);																// Write 000 into file.
		create.close();																		// Close stream.
		BEST = 0;																			// Set best scores to 0.
		memset(cBEST, '0', sizeof(cBEST));													// Set cBEST to 000.
	}
	else																			// Else:
	{
		ifstream file("best.txt");															// Open stream to read file.
		file.read(cBEST, 3);																// Read data.
		BEST = atoi(cBEST);																	// Set best scores to recorded.
		file.close();																		// Close stream.
	}
}
																			// Text output function.
void	displayText(char *scores, char *best)
{
	string sScores  = "SCORES: ",
		   sBest	= "BEST: ";
	glColor3f(1.0f, 1.0f, 1.0f);
	

	glRasterPos2f((WINWDTH / 2) - (sScores.size() * 11), GRNDLVL - 20);				// Position at center of screen.
	for(unsigned int i = 0; i < sScores.size(); ++i)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, sScores[i]);					// Writes "SCORES: ".

	char *c;
	glRasterPos2f((WINWDTH / 2) - (sScores.size() * 11) + 85, GRNDLVL - 20);		// Position from the right of "SCORES: ".
	for(c = scores; *c != '\0'; ++c)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);							// Writes scores.
	

	glRasterPos2f((WINWDTH / 2) - (sBest.size() * 11) + 6, GRNDLVL - 40);			// Position at center of screen under "SCORES: ".
	for(unsigned int i = 0; i < sBest.size(); ++i)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, sBest[i]);					// Writes "BEST: ".

	glRasterPos2f((WINWDTH / 2) - (sBest.size() * 11) + 63, GRNDLVL - 40);			// Position from the right of "BEST: ".
	for(c = best; *c != '\0'; ++c)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);							// Writes best scores.
}
																			// Initialize arrays if didn't, fill it with walls coordinates, reset scores.
void	initFillArrResetScores()
{
	SCORES = 0;																		// Reset scores.
	memset(cSCORES, '0', sizeof(cSCORES));											// Fill cScores with null.
	if(!FAIL)																		// If game not failed:
		for(int i = 0; i < WLLC; ++i)
		{
			WLLS[i]  = new float[3];														// Initialize array WLLS.
			ISLFT[i] = false;																// Set all elements from ISLFT to false.
		}
																					// Fill array with walls and walls centers coordinates:
	WLLS[0][0]		 = (GLfloat)(WLLSTRPOS);
	WLLS[0][1]		 = (GLfloat)(WLLS[0][0] + WLLSTHCKN * PLRSZ);
	WLLS[0][2]		 = (GLfloat)(calculateHeight());
	for(int i = 1; i < WLLC; ++i)
	{
		WLLS[i][0]	 = (GLfloat)(WLLS[i - 1][1] + WLLSTP * PLRSZ); 
		WLLS[i][1]	 = (GLfloat)(WLLS[i][0] + WLLSTHCKN * PLRSZ);
		WLLS[i][2]	 = (GLfloat)(calculateHeight());
	}
	for(int i = 0; i < WLLC; ++i)
		CNTR[i]		 = (GLfloat)(WLLS[i][0] + ((WLLSTHCKN * PLRSZ) / 2));
}
																			// Move walls and check for collisions.
void	moveWallsChkColl(float **WLLS, float *CNTR)
{
	for(int i = 0; i < WLLC; ++i)
	{																				// Move all walls and centers.
		WLLS[i][0]		 -= (GLfloat)SPD;
		WLLS[i][1]		 -= (GLfloat)SPD;
		CNTR[i]			 -= (GLfloat)SPD;
		if(WLLS[i][1] < 0)															// If wall reached left side of the screen:
		{																					// Move it behind last wall and recalculate height of hole in this wall.
			CNTR[i]		 += (GLfloat)((WLLC) * ((PLRSZ) * (WLLSTP + WLLSTHCKN)));
			WLLS[i][0]	 += (GLfloat)((WLLC) * ((PLRSZ) * (WLLSTP + WLLSTHCKN)));
			WLLS[i][1]    = (GLfloat)(WLLS[i][0] + PLRSZ * WLLSTHCKN);
			WLLS[i][2]    = (GLfloat)(calculateHeight());
			ISLFT[i]	  = false;															// Now wall at the right side of the player.
		}
																					// If player reached or within wall:
		if((XSTPOSC + PLRSZ > WLLS[i][0])	&& 
		   (XSTPOSC < WLLS[i][0]) 
		   ||
		   (XSTPOSC > WLLS[i][0])			&& 
		   (XSTPOSC + PLRSZ < WLLS[i][1]) 
		   ||
		   (XSTPOSC + PLRSZ > WLLS[i][1])	&& 
		   (XSTPOSC < WLLS[i][1]))
		   
		{
			if(CHEAT)																		// If cheat on:
			{
				if(!ISLFT[i])																		// If wall at the right side of the player:
				{
					if(XSTPOSC > CNTR[i])																		// If player reached center of wall:
					{
						ISLFT[i] = true;																				// Now wall at left side of player.
						++SCORES;																						// Increase scores.
					}
					sprintf(cSCORES, "%.3d", SCORES);															// Write scores in cScores.
					displayText(cSCORES, cBEST);
				}
			}
			else																			// If cheat off:
			{
																									// If player over or under hole:
				if((YSTPOS < WLLS[i][2]) ||		
				   (YSTPOS + PLRSZ > WLLS[i][2] + (JMP * HLSZRT)))
				{
						GAME = false;																		// Stop game.
						FAIL = true;																		// Game failed.
				}
				else																				// Else:
				{
					if(!ISLFT[i])																			// If wall at the right side of the player:
					{
						if(XSTPOSC > CNTR[i])																		// If player reached the center of wall:
						{
							ISLFT[i] = true;																				// Now wall at the left side of the player. 
							++SCORES;																						// Increase scores.
						}
						sprintf(cSCORES, "%.3d", SCORES);															// Write scores in cScores.
						displayText(cSCORES, cBEST);
					}
				}
			}
		}
	}
}
																			// Check collisions with surface and top, limit vertical speed.
void	chkPlayerPosLimSpeed()
{
	if(YSTPOS + PLRSZ > WINHGHT)													// If player flew over the top of scene:
		YSTPOS = WINHGHT - PLRSZ;															// Move him right to the top.
	else if(YSTPOS < SURFLVL)														// If player fell at ground:
		if(!CHEAT)																			// If cheat inactive:
		{
			GAME = false;																			// Stop game.
			FAIL = true;																			// Game failed.
		}
	if(YSPEED < MAXSPD)																// If vertical speed exceeded maximum:
		YSPEED = MAXSPD;																	// Set it to maximum.
}
																			// Main gaming function.
void	moving()
{
	if(GAME)																		// If game goes:
	{
		chkPlayerPosLimSpeed();																
		moveWallsChkColl(WLLS, CNTR);
		YSTPOS += YSPEED;																	// Move player by Oy with vertical speed.
		YSPEED -= GRVT;																		// Reduce vertical speed by gravity.
		glutPostRedisplay();																// Redraw scene.
	}
	if(FAIL)																		// If game failed:
		if(YSTPOS > SURFLVL)																// If player over the ground:
		{
			YSTPOS += MAXSPD;																	// Move player down with maximum vertical speed.
			glutPostRedisplay();																// Redraw scene.
		}
}
																			// Draw all scene objects.
void	renderScene()
{										
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);											// Sky color.
	glClear(GL_COLOR_BUFFER_BIT);													// Clear color buffer with sky color.

																					// WALLS.
	glColor3f(0.8f, 0.8f, 0.8f);															// Walls color.
	for(int i = 0; i < WLLC; ++i)
	{
																							// Lowest part.
		glRectf((GLfloat)(WLLS[i][0]),	(GLfloat)(SURFLVL), 
				(GLfloat)(WLLS[i][1]),	(GLfloat)(WLLS[i][2]));
																							// Highest part.
		glRectf((GLfloat)(WLLS[i][0]),	(GLfloat)(WLLS[i][2] + (JMP * HLSZRT)), 
				(GLfloat)(WLLS[i][1]),	(GLfloat)(WINHGHT));
	}

																					// SURFACE.
	glColor3f(0.8f, 0.8f, 0.8f);															// Surface color.
	glRectf(0.0f,						(GLfloat)(GRNDLVL),									// Drawing.
			(GLfloat)(WINWDTH),			(GLfloat)(SURFLVL));

																					// GROUND.
	glColor3f(0.1f, 0.1f, 0.1f);															// Ground color.
	glRectf(0.0f,						0.0f,												// Drawing
			(GLfloat)WINWDTH,			(GLfloat)GRNDLVL);

																					// PLAYER.
	glColor3f(0.7f, 0.7f, 1.0f);															// Player color.
	glRectf((GLfloat)(XSTPOSC),			(GLfloat)(YSTPOS + PLRSZ),							// Drawing.
			(GLfloat)(XSTPOSC + PLRSZ),	(GLfloat)(YSTPOS));
	displayText(cSCORES, cBEST);													// Display scores.
    glutSwapBuffers();																// Swap buffers.
}
																			// Handle resize of scene.
void	changeSize(int w, int h)
{
	glutReshapeWindow(WINWDTH, WINHGHT);											// Reshape window with default size.
	glViewport(0.0f, 0.0f, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, w, 0.0f, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
																			// Keypressing handle.
void	keysHook(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:																	// If Esc:
			exit (0);																		// Exit.
			break;													
		case VK_SPACE:																// If Space:
			if(GAME && !KEY)																// If game goes and no keys pressed:
			{
				YSPEED = JMP / JMPRT;																// Set vertical speed.
																				
			}
			else if(!GAME && !FAIL && !KEY)													// If game didn't start, player didn't fail, no keys pressed:
			{
				GAME = true;																		// Run game.
				glutIdleFunc(moving);																// Run main gaming function.
			}
			else if(!GAME && FAIL && !KEY)													// If game stopped, player failed, no keys pressed:
			{
				FAIL = false;																		// Fail state = false.
				YSTPOS = YSTPOSC;																	// Move player at standart height.
				YSPEED = 0;																			// Player vertical speed = 0;
				compareScores(cSCORES);																// Compare scores.
				initFillArrResetScores();															// Refill walls array, reset scores.
				glutPostRedisplay();																// Redraw scene.
			}		
			KEY = true;																		// Key state = pressed.
			break;
		case 'q':																	// If Q:
			CHEAT = !CHEAT;																	// Switch wallhack mode. Default is false.
			break;
	}
}
																			// Forbide permanent keydowns.
void	keyUp(unsigned char key, int x, int y)
{
	KEY = false;
}
																			// Start OGL code.
void	glInit(int argc, char* argv[])
{
	glutInit(&argc, argv);															// Initialize OGL.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);									// Set on double buffer, RGBA.
    glutInitWindowSize(WINWDTH, WINHGHT);											// Set window size.
	glutCreateWindow(WINTTL);														// Set window title and run it.
	glutDisplayFunc(renderScene);													// Render function.
	glutKeyboardFunc(keysHook);														// Keys hook function.
	glutKeyboardUpFunc(keyUp);														// Keys up function.
	glutReshapeFunc(changeSize);													// Resize function.
	glutMainLoop();																	// Main OGL loop.
}
																			// Entry point.
void	main(int argc, char* argv[])
{
	srand(time(NULL));																// Set randomize.
	initReadBestFile();																// Read best scores.
	initFillArrResetScores();														// Create walls arrays if not exists, fill it, reset scores.
	ShowWindow(GetConsoleWindow(), SW_HIDE);										// Hide console window.
    glInit(argc, argv);																// Initialize OGL.
}