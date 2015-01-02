#include "OSCWrapper.h"

using namespace Reymenta;

OSC::OSC(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures, WarpWrapperRef aWarpings)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	mTextures = aTextures;
	mWarpings = aWarpings;
	// OSC sender
	//mOSCSender.setup(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
	// OSC receiver
	mOSCReceiver.setup(mParameterBag->mOSCReceiverPort);
}

OSCRef OSC::create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTextures, WarpWrapperRef aWarpings)
{
	return shared_ptr<OSC>(new OSC(aParameterBag, aShadersRef, aTextures, aWarpings));
}

void OSC::update()
{
	// osc
	while (mOSCReceiver.hasWaitingMessages())
	{
		osc::Message message;
		mOSCReceiver.getNextMessage(&message);
		for (int a = 0; a < 4; a++)
		{
			iargs[a] = 0;
			fargs[a] = 0.0;
			sargs[a] = "";
		}
		int skeletonIndex = 0;
		int jointIndex = 0;
		string oscAddress = message.getAddress();

		int numArgs = message.getNumArgs();
		// get arguments
		for (int i = 0; i < message.getNumArgs(); i++) 
		{
			cout << "-- Argument " << i << std::endl;
			cout << "---- type: " << message.getArgTypeName(i) << std::endl;
			if (message.getArgType(i) == osc::TYPE_INT32) {
				try 
				{
						iargs[i] = message.getArgAsInt32(i);
						sargs[i] = toString(iargs[i]);
				}
				catch (...) {
					cout << "Exception reading argument as int32" << std::endl;
				}
			}
			if (message.getArgType(i) == osc::TYPE_FLOAT) {
				try 
				{
						fargs[i] = message.getArgAsFloat(i);
						sargs[i] = toString(fargs[i]);
				}
				catch (...) {
					cout << "Exception reading argument as float" << std::endl;
				}
			}
			if (message.getArgType(i) == osc::TYPE_STRING) {
				try
				{
						sargs[i] = message.getArgAsString(i);
				}
				catch (...) {
					cout << "Exception reading argument as string" << std::endl;
				}
			}

		}
		// route the OSC commands
		if (oscAddress == "/fspath")
		{
			mShaders->loadPixelFragmentShader(sargs[1]);
			mTextures->addShadaFbo();
		}
		else if (oscAddress == "/texture")
		{
			mTextures->setInput(iargs[0], iargs[1], iargs[2]);
		}
		else if (oscAddress == "/createwarps")
		{
			mWarpings->createWarps(iargs[0]);
		}
		else if (oscAddress == "/select")
		{
			mWarpings->setSelectedWarp(iargs[0]);
		}
		else if (oscAddress == "/channel")
		{
			mWarpings->setChannelForSelectedWarp(iargs[0]);
		}
		else if (oscAddress == "/crossfade")
		{
			mWarpings->setCrossfadeForSelectedWarp(fargs[0]);
		}
		else if (oscAddress == "/midi")
		{
			if (iargs[0] < 0) iargs[0] = 0;
			if (iargs[1] < 0) iargs[1] = 0;
			if (iargs[0] > 4096) iargs[0] = 4096;
			if (iargs[1] > 4096) iargs[1] = 4096;
			float normalizedValue = lmap<float>(iargs[1], 0.0, 127.0, 0.0, 1.0);
			switch (iargs[0])
			{
			case 15:
				// crossfade
				mParameterBag->controlValues[iargs[0]] = normalizedValue;
				break;
			}
		}

		string oscString = "osc from:" + message.getRemoteIp() + " adr:" + oscAddress + " 0: " + sargs[0] + " 1: " + sargs[1] + " 2: " + sargs[2] + " 3: " + sargs[3];
		mParameterBag->OSCMsg = oscString;
	}
}
