#include "ReymentaMixnMapApp.h"

void ReymentaMixnMapApp::prepareSettings(Settings *settings)
{
	// instanciate the logger class
	log = Logger::create("MixnMap.txt");
	log->logTimedString("prepareSettings start");

	// parameters
	mParameterBag = ParameterBag::create();
	// if AutoLayout, try to position the window on the 2nd screen
	if (mParameterBag->mAutoLayout)
	{
		getWindowsResolution();
	}
#ifdef _DEBUG
	// debug mode
	settings->setWindowSize(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
	settings->setWindowPos(ivec2(mParameterBag->mRenderX, mParameterBag->mRenderY));
#else
	settings->setWindowSize(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
	settings->setWindowPos(ivec2(mParameterBag->mRenderX, mParameterBag->mRenderY));
	settings->setBorderless();
#endif  // _DEBUG
	settings->setResizable(false); // resize allowed for a receiver, but runtime error on the resize in the shaders drawing
	// set a high frame rate to disable limitation
	settings->setFrameRate(1000.0f);

	if (mParameterBag->mShowConsole) settings->enableConsoleWindow();
	log->logTimedString("prepareSettings done");
}
void ReymentaMixnMapApp::getWindowsResolution()
{
	log->logTimedString("getWindowsResolution");
	mParameterBag->mDisplayCount = 0;
	// Display sizes
	mParameterBag->mMainDisplayWidth = Display::getMainDisplay()->getWidth();
	mParameterBag->mMainDisplayHeight = Display::getMainDisplay()->getHeight();
	mParameterBag->mRenderY = 0;
	for (auto display : Display::getDisplays())
	{
		mParameterBag->mDisplayCount++;
		mParameterBag->mRenderWidth = display->getWidth();
		mParameterBag->mRenderHeight = display->getHeight();
		
		log->logTimedString("Window " + toString(mParameterBag->mDisplayCount) + ": " + toString(mParameterBag->mRenderWidth) + "x" + toString(mParameterBag->mRenderHeight));
	}
	log->logTimedString(" mRenderWidth" + toString(mParameterBag->mRenderWidth) + "mRenderHeight" + toString(mParameterBag->mRenderHeight));
	mParameterBag->mRenderResoXY = vec2(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);

	// in case only one screen , render from x = 0
	if (mParameterBag->mDisplayCount == 1) mParameterBag->mRenderX = 0; else mParameterBag->mRenderX = mParameterBag->mMainDisplayWidth;
}
void ReymentaMixnMapApp::setup()
{
	log->logTimedString("setup");

	// instanciate the Shaders class, must not be in prepareSettings
	mShaders = Shaders::create(mParameterBag);
	// instanciate the textures class
	mTextures = Textures::create(mParameterBag, mShaders);
	// instanciate the spout class
	mSpout = SpoutWrapper::create(mParameterBag, mTextures);
	// instanciate the Warps class
	mWarpings = WarpWrapper::create(mParameterBag, mTextures, mShaders);
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag, mShaders, mTextures, mWarpings);

	updateWindowTitle();
}

void ReymentaMixnMapApp::shutdown()
{
	// save warp settings
	mWarpings->save();
	// save params
	mParameterBag->save();
	// close ui and save settings
	mSpout->shutdown();
	mTextures->shutdown();
}

void ReymentaMixnMapApp::update()
{
	mParameterBag->iChannelTime[0] = getElapsedSeconds();
	mParameterBag->iChannelTime[1] = getElapsedSeconds() - 1;
	mParameterBag->iChannelTime[3] = getElapsedSeconds() - 2;
	mParameterBag->iChannelTime[4] = getElapsedSeconds() - 3;
	//
	if (mParameterBag->mUseTimeWithTempo)
	{
		mParameterBag->iGlobalTime = mParameterBag->iTempoTime*mParameterBag->iTimeFactor;
	}
	else
	{
		mParameterBag->iGlobalTime = getElapsedSeconds();
	}
	mOSC->update();
	//! update textures
	mTextures->update();
	//! update shaders (must be after the textures update)
	mShaders->update();
	
	mSpout->update();
	updateWindowTitle();
}

void ReymentaMixnMapApp::draw()
{
	// clear the window and set the drawing color to white
	gl::clear();
	gl::color(Color::white());

	// draw textures
	mSpout->draw();
	mTextures->draw();
	mWarpings->draw();

}

void ReymentaMixnMapApp::resize()
{
	mShaders->resize();
	// tell the warps our window has been resized, so they properly scale up or down
	mWarpings->resize();
}

void ReymentaMixnMapApp::mouseMove(MouseEvent event)
{
	mWarpings->mouseMove(event);
}

void ReymentaMixnMapApp::mouseDown(MouseEvent event)
{
	mWarpings->mouseDown(event);
}

void ReymentaMixnMapApp::mouseDrag(MouseEvent event)
{
	mWarpings->mouseDrag(event);
}

void ReymentaMixnMapApp::mouseUp(MouseEvent event)
{
	mWarpings->mouseUp(event);
}

void ReymentaMixnMapApp::keyDown(KeyEvent event)
{
	mWarpings->keyDown(event);
}

void ReymentaMixnMapApp::keyUp(KeyEvent event)
{
	mWarpings->keyUp(event);
}

void ReymentaMixnMapApp::updateWindowTitle()
{
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) Reymenta mix-n-map");
}

CINDER_APP_NATIVE(ReymentaMixnMapApp, RendererGl)
