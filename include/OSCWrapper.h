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

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class OSC> OSCRef;

	class OSC 
	{

	public:
		OSC(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures);
		static	OSCRef create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures);

		void	update();
	private:
		//! parameters
		ParameterBagRef mParameterBag;
		//! Shaders
		ShadersRef					mShaders;
		// textures
		TexturesRef					mTextures;

		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;
	};
}