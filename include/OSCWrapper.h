#pragma once

#include "cinder/app/AppNative.h"
#include "OscListener.h"
#include "OSCSender.h"
#include "Resources.h"
#include "ParameterBag.h"
// shaders
#include "Shaders.h"
// textures
#include "Textures.h"
// Warps
#include "WarpWrapper.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class OSC> OSCRef;

	class OSC 
	{

	public:
		OSC(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures, WarpWrapperRef aWarpings);
		static	OSCRef create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures, WarpWrapperRef aWarpings);

		void	update();
	private:
		//! parameters
		ParameterBagRef mParameterBag;
		//! Shaders
		ShadersRef					mShaders;
		// textures
		TexturesRef					mTextures;
		// warps
		WarpWrapperRef				mWarpings;

		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;
	};
}