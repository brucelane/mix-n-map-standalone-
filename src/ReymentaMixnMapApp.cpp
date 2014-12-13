#include "ReymentaMixnMapApp.h"

void ReymentaMixnMapApp::prepareSettings(Settings *settings)
{
	// instanciate the logger class
	log = Logger::create("MixnMap.txt");
	log->logTimedString("start");

	// parameters
	mParameterBag = ParameterBag::create();
	if (mParameterBag->mAutoLayout) getWindowsResolution();
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
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag);

	updateWindowTitle();

	//mImage = gl::Texture::create( loadImage( loadAsset( "help.png" ) ), gl::Texture2d::Format().loadTopDown() );

	// initialize warps
	log->logTimedString("Loading MixnMapWarps.xml");
	fs::path settings = getAssetPath("") / warpsFileName;
	if (fs::exists(settings))
	{
		// load warp settings from file if one exists
		mWarps = Warp::readSettings(loadFile(settings));
	}
	else
	{
		// otherwise create a warp from scratch
		mWarps.push_back(WarpPerspectiveBilinear::create());
	}
	for (auto &warp : mWarps)
	{
		mTextures->createWarpInput();
	}
	// adjust the content size of the warps
	Warp::setSize(mWarps, ivec2(mParameterBag->mFboWidth, mParameterBag->mFboHeight));//mTextures->getTexture(0)->getSize());
	log->logTimedString("Warps count " + toString(mWarps.size()));

}

void ReymentaMixnMapApp::shutdown()
{
	// save warp settings
	fs::path settings = getAssetPath("") / warpsFileName;
	Warp::writeSettings(mWarps, writeFile(settings));
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

	// iterate over the warps and draw their content
	int i = 0;
	for (auto &warp : mWarps)
	{

		//warp->setSize(ivec2(mTextures->getMixTexture(mParameterBag->iWarpFboChannels[i])->getWidth(), mTextures->getMixTexture(mParameterBag->iWarpFboChannels[i])->getHeight()));
		warp->draw(mTextures->getMixTexture(mParameterBag->iWarpFboChannels[i]), mTextures->getMixTexture(mParameterBag->iWarpFboChannels[i])->getBounds());
		i++;
	}
}

void ReymentaMixnMapApp::resize()
{
	mShaders->resize();
	// tell the warps our window has been resized, so they properly scale up or down
	Warp::handleResize(mWarps);
}

void ReymentaMixnMapApp::mouseMove(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarps, event)) {
		// let your application perform its mouseMove handling here
	}
}

void ReymentaMixnMapApp::mouseDown(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarps, event)) {
		// let your application perform its mouseDown handling here
	}
}

void ReymentaMixnMapApp::mouseDrag(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarps, event)) {
		// let your application perform its mouseDrag handling here
	}
}

void ReymentaMixnMapApp::mouseUp(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarps, event)) {
		// let your application perform its mouseUp handling here
	}
}

void ReymentaMixnMapApp::keyDown(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyDown(mWarps, event)) {
		// warp editor did not handle the key, so handle it here
		switch (event.getCode()) {
		case KeyEvent::KEY_ESCAPE:
			// quit the application
			quit();
			break;
		case KeyEvent::KEY_n:
			// create a warp
			mWarps.push_back(WarpPerspectiveBilinear::create());
			break;
		case KeyEvent::KEY_f:
			// toggle full screen
			setFullScreen(!isFullScreen());
			break;
		case KeyEvent::KEY_w:
			// toggle warp edit mode
			Warp::enableEditMode(!Warp::isEditModeEnabled());
			break;
		case KeyEvent::KEY_h:
			hideCursor();
			break;
		case KeyEvent::KEY_s:
			showCursor();
			break;
		case KeyEvent::KEY_SPACE:
			// save warp settings
			fs::path settings = getAssetPath("") / warpsFileName;
			Warp::writeSettings(mWarps, writeFile(settings));
			break;
		}
	}
}

void ReymentaMixnMapApp::keyUp(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarps, event)) {
		// let your application perform its keyUp handling here
	}
}

void ReymentaMixnMapApp::updateWindowTitle()
{
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) Reymenta mix-n-map");
}

CINDER_APP_NATIVE(ReymentaMixnMapApp, RendererGl)
