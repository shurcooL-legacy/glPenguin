// glPenguin v0.0 by shurcooL (shurcooL@hotmail.com)

#pragma comment(lib, "glfw.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#pragma comment(linker, "/NODEFAULTLIB:\"LIBC\"")
#pragma comment(linker, "/NODEFAULTLIB:\"LIBCMT\"")

#include "Globals.h"

#include "CTimer.h"
#include "CCamera.h"
#include "CPlayer.h"
#include "CWorld.h"
#include "CConfig.h"

tPoint2		g_oWindowSize;
bool		g_bPaused = false;

void InitOpenGL()
{
	// set clear colour
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// perspective correction hint
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// front face = CW
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);
}

void GLFWCALL ProcessWindowResize(int nWidth, int nHeight)
{
	if (nHeight <= 0) nHeight = 1;

	g_oWindowSize.nX = nWidth;
	g_oWindowSize.nY = nHeight;

	glViewport(0, 0, g_oWindowSize.nX, g_oWindowSize.nY);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, static_cast<float>(g_oWindowSize.nX) / g_oWindowSize.nY, 1.0, 1500.0);
	//glOrtho(-160.0, 160.0, -120.0, 120.0, 1.0, 1500.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLFWCALL ProcessKey(int nKey, int nAction)
{
	if (nAction == GLFW_PRESS) {
		switch (nKey) {
		case 'C':
			CCamera::Get().ChangeStyle();

			break;
		case 'W':
			CCamera::Get().ToggleWireframe();

			break;
		case 'P':
			g_bPaused = !g_bPaused;

			if (g_bPaused) {
				glfwSetWindowTitle("glPenguin v0.0 - paused");
			} else {
				glfwSetWindowTitle("glPenguin v0.0");
			}

			break;
		default:
			if (!g_bPaused) CPlayer::ProcessKeyAll(nKey);

			break;
		}
	}
	else if (nAction == GLFW_RELEASE) {
	}
}

void GLFWCALL ProcessMouseWheel(int nMouseWheelPos)
{
	static int nLastMouseWheel = glfwGetMouseWheel();

	// change camera zoom
	CCamera::Get().Zoom((nMouseWheelPos - nLastMouseWheel) * -25);

	nLastMouseWheel = nMouseWheelPos;
}

