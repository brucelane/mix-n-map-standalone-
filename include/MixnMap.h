/*
Copyright (c) 2010-2013, Paul Houx - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

This file is part of Cinder-Warping.

Cinder-Warping is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Cinder-Warping is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cinder-Warping.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

// parameters
#include "ParameterBag.h"
// textures
#include "Textures.h"
// logger
#include "Logger.h"
// spout
#include "SpoutWrapper.h"
// OSC
#include "OSCWrapper.h"
// Warps
#include "WarpWrapper.h"
// Utils
#include "Batchass.h"
// UserInterface
#include "CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace std;
using namespace Reymenta;

#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

class MixnMap : public App {
public:


	void setup();
	void shutdown();
	void update();
	void draw();

	void resize();
	void fileDrop(FileDropEvent event);
	void mouseMove(MouseEvent event);
	void mouseDown(MouseEvent event);
	void mouseDrag(MouseEvent event);
	void mouseUp(MouseEvent event);

	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);

	void updateWindowTitle();
private:
	// parameters
	ParameterBagRef				mParameterBag;
	// Logger
	LoggerRef					log;
	// Shaders
	//ShadersRef					mShaders;
	// Textures
	//TexturesRef					mTextures;
	// spout
	SpoutWrapperRef				mSpout;
	// osc
	OSCRef						mOSC;
	// warps
	WarpWrapperRef				mWarpings;
	// utils
	BatchassRef					mBatchass;
	// log
	string						mLogMsg;
	bool						newLogMsg;
	static const int			MAX = 16;

};
