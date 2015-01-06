/*
TODO
- competition between imgui and midi colors: imgui wins
- render video to fbo

*/

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
	// set a high frame rate 1000 to disable limitation
	settings->setFrameRate(60.0f);

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
void MixnMap::fileDrop(FileDropEvent event)
{
	// use the last of the dropped files
	boost::filesystem::path mPath = event.getFile(event.getNumFiles() - 1);
	string mFile = mPath.string();
	mTextures->fileDrop(mFile);


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
	if (mParameterBag->mShowUI)
	{
		gl::enableAlphaBlending();

		static bool showTest = false, showTheme = false, showAudio = true, showShaders = true, showOSC = true, showFps = true;
		// UI
		ImGui::NewFrame();
		ImGui::SetNewWindowDefaultPos(ImVec2(0, 0));
		// start a new window
		ImGui::Begin("MixnMap", NULL, ImVec2(500, 600));
		{
			ImVec4 color0 = ImVec4(0.431373, 0.160784, 0.372549, 1);
			ImVec4 color1 = ImVec4(0.2, 0.0392157, 0.0392157, 1);
			ImVec4 color2 = ImVec4(0.317647, 0.184314, 0.2, 1);
			ImVec4 color3 = ImVec4(1, 0.647059, 1, 1);
			ImVec4 color4 = ImVec4(0.741176, 0.0941176, 1, 1);
			ImGui::setThemeColor(color0, color1, color2, color3, color4);
			// our theme variables
			static float WindowPadding[2] = { 4, 2 };
			static float WindowMinSize[2] = { 160, 80 };
			static float FramePadding[2] = { 4, 4 };
			static float ItemSpacing[2] = { 10, 5 };
			static float ItemInnerSpacing[2] = { 5, 5 };

			static float WindowFillAlphaDefault = 0.35;
			static float WindowRounding = 11;
			static float TreeNodeSpacing = 22;
			static float ColumnsMinSpacing = 50;
			static float ScrollBarWidth = 12;

			if (ImGui::CollapsingHeader("Parameters", "1", true, true))
			{
				// Checkbox
				ImGui::Checkbox("Audio", &showAudio);
				ImGui::SameLine();
				ImGui::Checkbox("Shada", &showShaders);
				ImGui::SameLine();
				ImGui::Checkbox("Test", &showTest);
				ImGui::SameLine();
				ImGui::Checkbox("FPS", &showFps);
				ImGui::SameLine();
				ImGui::Checkbox("OSC", &showOSC);
				ImGui::SameLine();
				ImGui::Checkbox("Editor", &showTheme);
				if (ImGui::Button("Save Params")) { mParameterBag->save(); }

				// foreground color
				/*static float color[4] = { mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3], mParameterBag->controlValues[4] };
				ImGui::ColorEdit4("f", color);
				mParameterBag->controlValues[1] = color[0];
				mParameterBag->controlValues[2] = color[1];
				mParameterBag->controlValues[3] = color[2];
				mParameterBag->controlValues[4] = color[3];
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3], mParameterBag->controlValues[4]), "fg color");

				// background color
				static float backcolor[4] = { mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7], mParameterBag->controlValues[8] };
				ImGui::ColorEdit4("g", backcolor);
				mParameterBag->controlValues[5] = backcolor[0];
				mParameterBag->controlValues[6] = backcolor[1];
				mParameterBag->controlValues[7] = backcolor[2];
				mParameterBag->controlValues[8] = backcolor[3];
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7], mParameterBag->controlValues[8]), "bg color");*/

			}
			/*if (ImGui::CollapsingHeader("Warps", "2", true, true))
			{
			ImGui::Columns(5, "data", true);
			ImGui::Text("WarpID"); ImGui::NextColumn();
			ImGui::Text("LeftMode"); ImGui::NextColumn();
			ImGui::Text("LeftInput"); ImGui::NextColumn();
			ImGui::Text("rightMode"); ImGui::NextColumn();
			ImGui::Text("rightIndex"); ImGui::NextColumn();
			ImGui::Separator();

			for (int i = 0; i < mTextures->warpInputs.size(); i++)
			{
			ImGui::Text("%d", i);		ImGui::NextColumn();
			ImGui::Text("%d", mTextures->getWarpInput(i).leftMode); ImGui::NextColumn();
			ImGui::Text("%d", mTextures->getWarpInput(i).leftIndex); ImGui::NextColumn();
			//ImGui::Text("%d", mTextures->getWarpInput(i).rightMode); ImGui::NextColumn();
			//static int e = 0;
			//ImGui::RadioButton("texture", &e, 0); ImGui::NextColumn();
			//ImGui::RadioButton("shader", &e, 1); ImGui::NextColumn();
			//ImGui::Text("%d", mTextures->getWarpInput(i).rightIndex); ImGui::NextColumn();
			}
			//RTE because nothing ! mTextures->warpInputs[0].rightMode = 1;
			//mTextures->setWarpInputModeRight(0, 1);


			ImGui::Separator();
			ImGui::Text("Selected warp: %d", mParameterBag->selectedWarp);

			}
			if (ImGui::CollapsingHeader("Log", "4", true, true))
			{

				static ImGuiTextBuffer log;
				static int lines = 0;
				if (ImGui::Button("Clear")) { log.clear(); lines = 0; }
				ImGui::SameLine();
				ImGui::Text("Buffer contents: %d lines, %d bytes", lines, log.size());

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
			}*/

		}
		if (showTest) ImGui::ShowTestWindow();
		ImGui::End();
		// start a new window
		if (showTheme)
		{

			ImGui::Begin("Theme Editor", NULL, ImVec2(200, 100));
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

				if (ImGui::CollapsingHeader("Colors")){

					// add radios on one line to switch between color modes
					static ImGuiColorEditMode colorMode = ImGuiColorEditMode_RGB;
					ImGui::RadioButton("RGB", &colorMode, ImGuiColorEditMode_RGB);
					ImGui::SameLine();
					ImGui::RadioButton("HSV", &colorMode, ImGuiColorEditMode_HSV);
					ImGui::SameLine();
					ImGui::RadioButton("HEX", &colorMode, ImGuiColorEditMode_HEX);

					// change color mode
					ImGui::ColorEditMode(colorMode);

					// add color controls
					ImGui::ColorEdit4("Color0", (float*)&color0, false);
					ImGui::ColorEdit4("Color1", (float*)&color1, false);
					ImGui::ColorEdit4("Color2", (float*)&color2, false);
					ImGui::ColorEdit4("Color3", (float*)&color3, false);
					ImGui::ColorEdit4("Color4", (float*)&color4, false);
				}

				if (ImGui::CollapsingHeader("Layout")){

					// layout controls
					ImGui::SliderFloat2("WindowPadding", WindowPadding, 0, 50, "%.0f");
					ImGui::SliderFloat2("WindowMinSize", WindowMinSize, 0, 250, "%.0f");
					ImGui::SliderFloat2("FramePadding", FramePadding, 0, 40, "%.0f");
					ImGui::SliderFloat2("ItemSpacing", ItemSpacing, 0, 40, "%.0f");
					ImGui::SliderFloat2("ItemInnerSpacing", ItemInnerSpacing, 0, 40, "%.0f");
					ImGui::SliderFloat("WindowFillAlphaDefault", &WindowFillAlphaDefault, 0, 1);
					ImGui::SliderFloat("WindowRounding", &WindowRounding, 0, 15, "%.0f");
					ImGui::SliderFloat("TreeNodeSpacing", &TreeNodeSpacing, 0, 50, "%.0f");
					ImGui::SliderFloat("ColumnsMinSpacing", &ColumnsMinSpacing, 0, 150, "%.0f");
					ImGui::SliderFloat("ScrollBarWidth", &ScrollBarWidth, 0, 35, "%.0f");
				}


				// add a checkbox
				static bool apply = false;
				ImGui::Checkbox("Apply", &apply);
				ImGui::SameLine();
				if (ImGui::Button("Reset")){
					color0 = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
					color1 = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
					color2 = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
					color3 = ImVec4(0.69f, 0.69f, 0.69f, 1.0f);
					color4 = ImVec4(0.27f, 0.27f, 0.27f, 1.0f);

					WindowPadding[0] = WindowPadding[1] = 8;
					WindowMinSize[0] = WindowMinSize[1] = 48;
					FramePadding[0] = 5; FramePadding[1] = 4;
					ItemSpacing[0] = 10; ItemSpacing[1] = 5;
					ItemInnerSpacing[0] = ItemInnerSpacing[1] = 5;

					WindowFillAlphaDefault = 0.70f;
					WindowRounding = 4.0f;
					TreeNodeSpacing = 22.0f;
					ColumnsMinSpacing = 6.0f;
					ScrollBarWidth = 16.0f;
				}

				// show tooltip over checkbox
				if (ImGui::IsHovered()){
					ImGui::SetTooltip("Apply the theme colors");
				}

				// if apply is checked make modifications
				if (apply){
					ImGui::setThemeColor(color0, color1, color2, color3, color4);

					ImGui::GetStyle().WindowPadding = ImVec2(WindowPadding[0], WindowPadding[1]);
					ImGui::GetStyle().WindowMinSize = ImVec2(WindowMinSize[0], WindowMinSize[1]);
					ImGui::GetStyle().FramePadding = ImVec2(FramePadding[0], FramePadding[1]);
					ImGui::GetStyle().ItemSpacing = ImVec2(ItemSpacing[0], ItemSpacing[1]);
					ImGui::GetStyle().ItemInnerSpacing = ImVec2(ItemInnerSpacing[0], ItemInnerSpacing[1]);
					ImGui::GetStyle().WindowFillAlphaDefault = WindowFillAlphaDefault;
					ImGui::GetStyle().WindowRounding = WindowRounding;
					ImGui::GetStyle().TreeNodeSpacing = TreeNodeSpacing;
					ImGui::GetStyle().ColumnsMinSpacing = ColumnsMinSpacing;
					ImGui::GetStyle().ScrollBarWidth = ScrollBarWidth;
				}

				// if log pressed generate code and output to the console
				if (ImGui::Button("Log to Console")){
					console() << "ImVec4 color0 = ImVec4( " << color0.x << ", " << color0.y << ", " << color0.z << ", " << color0.w << " );" << endl
						<< "ImVec4 color1 = ImVec4( " << color1.x << ", " << color1.y << ", " << color1.z << ", " << color1.w << " );" << endl
						<< "ImVec4 color2 = ImVec4( " << color2.x << ", " << color2.y << ", " << color2.z << ", " << color2.w << " );" << endl
						<< "ImVec4 color3 = ImVec4( " << color3.x << ", " << color3.y << ", " << color3.z << ", " << color3.w << " );" << endl
						<< "ImVec4 color4 = ImVec4( " << color4.x << ", " << color4.y << ", " << color4.z << ", " << color4.w << " );" << endl
						<< "ImGui::setThemeColor( color0, color1, color2, color3, color4 );" << endl << endl
						<< "ImGuiStyle& style = ImGui::GetStyle();" << endl
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
				//ImGui::SliderFloat("Background", &f, 0, 1);
			}
			ImGui::End();
		}
		// audio window
		if (showAudio)
		{

			ImGui::Begin("Audio", NULL, ImVec2(200, 100));
			{
				ImGui::Checkbox("Playing", &mParameterBag->mIsPlaying);
				ImGui::SameLine();
				ImGui::Text("Beat %d", mParameterBag->mBeat);
				ImGui::SameLine();
				ImGui::Text("Tempo %.2f", mParameterBag->mTempo);

				static ImVector<float> values; if (values.empty()) { values.resize(40); memset(&values.front(), 0, values.size()*sizeof(float)); }
				static int values_offset = 0;
				// audio maxVolume
				static float refresh_time = -1.0f;
				if (ImGui::GetTime() > refresh_time + 1.0f / 20.0f)
				{
					refresh_time = ImGui::GetTime();
					values[values_offset] = mParameterBag->maxVolume;
					values_offset = (values_offset + 1) % values.size();
				}
				ImGui::PlotLines("Volume", &values.front(), (int)values.size(), values_offset, toString(mBatchass->formatFloat(mParameterBag->maxVolume)).c_str(), 0.0f, 1.0f, ImVec2(0, 70));

				for (int a = 0; a < MAX; a++)
				{
					if (mOSC->tracks[a] != "default.glsl") ImGui::Button(mOSC->tracks[a].c_str());
				}

			}
			ImGui::End();
		}
		// shada window
		if (showShaders)
		{

			ImGui::Begin("Shada", NULL, ImVec2(200, 100));
			{
				for (int a = 0; a < MAX; a++)
				{
					if (mShaders->getShaderName(a) != "default.glsl") ImGui::Button(mShaders->getShaderName(a).c_str());
				}
			}
			ImGui::End();
		}
		// fps window
		if (showFps)
		{
			ImGui::Begin("Fps", NULL, ImVec2(200, 100));
			{
				static ImVector<float> values; if (values.empty()) { values.resize(100); memset(&values.front(), 0, values.size()*sizeof(float)); }
				static int values_offset = 0;
				static float refresh_time = -1.0f;
				if (ImGui::GetTime() > refresh_time + 1.0f / 6.0f)
				{
					refresh_time = ImGui::GetTime();
					values[values_offset] = getAverageFps();
					values_offset = (values_offset + 1) % values.size();
				}

				ImGui::PlotLines("FPS", &values.front(), (int)values.size(), values_offset, toString(floor(getAverageFps())).c_str(), 0.0f, 300.0f, ImVec2(0, 70));
			}
			ImGui::End();
		}
		// osc window
		if (showOSC)
		{

			ImGui::Begin("OSC", NULL, ImVec2(200, 100));
			{
				// osc
				if (mParameterBag->mIsOSCSender)
				{
					ImGui::Text("Sending to host %s", mParameterBag->mOSCDestinationHost.c_str());
					ImGui::SameLine();
					ImGui::Text(" on port %d", mParameterBag->mOSCDestinationPort);
				}
				else
				{
					ImGui::Text("Host %s", mParameterBag->mOSCDestinationHost.c_str());
					ImGui::SameLine();
					ImGui::Text("Port %d", mParameterBag->mOSCDestinationPort);
					ImGui::SameLine();
					/*static char host[128];
					strcpy(host, mParameterBag->mOSCDestinationHost.c_str());
					ImGui::InputText("OSC send to host", host, 128);
					ImGui::InputInt("OSC send on port", &mParameterBag->mOSCDestinationPort);
					ImGui::SameLine();*/
					if (ImGui::Button("OK"))
					{
						//mParameterBag->mOSCDestinationHost = host;
						mOSC->setupSender();
					}
				}
				ImGui::SameLine();
				ImGui::Text("Receiving on port %d", mParameterBag->mOSCReceiverPort);
				static ImGuiTextBuffer OSClog;
				static int lines = 0;
				if (ImGui::Button("Clear")) { OSClog.clear(); lines = 0; }
				ImGui::SameLine();
				ImGui::Text("Buffer contents: %d lines, %d bytes", lines, OSClog.size());

				if (mParameterBag->newOSCMsg)
				{
					mParameterBag->newOSCMsg = false;
					OSClog.append(mParameterBag->OSCMsg.c_str());
					lines++;
					if (lines > 1000) { OSClog.clear(); lines = 0; }
				}
				ImGui::BeginChild("OSClog");
				ImGui::TextUnformatted(OSClog.begin(), OSClog.end());
				ImGui::EndChild();
			}
			ImGui::End();
		}
		ImGui::Render();
	}
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

CINDER_APP_NATIVE(MixnMap, RendererGl)
