//  Copyright (c) 2014 University of Oregon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "PolicyHandler.hpp"
#include "ThreadInstance.hpp"

#include <iostream>

using namespace std;

namespace apex {

PolicyHandler::PolicyHandler (void) : Handler() { 
  _init(); 
}

PolicyHandler::PolicyHandler (unsigned int period) : Handler(period) { 
  _init(); 
}

void PolicyHandler::_handler(void) {
  return;
}

void PolicyHandler::_init(void) {
  _timer.async_wait(boost::bind(&PolicyHandler::_handler, this));
  run();
  return;
}

void PolicyHandler::onEvent(EventData* eventData) {
  unsigned int tid = ThreadInstance::getID();
  if (!_terminate) {
    switch(eventData->eventType) {
    case STARTUP: {
    	cerr << tid << ": STARTUP" << endl;
    	break;
    }
    case SHUTDOWN: {
    	_terminate = true;
    	cerr << tid << ": SHUTDOWN" << endl;
    	break;
    }
    case NEW_NODE: {
    	cerr << tid << ": NEW_NODE" << endl;
    	break;
    }
    case NEW_THREAD: {
    	cerr << tid << ": NEW_THREAD" << endl;
    	break;
    }
    case START_EVENT: {
    	cerr << tid << ": START_EVENT" << endl;
    	break;
    }
    case STOP_EVENT: {
    	cerr << tid << ": STOP_EVENT" << endl;
    	break;
    }
    case SAMPLE_VALUE: {
    	cerr << tid << ": SAMPLE_EVENT" << endl;
    	break;
    }

    }
  }
  return;
}

}
