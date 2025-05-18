/*

	Spout OpenFrameworks Receiver/Sender

	Copyright (C) 2019-2021 Lynn Jarvis.

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#pragma once

#include "ofMain.h"
#include "..\..\..\SpoutGL\Spout.h"
#include "ofxWinMenu.h" // Addon for a windows menu
#include "resource.h" // Dialogs etc

class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();
		void exit();
		void mousePressed(int x, int y, int button);
		void keyPressed(int key);

		Spout receiver; // Receiver
		Spout sender; // Sender
		ofTexture myTexture; // Display texture
		bool bShowInfo = true; // Show on-screen information
		void showInfo(); // On-screen information
		void OpenSender(); // Select sender

		// For ofxWinMenu
		HWND g_hWnd = nullptr;
		HINSTANCE g_hInstance = nullptr;
		ofTrueTypeFont myFont;
		ofxWinMenu* menu = nullptr;
		void appMenuFunction(std::string title, bool bChecked);


};
