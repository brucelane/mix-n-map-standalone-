/**
* \file ParameterBag.cpp
* \author Bruce LANE/Nathan Selikoff
* \date 20 november 2014
*
* Parameters for all classes.
*
*/

#include "ParameterBag.h"

using namespace ci;
using namespace std;
using namespace Reymenta;

ParameterBag::ParameterBag()
{
	// reset no matter what, so we don't miss anything
	reset();

	// check to see if ReymentaSettings.xml file exists and restore if it does
	fs::path params = getAssetPath("") / settingsFileName;
	if (fs::exists(params))
		restore();
}

ParameterBagRef ParameterBag::create()
{
	return shared_ptr<ParameterBag>(new ParameterBag());
}

bool ParameterBag::save()
{
	// save in assets folder
	fs::path directory = getAssetPath("");
	if (!fs::exists(directory)) {
		if (!createDirectories(directory / settingsFileName)) {
			return false;
		}
	}

	fs::path path = directory / settingsFileName;

	XmlTree settings("settings", "");

	XmlTree ShowConsole("ShowConsole", "");
	ShowConsole.setAttribute("value", toString(mShowConsole));
	settings.push_back(ShowConsole);

	XmlTree UseDX9("UseDX9", "");
	UseDX9.setAttribute("value", toString(mUseDX9));
	settings.push_back(UseDX9);

	XmlTree OSCReceiverPort("OSCReceiverPort", "");
	OSCReceiverPort.setAttribute("value", toString(mOSCReceiverPort));
	settings.push_back(OSCReceiverPort);

	XmlTree AutoLayout("AutoLayout", "");
	AutoLayout.setAttribute("value", toString(mAutoLayout));
	settings.push_back(AutoLayout);

	XmlTree RenderWidth("RenderWidth", "");
	RenderWidth.setAttribute("value", toString(mRenderWidth));
	settings.push_back(RenderWidth);

	XmlTree RenderHeight("RenderHeight", "");
	RenderHeight.setAttribute("value", toString(mRenderHeight));
	settings.push_back(RenderHeight);

	XmlTree RenderX("RenderX", "");
	RenderX.setAttribute("value", toString(mRenderX));
	settings.push_back(RenderX);

	XmlTree RenderY("RenderY", "");
	RenderY.setAttribute("value", toString(mRenderY));
	settings.push_back(RenderY);

	// TODO: test for successful writing of XML
	settings.write(writeFile(path));

	return true;
}

