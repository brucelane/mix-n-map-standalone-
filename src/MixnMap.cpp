#include "MixnMap.h"

void MixnMap::prepareSettings(Settings *settings)
{
	// instanciate the logger class
	log = Logger::create("MixnMap.txt");
	log->logTimedString("prepareSettings start");

	// parameters
	mParameterBag = ParameterBag::create();
	// utils
	mBatchass = Batchass::create(mParameterBag);
	// if AutoLayout, try to position the window on the 2nd screen
	if (mParameterBag->mAutoLayout)
	{
		mBatchass->getWindowsResolution();
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

	log->logTimedString("prepareSettings done");
}

void MixnMap::setup()
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

void MixnMap::shutdown()
{
	// save warp settings
	mWarpings->save();
	// save params
	mParameterBag->save();
	// close ui and save settings
	mSpout->shutdown();
}

void MixnMap::update()
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

void MixnMap::draw()
{
	// clear the window and set the drawing color to white
	gl::clear();
	gl::color(Color::white());

	// draw textures
	mSpout->draw();
	mTextures->draw();
	mWarpings->draw();
	// Show the user what it is receiving
	gl::enableAlphaBlending();
	char txtReceiver[256];
	sprintf_s(txtReceiver, "Received:", mParameterBag->OSCMsg);
	gl::drawString(txtReceiver, vec2(toPixels(20), toPixels(40)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
	sprintf_s(txtReceiver, "fps : %2.2d", (int)getAverageFps());
	gl::drawString(txtReceiver, vec2(getWindowWidth() - toPixels(100), toPixels(40)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
	gl::disableAlphaBlending();

}

void MixnMap::resize()
{
	mShaders->resize();
	// tell the warps our window has been resized, so they properly scale up or down
	mWarpings->resize();
}

void MixnMap::mouseMove(MouseEvent event)
{
	mWarpings->mouseMove(event);
}

void MixnMap::mouseDown(MouseEvent event)
{
	mWarpings->mouseDown(event);
}

void MixnMap::mouseDrag(MouseEvent event)
{
	mWarpings->mouseDrag(event);
}

void MixnMap::mouseUp(MouseEvent event)
{
	mWarpings->mouseUp(event);
}

void MixnMap::keyDown(KeyEvent event)
{
	mWarpings->keyDown(event);
	switch (event.getCode())
	{
	case KeyEvent::KEY_ESCAPE:
		// quit the application
		quit();
		break;
	}
}

void MixnMap::keyUp(KeyEvent event)
{
	mWarpings->keyUp(event);
}

void MixnMap::updateWindowTitle()
{
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) Reymenta mix-n-map");
}

CINDER_APP_NATIVE(MixnMap, RendererGl)