int main()
{
	int			nError = 0;
	bool		bRunning = false;
	float		fFps, fFpsTime = 0.0; int nFpsFrames = 0;

	// Read config file
	CConfig* pConfig = new CConfig("Config.cfg");
	if (pConfig->IsBad()) MessageBox(NULL, "Configuration file 'Config.cfg' was not found in the root directory.\n\nUsing default values instead.", "glPenguin Warning", MB_OK | MB_ICONWARNING);

	if (pConfig->GetBoolParam(true, "ShowIntro", "General")) MessageBox(NULL, "Welcome to glPenguin v0.0!\n\nYour objective is to get to higher ground.\nIf you'd like to change the world size or game rules, please edit the 'Config.cfg' file in Notepad.\n\nTo move, use the arrow keys.\n\n'P' pauses/unpauses the game. 'W' toggles wireframe mode. 'C' cycles between two camera modes.\nUse the mouse to move the camera. If the game stops for some reason, read what it says in the title bar (only in windowed mode),\nas it's most likely you've either won or died.\n\nGood luck, and hope you enjoy this pre-release tech demo, while I work on some bug fixes, and soon enough, on actually *finishing* the game! :)", "glPenguin Information", MB_OK | MB_ICONINFORMATION);

	if (!glfwInit()) {
		nError = 1;
	} else if (!glfwOpenWindow(g_oWindowSize.nX = pConfig->GetIntParam(640, "ResolutionX", "Video"),
							   g_oWindowSize.nY = pConfig->GetIntParam(480, "ResolutionY", "Video"),
							   pConfig->GetIntParam(16, "ColourBits", "Video") / 3, pConfig->GetIntParam(16, "ColourBits", "Video") / 3, pConfig->GetIntParam(16, "ColourBits", "Video") / 3,
							   0, pConfig->GetIntParam(16, "DepthBits", "Video"), 0,
							   pConfig->GetBoolParam(false, "Fullscreen", "Video") ? GLFW_FULLSCREEN : GLFW_WINDOW)) {
		nError = 2;
	} else {
		GLFWvidmode oDesktopMode;
		glfwGetDesktopMode(&oDesktopMode);
		glfwSetWindowPos(oDesktopMode.Width * 0.5 - g_oWindowSize.nX * 0.5, oDesktopMode.Height * 0.5 - g_oWindowSize.nY * 0.5);
		glfwSetWindowTitle("glPenguin v0.0");
		glfwEnable(GLFW_STICKY_KEYS);		// When GLFW_STICKY_KEYS is enabled, keys which are pressed will not be released until they are physically released and checked with glfwGetKey

		glfwSetWindowSizeCallback(ProcessWindowResize);
		glfwSetKeyCallback(ProcessKey);
		glfwSetMouseWheelCallback(ProcessMouseWheel);

		InitOpenGL();

		std::srand((long)std::time(NULL));

		// create the world
		CWorld::Create(pConfig->GetIntParam(10, "WorldX", "World"),
					   pConfig->GetIntParam(10, "WorldY", "World"),
					   pConfig->GetIntParam(3, "WinHeight", "World"),
					   pConfig->GetFloatParam(0.5, "WorldTickTime", "World"),
					   pConfig->GetFloatParam(0.5, "IceCubeRespawnRate", "World"));

		// TODO: debug pre-set falling ice cubes, to test movement code
		if (pConfig->GetBoolParam(false, "TestFallingStairs", "General")) {
			CWorld::Get().SetIceCube(0, 0, 13, 1);
			CWorld::Get().SetIceCube(1, 0, 12, 1);
			CWorld::Get().SetIceCube(1, 1, 12, 1);
			CWorld::Get().SetIceCube(0, 1, 13, 1);
			CWorld::Get().SetIceCube(1, 0, 14, 1);
			CWorld::Get().SetIceCube(2, 0, 11, 1);
			CWorld::Get().SetIceCube(3, 0, 10, 1);
			CWorld::Get().SetIceCube(4, 0, 9, 1);

			CWorld::Get().SetIceCube(5, 0, 1, 2);
			CWorld::Get().SetIceCube(5, 0, 0, 2);
			CWorld::Get().SetIceCube(6, 0, 0, 2);
		}

		CPlayer::CreatePlayers(1);

		glfwSetMousePos((g_oWindowSize * 0.5).nX, (g_oWindowSize * 0.5).nY);

		CTimer::Get();		// Init the timer
	}

	std::stringstream x;
	x << "GLFW_ACCELERATED: " << glfwGetWindowParam(GLFW_ACCELERATED)
	  << "\nGLFW_RED_BITS: " << glfwGetWindowParam(GLFW_RED_BITS)
	  << "\nGLFW_GREEN_BITS: " << glfwGetWindowParam(GLFW_GREEN_BITS)
	  << "\nGLFW_BLUE_BITS: " << glfwGetWindowParam(GLFW_BLUE_BITS)
	  << "\nGLFW_ALPHA_BITS: " << glfwGetWindowParam(GLFW_ALPHA_BITS)
	  << "\nGLFW_DEPTH_BITS: " << glfwGetWindowParam(GLFW_DEPTH_BITS)
	  << "\nGLFW_STENCIL_BITS: " << glfwGetWindowParam(GLFW_STENCIL_BITS)
	  << "\nGLFW_REFRESH_RATE: " << glfwGetWindowParam(GLFW_REFRESH_RATE);
	MessageBox(NULL, x.str().c_str(), "MessageBox1", NULL);
	GLFWvidmode y[100];
	int z = glfwGetVideoModes(y, 100);
	std::ofstream f;
	f.open("vid modes.txt", std::ofstream::out | std::ofstream::trunc);
	for (int i = 0; i < z; ++i)
	{
		f << "res: " << y[i].Width << "x" << y[i].Height
		  << "@" << y[i].RedBits + y[i].GreenBits + y[i].BlueBits
		  << "\n";
	}
	f.close();

	if (!nError) {
		glfwDisable(GLFW_MOUSE_CURSOR);

		bRunning = true;
	}

	// Close the config file
	delete pConfig;

	while (bRunning)
	{
		// clear opengl buffers, reset the world matrix
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		// time calcs
		CTimer::Get().Update();

		// fps calcs
		++nFpsFrames;
		fFpsTime += CTimer::Get().GetTimePassed();
		if (fFpsTime >= 0.5) {
			fFps = nFpsFrames / fFpsTime;

			// convert it to a string
			std::stringstream oSStream;
			oSStream << fFps;

			if (!g_bPaused) {
				glfwSetWindowTitle((std::string("glPenguin v0.0 - ") + oSStream.str() + " fps").c_str());
			}

			nFpsFrames = 0;
			fFpsTime = 0.0;
		}

		// mouse calcs
		tPoint2 oMouseMoved;
		glfwGetMousePos(&oMouseMoved.nX, &oMouseMoved.nY);
		oMouseMoved = oMouseMoved - g_oWindowSize * 0.5;
		if (oMouseMoved.nX || oMouseMoved.nY) {
			glfwSetMousePos((g_oWindowSize * 0.5).nX, (g_oWindowSize * 0.5).nY);
		}

		// camera position & angle calcs
		CCamera::Get().ProcessMouse(oMouseMoved);

		// opengl world matrix translation
		CCamera::Get().Translate();

		// Scale up the scene
		glScalef(10.0, 10.0, 10.0);

		// physics
		if (!g_bPaused) CWorld::Get().Tick();

		// render the world
		CWorld::Get().Render();

		glFlush();
		glfwSwapBuffers();

		//glfwSleep(0.25); glfwPollEvents();

		bRunning = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

		glfwSleep(0.0);
	}

	glfwEnable(GLFW_MOUSE_CURSOR);

	CWorld::Destroy();
	CPlayer::DestroyPlayers();
	CCamera::Destroy();
	CTimer::Destroy();

	glfwTerminate();

	return nError;
}