/**
* This file is part of OSC To Spout.
* It is released under the terms of the MIT license
*
* Copyright (c) 2019 -- Élie Michel <elie.michel@exppad.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* The Software is provided “as is”, without warranty of any kind, express or
* implied, including but not limited to the warranties of merchantability,
* fitness for a particular purpose and non-infringement. In no event shall the
* authors or copyright holders be liable for any claim, damages or other
* liability, whether in an action of contract, tort or otherwise, arising
* from, out of or in connection with the software or the use or other dealings
* in the Software.
*/

#include "ofApp.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Settings

/**
 * Maps osc message to texture line
 */
const std::map<std::string, size_t> addr_to_index{
	{ "/time", 0 },
	{ "/beat1", 1 },
	{ "/beat2", 2 },
	{ "/beat3", 3 },
	{ "/beat4", 4 },
	{ "/beat5", 5 },
	{ "/beat6", 6 },
	{ "/beat7", 7 },
	{ "/beat8", 8 },
};

/**
 * Texture size. Each line correspond to a message address, and each column to
 * a message argument.
 */
constexpr size_t width = 4; // number fo columns
constexpr size_t height = 9; // number of lines

/**
 * Port on which listening for OSC messages.
 */
constexpr int osc_port = 3615;

//
///////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetWindowTitle("OSC To Spout");

	if (!osc.setup(osc_port))
	{
		std::cout << "Could not start listening OSC messages on port " << osc_port << "!" << std::endl;
		OF_EXIT_APP(1);
	}
	std::cout << "Listening for OSC messages on port 3615..." << std::endl;

	spout.init("OSC To Spout");
	std::cout << "Emitting Spout stream on channel 'OSC To Spout'..." << std::endl;

	pixels.allocate(width, height, OF_IMAGE_GRAYSCALE);
	texture.allocate(pixels);
	texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	if (!texture.isAllocated())
	{
		std::cout << "Could not allocate texture!" << std::endl;
		OF_EXIT_APP(1);
	}
	std::cout << "Streamed texture has size 4x9" << std::endl;
}

//--------------------------------------------------------------
void ofApp::update() {
	if (osc.hasWaitingMessages())
	{
		texture.readToPixels(pixels);
		ofxOscMessage msg;
		while (osc.getNextMessage(msg))
		{
			std::string addr = msg.getAddress();
			if (addr_to_index.count(addr) == 0) {
				ofLogWarning() << "Ignoring unknown message received at address " + addr;
				continue;
			}
			size_t idx = addr_to_index.at(addr);
			if (idx >= height) {
				ofLogWarning() << "Line index out of bound: " << idx << " (should be bellow " << height << "). There may be a mistake in addr_to_index.";
				continue;
			}
			for (size_t k = 0; k < std::min(width, msg.getNumArgs()); ++k) {
				pixels[idx * width + k] = msg.getArgAsFloat(k);
			}
		}
		texture.loadData(pixels);
	}

	spout.send(texture);
}

//--------------------------------------------------------------
void ofApp::draw() {
	texture.draw(0, 0, ofGetViewportWidth(), ofGetViewportHeight());
}
