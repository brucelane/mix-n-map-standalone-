/*
TODO
- competition between imgui and midi colors: imgui wins
- render video to fbo

*/

#include "MixnMap.h"

void MixnMap::setup()
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
	setWindowSize(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
	setWindowPos(ivec2(mParameterBag->mRenderX, mParameterBag->mRenderY));
#else
	setWindowSize(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
	setWindowPos(ivec2(mParameterBag->mRenderX, mParameterBag->mRenderY));
#endif  // _DEBUG
	//setBorderless();
	//setResizable(false); // resize allowed for a receiver, but runtime error on the resize in the shaders drawing
	// set a high frame rate 1000 to disable limitation
	setFrameRate(60.0f);

	log->logTimedString("prepareSettings done");	
	log->logTimedString("setup");

	mBatchass->setup();
	// instanciate the Shaders class, must not be in prepareSettings
	//mShaders = Shaders::create(mParameterBag);
	// instanciate the textures class
	//mTextures = Textures::create(mParameterBag, mShaders);
	// instanciate the spout class
	mSpout = SpoutWrapper::create(mParameterBag, mBatchass->mTextures);
	// instanciate the Warps class
	mWarpings = WarpWrapper::create(mParameterBag, mBatchass->mTextures, mBatchass->mShaders);
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag, mBatchass->mShaders, mBatchass->mTextures);
	// set ui window and io events callbacks
	ui::connectWindow(getWindow());
	ui::initialize();

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
void MixnMap::fileDrop(FileDropEvent event)
{
	// use the last of the dropped files
	boost::filesystem::path mPath = event.getFile(event.getNumFiles() - 1);
	string mFile = mPath.string();
	mBatchass->mTextures->fileDrop(mFile);


}

void MixnMap::update()
{
	mParameterBag->iChannelTime[0] = getElapsedSeconds();
	mParameterBag->iChannelTime[1] = getElapsedSeconds() - 1;
	mParameterBag->iChannelTime[2] = getElapsedSeconds() - 2;
	mParameterBag->iChannelTime[3] = getElapsedSeconds() - 3;
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
	mBatchass->mTextures->update();
	//! update shaders (must be after the textures update)
	mBatchass->mShaders->update();

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
	mBatchass->mTextures->draw();
	mWarpings->draw();
	if (mParameterBag->mShowUI)
	{
		gl::enableAlphaBlending();

		static bool showTest = false, showTheme = false, showAudio = true, showShaders = true, showOSC = true, showFps = true;
		// UI
#pragma region style
		// our theme variables
		ImGuiStyle& style = ui::GetStyle();
		style.WindowRounding = 4;
		style.WindowPadding = ImVec2(3, 3);
		style.FramePadding = ImVec2(2, 2);
		style.ItemSpacing = ImVec2(3, 3);
		style.ItemInnerSpacing = ImVec2(3, 3);
		style.WindowMinSize = ImVec2(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight);
		style.Alpha = 0.6f;
		style.Colors[ImGuiCol_Text] = ImVec4(0.89f, 0.92f, 0.94f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.38f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.4f, 0.21f, 0.21f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.99f, 0.22f, 0.22f, 0.50f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.04f, 0.04f, 0.04f, 0.22f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.8f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9f, 0.45f, 0.45f, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.8f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.8f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_TooltipBg] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
#pragma endregion style
		//ui::NewFrame();
		ui::SetNextWindowPos(ImVec2(0, 0));
		// start a new window
		ui::Begin("MixnMap", NULL, ImVec2(500, 600));
		{
			if (ui::CollapsingHeader("Parameters", "1", true, true))
			{
				// Checkbox
				ui::Checkbox("Audio", &showAudio);
				ui::SameLine();
				ui::Checkbox("Shada", &showShaders);
				ui::SameLine();
				ui::Checkbox("Test", &showTest);
				ui::SameLine();
				ui::Checkbox("FPS", &showFps);
				ui::SameLine();
				ui::Checkbox("OSC", &showOSC);
				ui::SameLine();
				ui::Checkbox("Editor", &showTheme);
				if (ui::Button("Save Params")) { mParameterBag->save(); }

				// foreground color
				/*static float color[4] = { mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3], mParameterBag->controlValues[4] };
				ui::ColorEdit4("f", color);
				mParameterBag->controlValues[1] = color[0];
				mParameterBag->controlValues[2] = color[1];
				mParameterBag->controlValues[3] = color[2];
				mParameterBag->controlValues[4] = color[3];
				ui::SameLine();
				ui::TextColored(ImVec4(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3], mParameterBag->controlValues[4]), "fg color");

				// background color
				static float backcolor[4] = { mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7], mParameterBag->controlValues[8] };
				ui::ColorEdit4("g", backcolor);
				mParameterBag->controlValues[5] = backcolor[0];
				mParameterBag->controlValues[6] = backcolor[1];
				mParameterBag->controlValues[7] = backcolor[2];
				mParameterBag->controlValues[8] = backcolor[3];
				ui::SameLine();
				ui::TextColored(ImVec4(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7], mParameterBag->controlValues[8]), "bg color");*/

			}
			/*if (ui::CollapsingHeader("Warps", "2", true, true))
			{
			ui::Columns(5, "data", true);
			ui::Text("WarpID"); ui::NextColumn();
			ui::Text("LeftMode"); ui::NextColumn();
			ui::Text("LeftInput"); ui::NextColumn();
			ui::Text("rightMode"); ui::NextColumn();
			ui::Text("rightIndex"); ui::NextColumn();
			ui::Separator();

			for (int i = 0; i < mTextures->warpInputs.size(); i++)
			{
			ui::Text("%d", i);		ui::NextColumn();
			ui::Text("%d", mTextures->getWarpInput(i).leftMode); ui::NextColumn();
			ui::Text("%d", mTextures->getWarpInput(i).leftIndex); ui::NextColumn();
			//ui::Text("%d", mTextures->getWarpInput(i).rightMode); ui::NextColumn();
			//static int e = 0;
			//ui::RadioButton("texture", &e, 0); ui::NextColumn();
			//ui::RadioButton("shader", &e, 1); ui::NextColumn();
			//ui::Text("%d", mTextures->getWarpInput(i).rightIndex); ui::NextColumn();
			}
			//RTE because nothing ! mTextures->warpInputs[0].rightMode = 1;
			//mTextures->setWarpInputModeRight(0, 1);


			ui::Separator();
			ui::Text("Selected warp: %d", mParameterBag->selectedWarp);

			}
			if (ui::CollapsingHeader("Log", "4", true, true))
			{

				static ImGuiTextBuffer log;
				static int lines = 0;
				if (ui::Button("Clear")) { log.clear(); lines = 0; }
				ui::SameLine();
				ui::Text("Buffer contents: %d lines, %d bytes", lines, log.size());

				if (newLogMsg)
				{
					newLogMsg = false;
					log.append(mLogMsg.c_str());
					lines++;
					if (lines > 10) { log.clear(); lines = 0; }
				}
				ui::BeginChild("Log");
				ui::TextUnformatted(log.begin(), log.end());
				ui::EndChild();
			}*/

		}
		if (showTest) ui::ShowTestWindow();
		ui::End();
		// start a new window
		if (showTheme)
		{

			ui::Begin("Theme Editor", NULL, ImVec2(200, 100));
			{
				// our theme variables
				static ImVec4 color0 = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
				static ImVec4 color1 = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
				static ImVec4 color2 = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
				static ImVec4 color3 = ImVec4(0.69f, 0.69f, 0.69f, 1.0f);
				static ImVec4 color4 = ImVec4(0.27f, 0.27f, 0.27f, 1.0f);

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

				if (ui::CollapsingHeader("Colors")){

					// add radios on one line to switch between color modes
					static ImGuiColorEditMode colorMode = ImGuiColorEditMode_RGB;
					ui::RadioButton("RGB", &colorMode, ImGuiColorEditMode_RGB);
					ui::SameLine();
					ui::RadioButton("HSV", &colorMode, ImGuiColorEditMode_HSV);
					ui::SameLine();
					ui::RadioButton("HEX", &colorMode, ImGuiColorEditMode_HEX);

					// change color mode
					ui::ColorEditMode(colorMode);

					// add color controls
					ui::ColorEdit4("Color0", (float*)&color0, false);
					ui::ColorEdit4("Color1", (float*)&color1, false);
					ui::ColorEdit4("Color2", (float*)&color2, false);
					ui::ColorEdit4("Color3", (float*)&color3, false);
					ui::ColorEdit4("Color4", (float*)&color4, false);
				}

				if (ui::CollapsingHeader("Layout")){

					// layout controls
					ui::SliderFloat2("WindowPadding", WindowPadding, 0, 50, "%.0f");
					ui::SliderFloat2("WindowMinSize", WindowMinSize, 0, 250, "%.0f");
					ui::SliderFloat2("FramePadding", FramePadding, 0, 40, "%.0f");
					ui::SliderFloat2("ItemSpacing", ItemSpacing, 0, 40, "%.0f");
					ui::SliderFloat2("ItemInnerSpacing", ItemInnerSpacing, 0, 40, "%.0f");
					ui::SliderFloat("WindowFillAlphaDefault", &WindowFillAlphaDefault, 0, 1);
					ui::SliderFloat("WindowRounding", &WindowRounding, 0, 15, "%.0f");
					ui::SliderFloat("TreeNodeSpacing", &TreeNodeSpacing, 0, 50, "%.0f");
					ui::SliderFloat("ColumnsMinSpacing", &ColumnsMinSpacing, 0, 150, "%.0f");
					ui::SliderFloat("ScrollBarWidth", &ScrollBarWidth, 0, 35, "%.0f");
				}

				// if log pressed generate code and output to the console
				if (ui::Button("Log to Console")){
					console() << "ImVec4 color0 = ImVec4( " << color0.x << ", " << color0.y << ", " << color0.z << ", " << color0.w << " );" << endl
						<< "ImVec4 color1 = ImVec4( " << color1.x << ", " << color1.y << ", " << color1.z << ", " << color1.w << " );" << endl
						<< "ImVec4 color2 = ImVec4( " << color2.x << ", " << color2.y << ", " << color2.z << ", " << color2.w << " );" << endl
						<< "ImVec4 color3 = ImVec4( " << color3.x << ", " << color3.y << ", " << color3.z << ", " << color3.w << " );" << endl
						<< "ImVec4 color4 = ImVec4( " << color4.x << ", " << color4.y << ", " << color4.z << ", " << color4.w << " );" << endl
						<< "ui::setThemeColor( color0, color1, color2, color3, color4 );" << endl << endl
						<< "ImGuiStyle& style = ui::GetStyle();" << endl
						<< "style.WindowPadding          = ImVec2( " << WindowPadding[0] << "," << WindowPadding[1] << " );" << endl
						<< "style.WindowMinSize          = ImVec2( " << WindowMinSize[0] << "," << WindowMinSize[1] << " );" << endl
						<< "style.FramePadding           = ImVec2( " << FramePadding[0] << "," << FramePadding[1] << " );" << endl
						<< "style.ItemSpacing            = ImVec2( " << ItemSpacing[0] << "," << ItemSpacing[1] << " );" << endl
						<< "style.ItemInnerSpacing       = ImVec2( " << ItemInnerSpacing[0] << "," << ItemInnerSpacing[1] << " );" << endl
						<< "style.WindowFillAlphaDefault = " << WindowFillAlphaDefault << ";" << endl
						<< "style.WindowRounding         = " << WindowRounding << ";" << endl
						<< "style.TreeNodeSpacing		 = " << TreeNodeSpacing << ";" << endl
						<< "style.ColumnsMinSpacing		 = " << ColumnsMinSpacing << ";" << endl
						<< "style.ScrollBarWidth		 = " << ScrollBarWidth << ";" << endl;
				}


				// add a slider to control the background brightness
				//ui::SliderFloat("Background", &f, 0, 1);
			}
			ui::End();
		}
		// audio window
		if (showAudio)
		{

			ui::Begin("Audio", NULL, ImVec2(200, 100));
			{
				ui::Checkbox("Playing", &mParameterBag->mIsPlaying);
				ui::SameLine();
				ui::Text("Beat %d", mParameterBag->mBeat);
				ui::SameLine();
				ui::Text("Tempo %.2f", mParameterBag->mTempo);

				static ImVector<float> values; if (values.empty()) { values.resize(40); memset(&values.front(), 0, values.size()*sizeof(float)); }
				static int values_offset = 0;
				// audio maxVolume
				static float refresh_time = -1.0f;
				if (ui::GetTime() > refresh_time + 1.0f / 20.0f)
				{
					refresh_time = ui::GetTime();
					values[values_offset] = mParameterBag->maxVolume;
					values_offset = (values_offset + 1) % values.size();
				}
				ui::PlotLines("Volume", &values.front(), (int)values.size(), values_offset, toString(mBatchass->formatFloat(mParameterBag->maxVolume)).c_str(), 0.0f, 1.0f, ImVec2(0, 70));

				for (int a = 0; a < MAX; a++)
				{
					if (mOSC->tracks[a] != "default.glsl") ui::Button(mOSC->tracks[a].c_str());
				}

			}
			ui::End();
		}
		// shada window
		if (showShaders)
		{

			ui::Begin("Shada", NULL, ImVec2(200, 100));
			{
				for (int a = 0; a < MAX; a++)
				{
					if (mBatchass->mShaders->getShaderName(a) != "default.glsl") ui::Button(mBatchass->mShaders->getShaderName(a).c_str());
				}
			}
			ui::End();
		}
		// fps window
		if (showFps)
		{
			ui::Begin("Fps", NULL, ImVec2(200, 60));
			{
				static ImVector<float> values; if (values.empty()) { values.resize(100); memset(&values.front(), 0, values.size()*sizeof(float)); }
				static int values_offset = 0;
				static float refresh_time = -1.0f;
				if (ui::GetTime() > refresh_time + 1.0f / 6.0f)
				{
					refresh_time = ui::GetTime();
					values[values_offset] = getAverageFps();
					values_offset = (values_offset + 1) % values.size();
				}

				ui::PlotLines("FPS", &values.front(), (int)values.size(), values_offset, toString(floor(getAverageFps())).c_str(), 0.0f, 300.0f, ImVec2(0, 30));
			}
			ui::End();
		}
		// osc window
		if (showOSC)
		{

			ui::Begin("OSC", NULL, ImVec2(200, 100));
			{
				// osc
				if (mParameterBag->mIsOSCSender)
				{
					ui::Text("Sending to host %s", mParameterBag->mOSCDestinationHost.c_str());
					ui::SameLine();
					ui::Text(" on port %d", mParameterBag->mOSCDestinationPort);
				}
				else
				{
					ui::Text("Host %s", mParameterBag->mOSCDestinationHost.c_str());
					ui::SameLine();
					ui::Text("Port %d", mParameterBag->mOSCDestinationPort);
					ui::SameLine();
					/*static char host[128];
					strcpy(host, mParameterBag->mOSCDestinationHost.c_str());
					ui::InputText("OSC send to host", host, 128);
					ui::InputInt("OSC send on port", &mParameterBag->mOSCDestinationPort);
					ui::SameLine();*/
					if (ui::Button("OK"))
					{
						//mParameterBag->mOSCDestinationHost = host;
						mOSC->setupSender();
					}
				}
				ui::SameLine();
				ui::Text("Receiving on port %d", mParameterBag->mOSCReceiverPort);
				static ImGuiTextBuffer OSClog;
				static int lines = 0;
				if (ui::Button("Clear")) { OSClog.clear(); lines = 0; }
				ui::SameLine();
				ui::Text("Buffer contents: %d lines, %d bytes", lines, OSClog.size());

				if (mParameterBag->newOSCMsg)
				{
					mParameterBag->newOSCMsg = false;
					OSClog.append(mParameterBag->OSCMsg.c_str());
					lines++;
					if (lines > 3) { OSClog.clear(); lines = 0; }
				}
				ui::BeginChild("OSClog");
				ui::TextUnformatted(OSClog.begin(), OSClog.end());
				ui::EndChild();
			}
			ui::End();
		}
		ui::Render();
	}
	gl::disableAlphaBlending();

}

void MixnMap::resize()
{
	mBatchass->mShaders->resize();
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
	case ci::app::KeyEvent::KEY_o:
		mParameterBag->mOriginUpperLeft = !mParameterBag->mOriginUpperLeft;
		break;
	case ci::app::KeyEvent::KEY_g:
		mParameterBag->iGreyScale = !mParameterBag->iGreyScale;
		break;
	case ci::app::KeyEvent::KEY_u:
		mParameterBag->mShowUI = !mParameterBag->mShowUI;
		break;
	case ci::app::KeyEvent::KEY_c:
		if (mParameterBag->mCursorVisible)
		{
			hideCursor();
		}
		else
		{
			showCursor();
		}
		mParameterBag->mCursorVisible = !mParameterBag->mCursorVisible;
		break;
	default:
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

CINDER_APP(MixnMap, RendererGl)