bool ParameterBag::restore()
{
	// check to see if ReymentaSettings.xml file exists
	fs::path params = getAssetPath("") / settingsFileName;
	if (fs::exists(params)) {
		// if it does, restore
		const XmlTree xml(loadFile(params));

		if (!xml.hasChild("settings")) {
			return false;
		}
		else {
			const XmlTree settings = xml.getChild("settings");

			if (settings.hasChild("ShowConsole")) {
				XmlTree ShowConsole = settings.getChild("ShowConsole");
				mShowConsole = ShowConsole.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("UseDX9")) {
				XmlTree UseDX9 = settings.getChild("UseDX9");
				mUseDX9 = UseDX9.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCReceiverPort")) {
				XmlTree OSCReceiverPort = settings.getChild("OSCReceiverPort");
				mOSCReceiverPort = OSCReceiverPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("AutoLayout")) {
				XmlTree AutoLayout = settings.getChild("AutoLayout");
				mAutoLayout = AutoLayout.getAttributeValue<bool>("value");
			}
			// if AutoLayout is false we have to read the custom screen layout
			if (!mAutoLayout)
			{
				if (settings.hasChild("RenderWidth")) {
					XmlTree RenderWidth = settings.getChild("RenderWidth");
					mRenderWidth = RenderWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderHeight")) {
					XmlTree RenderHeight = settings.getChild("RenderHeight");
					mRenderHeight = RenderHeight.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderX")) {
					XmlTree RenderX = settings.getChild("RenderX");
					mRenderX = RenderX.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderY")) {
					XmlTree RenderY = settings.getChild("RenderY");
					mRenderY = RenderY.getAttributeValue<int>("value");
				}

			}
			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void ParameterBag::reset()
{
	mAutoLayout = true;
	mRenderWidth = 1024;
	mRenderHeight = 768;
	mRenderXY = mLeftRenderXY = mRightRenderXY = mPreviewRenderXY = vec2(0.0);
	mRenderPosXY = vec2(0.0, 320.0);
	mRenderResoXY = vec2(mRenderWidth, mRenderHeight);
	mRenderResolution = ivec2(mRenderWidth, mRenderHeight);
	mPreviewFragXY = vec2(0.0, 0.0);

	// spout
	mMemoryMode = false;
	mUseDX9 = false;

	mShowConsole = false;

	// tempo
	mTempo = 166.0;
	mUseTimeWithTempo = false;
	iDeltaTime = 60 / mTempo;
	iTempoTime = 0.0;
	iTimeFactor = 1.0;
	//audio
	// audio in multiplication factor
	mAudioMultFactor = 1.0;
	mUseLineIn = true;
	maxVolume = 0.0f;
	mAudioTextureIndex = 0;
	mData = new float[1024];
	for (int i = 0; i < 1024; i++)
	{
		mData[i] = 0;
	}
	for (int i = 0; i < 4; i++)
	{
		iFreqs[i] = i;
	}
	// shader uniforms
	iGlobalTime = 1.0f;
	iResolution = vec3(mRenderWidth, mRenderHeight, 1.0);
	for (int i = 0; i < 4; i++)
	{
		iChannelTime[i] = i;
	}
	for (int i = 0; i < 4; i++)
	{
		iChannelResolution[i] = vec3(mRenderWidth, mRenderHeight, 1.0);
	}
	iDebug = iFade = iLight = iLightAuto = iRepeat = false;
	iFps = 60.0;
	iShowFps = true;
	iMouse = vec4(mRenderWidth / 2, mRenderHeight / 2, 1.0, 1.0);
	iGreyScale = false;

	// transition
	iTransition = 0;
	iAnim = 0.0;
	mTransitionDuration = 1.0f;


	// fbo
	mFboWidth = 640;
	mFboHeight = 480;
	mPreviewWidth = 156;
	mPreviewHeight = 88;
	mCurrentShadaFboIndex = 0;
	mMixFboIndex = 1;
	mLeftFboIndex = 2;
	mRightFboIndex = 3;

	mWarpCount = 3;
	// OSC
	mOSCDestinationHost = "127.0.0.1";
	mOSCDestinationPort = 5005;
	mOSCReceiverPort = 9009;
	OSCMsg = "OSC listening on port 9009";
	// colors
	mLockFR = mLockFG = mLockFB = mLockFA = mLockBR = mLockBG = mLockBB = mLockBA = false;
	tFR = tFG = tFB = tFA = tBR = tBG = tBB = tBA = false;

	for (int a = 0; a < 8; a++)
	{
		iChannels[a] = a;
		iWarpFboChannels[a] = 0;
	}

	// midi and OSC
	for (int c = 0; c < 128; c++)
	{
		controlValues[c] = 0.01f;
	}

	// red
	controlValues[1] = 1.0f;
	// green
	controlValues[2] = 0.0f;
	// blue
	controlValues[3] = 0.0f;
	// Alpha 
	controlValues[4] = 1.0f;
	// background red
	controlValues[5] = 1.0f;
	// background green
	controlValues[6] = 1.0f;
	// background blue
	controlValues[7] = 0.0f;
	// background alpha
	controlValues[8] = 0.2f;
	// ratio
	controlValues[11] = 20.0f;
	// Speed 
	controlValues[12] = 12.0f;
	// zoom
	controlValues[13] = 1.0f;
	// exposure
	controlValues[14] = 1.0f;
	// crossfade from midi2osc (was Blendmode) 
	controlValues[15] = 0.5f;
	// Steps
	controlValues[16] = 16.0f;
	// Pixelate
	controlValues[18] = 60.0f;
	// RotationSpeed
	controlValues[19] = 1.0f;
	// glitch
	controlValues[45] = 0.0f;
	// toggle
	controlValues[46] = 0.0f;
	// vignette
	controlValues[47] = 0.0f;
	// invert
	controlValues[48] = 0.0f;
}