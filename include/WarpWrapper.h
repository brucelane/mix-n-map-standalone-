#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"
#include "Logger.h"
// warp
#include "WarpBilinear.h"
#include "WarpPerspective.h"
#include "WarpPerspectiveBilinear.h"
// textures
#include "Textures.h"
// shaders
#include "Shaders.h"
// Parameters
#include "ParameterBag.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ph::warping;

namespace Reymenta
{
	// stores the pointer to the WarpWrapper instance
	typedef std::shared_ptr<class WarpWrapper> WarpWrapperRef;

	class WarpWrapper {
	public:
		WarpWrapper(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef);
		virtual					~WarpWrapper();
		static WarpWrapperRef	create(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef)
		{
			return shared_ptr<WarpWrapper>(new WarpWrapper(aParameterBag, aTexturesRef, aShadersRef));
		}
		void						save();
		void						resize();
		void						mouseMove(MouseEvent event);
		void						mouseDown(MouseEvent event);
		void						mouseDrag(MouseEvent event);
		void						mouseUp(MouseEvent event);
		void						keyDown(KeyEvent event);
		void						keyUp(KeyEvent event);
		void						draw();
		void						createWarps(int count);
		void						setSelectedWarp(int index) { mParameterBag->selectedWarp = min(((int)mWarps.size())-1, index); };
		//void						setChannelForSelectedWarp(int index) { mParameterBag->iWarpFboChannels[mParameterBag->selectedWarp] = index; };
		void						setCrossfadeForSelectedWarp(float value) { mTextures->setCrossfade(value); };
		int							getSelectedWarp() { return mParameterBag->selectedWarp; };
	private:
		// Logger
		LoggerRef					log;
		// Parameters
		ParameterBagRef				mParameterBag;
		// Shaders
		ShadersRef					mShaders;
		// Textures
		TexturesRef					mTextures;

		const string				warpsFileName = "MixnMapWarps.xml";
		WarpList					mWarps;
	};
}