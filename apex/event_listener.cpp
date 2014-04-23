//  Copyright (c) 2014 University of Oregon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "event_listener.hpp"
#include "thread_instance.hpp"

/* At some point, make this multithreaded using the multiproducer/singlecomsumer example
 * at http://www.boost.org/doc/libs/1_55_0/doc/html/atomic/usage_examples.html
 */

using namespace std;

namespace apex {

timer_event_data::timer_event_data(event_type event_type_, int thread_id, string timer_name) {
  this->event_type_ = event_type_;
  this->thread_id = thread_id;
  this->timer_name = new string(timer_name);
}

timer_event_data::~timer_event_data() {
  delete(timer_name);
}

node_event_data::node_event_data(int node_id, int thread_id) {
  this->event_type_ = NEW_NODE;
  this->node_id = node_id;
  this->thread_id = thread_id;
}

sample_value_event_data::sample_value_event_data(int thread_id, string counter_name, double counter_value) {
  this->event_type_ = SAMPLE_VALUE;
  this->thread_id = thread_id;
  this->counter_name = new string(counter_name);
  this->counter_value = counter_value;
}

sample_value_event_data::~sample_value_event_data() {
  delete(counter_name);
}

startup_event_data::startup_event_data(int argc, char** argv) {
  this->thread_id = thread_instance::get_id();
  this->event_type_ = STARTUP;
  this->argc = argc;
  this->argv = argv;
}

shutdown_event_data::shutdown_event_data(int node_id, int thread_id) {
  this->event_type_ = SHUTDOWN;
  this->node_id = node_id;
  this->thread_id = thread_id;
}

new_thread_event_data::new_thread_event_data(string thread_name) {
  this->thread_id = thread_instance::get_id();
  this->event_type_ = NEW_THREAD;
  this->thread_name = new string(thread_name);
}

new_thread_event_data::~new_thread_event_data() {
  delete(thread_name);
}

}
