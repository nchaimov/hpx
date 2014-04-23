//  Copyright (c) 2014 University of Oregon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "policy_handler.hpp"
#include "thread_instance.hpp"

#include <iostream>

using namespace std;

namespace apex {

policy_handler::policy_handler (void) : handler() {
  _init();
}

policy_handler::policy_handler (unsigned int period) : handler(period) {
  _init();
}

void policy_handler::_handler(void) {
  return;
}

void policy_handler::_init(void) {
  _timer.async_wait(boost::bind(&policy_handler::_handler, this));
  run();
  return;
}

void policy_handler::on_event(event_data* event_data_) {
  unsigned int tid = thread_instance::get_id();
  if (!_terminate) {
    switch(event_data_->event_type_) {
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
