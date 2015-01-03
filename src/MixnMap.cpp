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
	// set ui window and io events callbacks
	ImGui::setWindow(getWindow());

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

	ImGui::NewFrame();

	// start a new window
	ImGui::Begin("MixnMap", NULL, ImVec2(500, 600));
	{
		// our theme variables
		static float WindowPadding[2] = { 25, 10 };
		static float WindowMinSize[2] = { 160, 80 };
		static float FramePadding[2] = { 4, 4 };
		static float ItemSpacing[2] = { 10, 5 };
		static float ItemInnerSpacing[2] = { 5, 5 };

		static float WindowFillAlphaDefault = 0.7;
		static float WindowRounding = 4;
		static float TreeNodeSpacing = 22;
		static float ColumnsMinSpacing = 50;
		static float ScrollBarWidth = 12;

		if (ImGui::CollapsingHeader("Parameters", "1", true, true))
		{
			ImGui::InputInt("OSC receiver port", &mParameterBag->mOSCReceiverPort);
			// foreground color
			static float color[4] = { mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3], mParameterBag->controlValues[4] };
			ImGui::ColorEdit4("color", color);
			mParameterBag->controlValues[1] = color[0];
			mParameterBag->controlValues[2] = color[1];
			mParameterBag->controlValues[3] = color[2];
			mParameterBag->controlValues[4] = color[3];
			ImGui::TextColored(ImVec4(mParameterBag->controlValues[1], 0.0f, 0.0f, 1.0f), "Red"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, mParameterBag->controlValues[2], 0.0f, 1.0f), "Green"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, mParameterBag->controlValues[3], 1.0f), "Blue"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3], mParameterBag->controlValues[4]), "Alpha");

			// background color
			static float backcolor[4] = { mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7], mParameterBag->controlValues[8] };
			ImGui::ColorEdit4("back color", backcolor);
			mParameterBag->controlValues[5] = backcolor[0];
			mParameterBag->controlValues[6] = backcolor[1];
			mParameterBag->controlValues[7] = backcolor[2];
			mParameterBag->controlValues[8] = backcolor[3];
			ImGui::TextColored(ImVec4(mParameterBag->controlValues[5], 0.0f, 0.0f, 1.0f), "Red"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, mParameterBag->controlValues[6], 0.0f, 1.0f), "Green"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, mParameterBag->controlValues[7], 1.0f), "Blue"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7], mParameterBag->controlValues[8]), "Alpha");

			if (ImGui::Button("Save")) { mParameterBag->save(); }

		}
		if (ImGui::CollapsingHeader("Log", "2", true, true))
		{
			static bool pause;
			static ImVector<float> values; if (values.empty()) { values.resize(100); memset(&values.front(), 0, values.size()*sizeof(float)); }
			static int values_offset = 0;
			if (!pause)
			{
				static float refresh_time = -1.0f;
				if (ImGui::GetTime() > refresh_time + 1.0f / 6.0f)
				{
					refresh_time = ImGui::GetTime();
					values[values_offset] = getAverageFps();
					values_offset = (values_offset + 1) % values.size();
				}
			}
			ImGui::PlotLines("FPS", &values.front(), (int)values.size(), values_offset, toString(floor(getAverageFps())).c_str(), 0.0f, 300.0f, ImVec2(0, 30));

			ImGui::SameLine(); ImGui::Checkbox("pause", &pause);
			static ImGuiTextBuffer log;
			static int lines = 0;
			ImGui::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
			if (ImGui::Button("Clear")) { log.clear(); lines = 0; }
			//ImGui::SameLine();

			if (newLogMsg)
			{
				newLogMsg = false;
				log.append(mLogMsg.c_str());
				lines++;
				if (lines > 10) { log.clear(); lines = 0; }
			}
			ImGui::BeginChild("Log");
			ImGui::TextUnformatted(log.begin(), log.end());
			ImGui::EndChild();
		}
		if (ImGui::CollapsingHeader("OSC", "2", true, true))
		{
			static ImGuiTextBuffer OSClog;
			static int lines = 0;
			ImGui::Text("Buffer contents: %d lines, %d bytes", lines, OSClog.size());
			if (ImGui::Button("Clear")) { OSClog.clear(); lines = 0; }

			if (mParameterBag->newOSCMsg)
			{
				mParameterBag->newOSCMsg = false;
				OSClog.append(mParameterBag->OSCMsg.c_str());
				lines++;
				if (lines > 10) { OSClog.clear(); lines = 0; }
			}
			ImGui::BeginChild("OSClog");
			ImGui::TextUnformatted(OSClog.begin(), OSClog.end());
			ImGui::EndChild();
		}
	}
	ImGui::ShowTestWindow();
	ImGui::End();
	ImGui::Render();

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
