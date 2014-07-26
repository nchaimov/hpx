//  Copyright (c) 2014 University of Oregon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "policy_handler.hpp"
#include "thread_instance.hpp"

#include <iostream>

#include <boost/make_shared.hpp>

using namespace std;

namespace apex {

policy_handler::policy_handler (void) : handler() {
  _init();
}

policy_handler::policy_handler (unsigned int period) : handler(period) {
  _init();
}

void policy_handler::_handler(void) {
  periodic_event_data data;
  this->on_event(&data);
  this->reset();
}

void policy_handler::_init(void) {
  next_id = 0;
  _timer.async_wait(boost::bind(&policy_handler::_handler, this));
  run();
  return;
}

inline void policy_handler::reset(void) {
  if (!_terminate) {
    _timer.expires_at(_timer.expires_at() + boost::posix_time::microseconds(_period));
    _timer.async_wait(boost::bind(&policy_handler::_handler, this));
  }
}

int policy_handler::register_policy(const std::set<_event_type> & when,
    bool (*test_function)(void* arg1),
    void (*action_function)(void* arg2)) {
  int id = next_id++;
  boost::shared_ptr<policy_instance> instance(
    boost::make_shared<policy_instance>(id, test_function, action_function));
  for(event_type type : when) {
    switch(type) {
      case STARTUP: {
        boost::unique_lock<mutex_type> l(startup_mutex);
        startup_policies.push_back(instance);
        break;
      }
      case SHUTDOWN: {
        boost::unique_lock<mutex_type> l(shutdown_mutex);
        shutdown_policies.push_back(instance);
        break;
      }
      case NEW_NODE: {
        boost::unique_lock<mutex_type> l(new_node_mutex);
        new_node_policies.push_back(instance);
        break;
      }
      case NEW_THREAD: {
        boost::unique_lock<mutex_type> l(new_thread_mutex);
        new_thread_policies.push_back(instance);
        break;
      }
      case START_EVENT: {
        boost::unique_lock<mutex_type> l(start_event_mutex);
        start_event_policies.push_back(instance);
        break;
      }
      case STOP_EVENT: {
        boost::unique_lock<mutex_type> l(stop_event_mutex);
        stop_event_policies.push_back(instance);
        break;
      }
      case SAMPLE_VALUE: {
        boost::unique_lock<mutex_type> l(sample_value_mutex);
        sample_value_policies.push_back(instance);
        break;
      }
      case PERIODIC: {
        boost::unique_lock<mutex_type> l(periodic_mutex);
        periodic_policies.push_back(instance);
        break;
      }

    }
  }
  return id;

}

void policy_handler::call_policies(
    const std::list<boost::shared_ptr<policy_instance> > & policies,
    event_data* event_data_) {
  for(const boost::shared_ptr<policy_instance>& policy : policies) {
    const bool result = policy->test_function(event_data_);
    if(result) {
      policy->action_function(event_data_);
    }
  }
}

void policy_handler::on_event(event_data* event_data_) {
  //unsigned int tid = thread_instance::get_id();
  if (!_terminate) {
    switch(event_data_->event_type_) {
    case STARTUP: {
        std::list<boost::shared_ptr<policy_instance> > policies;
        {
            boost::shared_lock<mutex_type> l(startup_mutex);
            if (startup_policies.empty())
                return;
            policies = startup_policies;
        }
        call_policies(policies, event_data_);
        break;
    }
    case SHUTDOWN: {
        _terminate = true;
        std::list<boost::shared_ptr<policy_instance> > policies;
        {
            boost::shared_lock<mutex_type> l(shutdown_mutex);
            if (shutdown_policies.empty())
                return;
            policies = shutdown_policies;
        }
        call_policies(policies, event_data_);
        break;
    }
    case NEW_NODE: {
        std::list<boost::shared_ptr<policy_instance> > policies;
        {
            boost::shared_lock<mutex_type> l(new_node_mutex);
            if (new_node_policies.empty())
                return;
            policies = new_node_policies;
        }
        call_policies(policies, event_data_);
        break;
    }
    case NEW_THREAD: {
        std::list<boost::shared_ptr<policy_instance> > policies;
        {
            boost::shared_lock<mutex_type> l(new_thread_mutex);
            if (new_thread_policies.empty())
                return;
            policies = new_thread_policies;
        }
        call_policies(policies, event_data_);
        break;
    }
    case START_EVENT: {
        std::list<boost::shared_ptr<policy_instance> > policies;
        {
            boost::shared_lock<mutex_type> l(start_event_mutex);
            if (start_event_policies.empty())
                return;
            policies = start_event_policies;
        }
        call_policies(policies, event_data_);
        break;
    }
    case STOP_EVENT: {
        std::list<boost::shared_ptr<policy_instance> > policies;
        {
            boost::shared_lock<mutex_type> l(stop_event_mutex);
            if (stop_event_policies.empty())
                return;
            policies = stop_event_policies;
        }
        call_policies(policies, event_data_);
        break;
    }
    case SAMPLE_VALUE: {
        std::list<boost::shared_ptr<policy_instance> > policies;
        {
            boost::shared_lock<mutex_type> l(sample_value_mutex);
            if (sample_value_policies.empty())
                return;
            policies = sample_value_policies;
        }
        call_policies(policies, event_data_);
        break;
    }
    case PERIODIC: {
        std::list<boost::shared_ptr<policy_instance> > policies;
        {
            boost::shared_lock<mutex_type> l(periodic_mutex);
            if (periodic_policies.empty())
                return;
            policies = periodic_policies;
        }
        call_policies(policies, event_data_);
        break;
    }
    } //end switch
  } // end if
  return;
}

} // end namespace apex

