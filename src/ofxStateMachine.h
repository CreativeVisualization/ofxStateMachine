/*
 *  StateMachine.h
 *
 *  Copyright (c) 2011, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of 16b.it nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#pragma once

#include <tr1/memory>
#include <map>
#include <string>
#include <iostream>
#include "ofxState.h"

using namespace std;
using namespace tr1;

namespace Apex
{	
	template<class SharedData = ofxEmptyData>
	class ofxStateMachine
	{
	public:
		typedef shared_ptr< ofxState<SharedData> > statePtr;
		typedef typename map<string, statePtr>::iterator stateIt;
		
		ofxStateMachine()
		{
			enableAppEvents();
#ifdef TARGET_OPENGLES
			enableTouchEvents();
#else
			enableMouseEvents();
#endif
		}
		
		/** State Stuff **/ 
		void addState(ofxState<SharedData>* state)
		{
			// we call setup here rather than use the setup event in case the state is added after
			// setup event has occured
			state->setSharedData(&sharedData);
			state->setup();
			ofAddListener(state->changeStateEvent, this, &ofxStateMachine::onChangeState);
			states.insert(make_pair(state->getName(), statePtr(state)));
		}
		
		SharedData& getSharedData()
		{
			return sharedData;
		}		
		
		void onChangeState(string& stateName)
		{
			changeState(stateName);
		}
		
		void changeState(const string& name)
		{
			stateIt it = states.find(name);
			if (it == states.end()) ofLog(OF_LOG_ERROR, "No state with name: %s.  Make sure you have added it to the state machine and you have set the state's name correctly.  Set the name by implementing \"const string getName()\" in your state class", name.c_str());
			else if (it->second != currentState)
			{
				if (currentState) currentState->stateExit();
				currentState = it->second;
				currentState->stateEnter();
			}
		}
		
		/** App Event Stuff **/
		void enableAppEvents()
		{
			ofAddListener(ofEvents.update, this, &ofxStateMachine::onUpdate);
			ofAddListener(ofEvents.draw, this, &ofxStateMachine::onDraw);
		}
		
		void onUpdate(ofEventArgs &data) { update(); }
		void onDraw(ofEventArgs &data) { draw(); }
		
		void update()
		{
			if (currentState) currentState->update();
			else ofLog(OF_LOG_WARNING, "State machine update called with no state set");
		}
		
		void draw()
		{
			if (currentState) currentState->draw();
			else ofLog(OF_LOG_WARNING, "State machine draw called with no state set");
		}
		
#ifdef TARGET_OPENGLES
		/** Touch Event Stuff **/
		void enableTouchEvents()
		{
			ofAddListener(ofEvents.touchUp, this, &ofxStateMachine::onTouchUp);
			ofAddListener(ofEvents.touchDown, this, &ofxStateMachine::onTouchDown);
			ofAddListener(ofEvents.touchMoved, this, &ofxStateMachine::onTouchMoved);
			ofAddListener(ofEvents.touchCancelled, this, &ofxStateMachine::onTouchCancelled);
			ofAddListener(ofEvents.touchDoubleTap, this, &ofxStateMachine::onTouchDoubleTap);
		}
		
		void onTouchUp(ofTouchEventArgs &data) { if (currentState) currentState->touchUp(data); }
		void onTouchDown(ofTouchEventArgs &data) { if (currentState) currentState->touchDown(data); }
		void onTouchMoved(ofTouchEventArgs &data) { if (currentState) currentState->touchMoved(data); }
		void onTouchCancelled(ofTouchEventArgs &data) { if (currentState) currentState->touchCancelled(data); }
		void onTouchDoubleTap(ofTouchEventArgs &data) { if (currentState) currentState->touchDoubleTap(data); }
#else
		/** Mouse Event Stuff **/
		void enableMouseEvents()
		{
			ofAddListener(ofEvents.mouseReleased, this, &ofxStateMachine::onMouseReleased);
			ofAddListener(ofEvents.mousePressed, this, &ofxStateMachine::onMousePressed);
			ofAddListener(ofEvents.mouseMoved, this, &ofxStateMachine::onMouseMoved);
			ofAddListener(ofEvents.mouseDragged, this, &ofxStateMachine::onMouseDragged);
		}
		
		void onMouseReleased(ofMouseEventArgs& data) { if (currentState) currentState->mouseReleased(data); }
		void onMousePressed(ofMouseEventArgs& data) { if (currentState) currentState->mousePressed(data); }
		void onMouseMoved(ofMouseEventArgs& data) { if (currentState) currentState->mouseMoved(data); }
		void onMouseDragged(ofMouseEventArgs& data) { if (currentState) currentState->mouseDragged(data); }
#endif
		
	private:
		statePtr currentState;
		map<string, shared_ptr< ofxState<SharedData> > > states;
		SharedData sharedData;
	};
}
