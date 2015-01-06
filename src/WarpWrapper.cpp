#include "WarpWrapper.h"

using namespace Reymenta;

WarpWrapper::WarpWrapper(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	mTextures = aTexturesRef;
	// instanciate the logger class
	log = Logger::create("WarpWrapperLog.txt");
	log->logTimedString("WarpWrapper constructor");

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
	/*for (auto &warp : mWarps)
	{
		mTextures->createWarpInput();
	}*/
	// adjust the content size of the warps
	Warp::setSize(mWarps, ivec2(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight));//mTextures->getTexture(0)->getSize());
	log->logTimedString("Warps count " + toString(mWarps.size()));

}
void WarpWrapper::createWarps(int count)
{
	if (count > mWarps.size())
	{
		for (int i = mWarps.size(); i < count; i++)
		{
			mWarps.push_back(WarpPerspectiveBilinear::create());
			//mTextures->createWarpInput();
		}
	}
}

void WarpWrapper::save()
{
	fs::path settings = getAssetPath("") / warpsFileName;
	Warp::writeSettings(mWarps, writeFile(settings));

}
void WarpWrapper::resize()
{
	// tell the warps our window has been resized, so they properly scale up or down
	Warp::handleResize(mWarps);

}
void WarpWrapper::mouseMove(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarps, event))
	{
		// let your application perform its mouseMove handling here
	}
}
void WarpWrapper::mouseDown(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarps, event))
	{
		// let your application perform its mouseDown handling here
	}
}
void WarpWrapper::mouseDrag(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarps, event))
	{
		// let your application perform its mouseDrag handling here
	}
}
void WarpWrapper::mouseUp(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarps, event))
	{
		// let your application perform its mouseUp handling here
	}
}
void WarpWrapper::keyDown(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyDown(mWarps, event))
	{
		// warp editor did not handle the key, so handle it here
		switch (event.getCode())
		{
		case KeyEvent::KEY_n:
			// create a warp
			mWarps.push_back(WarpPerspectiveBilinear::create());
			break;
		case KeyEvent::KEY_0:
			// select warp
			setSelectedWarp(0);
			break;
		case KeyEvent::KEY_1:
			// select warp
			setSelectedWarp(1);
			break;
		case KeyEvent::KEY_2:
			// select warp
			setSelectedWarp(2);
			break;
		case KeyEvent::KEY_3:
			// select warp
			setSelectedWarp(3);
			break;
		case KeyEvent::KEY_4:
			// select warp
			setSelectedWarp(4);
			break;
		case KeyEvent::KEY_5:
			// select warp
			setSelectedWarp(5);
			break;
		case KeyEvent::KEY_6:
			// select warp
			setSelectedWarp(6);
			break;
		case KeyEvent::KEY_7:
			// select warp
			setSelectedWarp(7);
			break;
		case KeyEvent::KEY_8:
			// select warp
			setSelectedWarp(8);
			break;
		case KeyEvent::KEY_w:
			// toggle warp edit mode
			Warp::enableEditMode(!Warp::isEditModeEnabled());
			break;
		case KeyEvent::KEY_SPACE:
			// save warp settings
			save();
			// save params
			mParameterBag->save();

			break;
		}
	}
}

void WarpWrapper::keyUp(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarps, event))
	{
		// let your application perform its keyUp handling here
	}
}
void WarpWrapper::draw()
{
	// iterate over the warps and draw their content
	int i = 0;
	for (auto &warp : mWarps)
	{
		if (mTextures->getWarpFbo(mParameterBag->iWarpFboChannels[i]).active)
		{
			//warp->draw(mTextures->getMixTexture(mParameterBag->iWarpFboChannels[i]), mTextures->getMixTexture(mParameterBag->iWarpFboChannels[i])->getBounds());
			warp->draw(mTextures->getWarpTexture(mParameterBag->iWarpFboChannels[i]), mTextures->getWarpTexture(mParameterBag->iWarpFboChannels[i])->getBounds());
			i++;
		}
	}
}
WarpWrapper::~WarpWrapper()
{
	log->logTimedString("WarpWrapper destructor");
}


