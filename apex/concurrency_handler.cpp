//  Copyright (c) 2014 University of Oregon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#include "concurrency_handler.hpp"
#include "thread_instance.hpp"
#include <iostream>
#include <map>
#include <iterator>
#include <iostream>
#include <fstream>
#if defined(__GNUC__)
#include <cxxabi.h>
#endif

using namespace std;

namespace apex {

concurrency_handler::concurrency_handler (void) : handler() {
  _init();
}

concurrency_handler::concurrency_handler (char *option) : handler(), _option(atoi(option)) {
  _init();
}

concurrency_handler::concurrency_handler (unsigned int period) : handler(period) {
  _init();
}

concurrency_handler::concurrency_handler (unsigned int period, char *option) : handler(period), _option(atoi(option)) {
  _init();
}

void concurrency_handler::_handler(void) {
  //cout << "HANDLER: " << endl;
  map<string, unsigned int> *counts = new(map<string, unsigned int>);
  stack<string>* tmp;
  boost::mutex* mut;
  for (unsigned int i = 0 ; i < _event_stack.size() ; i++) {
    if (_option > 1 && !thread_instance::map_id_to_worker(i)) {
      continue;
    }
    tmp = get_event_stack(i);
    if (tmp->size() > 0) {
      mut = get_event_stack_mutex(i);
      mut->lock();
      string func = tmp->top();
      mut->unlock();
      _function_mutex.lock();
      _functions.insert(func);
      _function_mutex.unlock();
      if (counts->find(func) == counts->end()) {
        (*counts)[func] = 1;
      } else {
        (*counts)[func] = (*counts)[func] + 1;
      }
    }
  }
  _states.push_back(counts);
  this->reset();
  return;
}

void concurrency_handler::_init(void) {
  add_thread(0);
  _timer.async_wait(boost::bind(&concurrency_handler::_handler, this));
  run();
  return;
}

void concurrency_handler::on_event(event_data* event_data_) {
  stack<string>* my_stack;
  boost::mutex* my_mut;
  if (!_terminate) {
    switch(event_data_->event_type_) {

      case START_EVENT:
      {
        timer_event_data *starter = (timer_event_data*)event_data_;
        my_stack = get_event_stack(starter->thread_id);
        my_mut = get_event_stack_mutex(starter->thread_id);
        my_mut->lock();
        my_stack->push(*(starter->timer_name));
        my_mut->unlock();
        break;
      }

      case STOP_EVENT:
      {
        timer_event_data *stopper = (timer_event_data*)event_data_;
        my_stack = get_event_stack(stopper->thread_id);
        my_mut = get_event_stack_mutex(stopper->thread_id);
        my_mut->lock();
        my_stack->pop();
        my_mut->unlock();
        break;
      }

      case NEW_THREAD:
      {
        new_thread_event_data *newthread = (new_thread_event_data*)event_data_;
        add_thread(newthread->thread_id);
        break;
      }

      case SHUTDOWN:
      {
        shutdown_event_data *shutdown = (shutdown_event_data*)event_data_;
        _terminate = true;
        output_samples(shutdown->node_id);
        break;
      }

      default:
      {
        break;
      }

    }
  }
  return;
}

inline stack<string>* concurrency_handler::get_event_stack(unsigned int tid) {
  stack<string>* tmp;
  _vector_mutex.lock();
  tmp = this->_event_stack[tid];
  _vector_mutex.unlock();
  return tmp;
}

inline boost::mutex* concurrency_handler::get_event_stack_mutex(unsigned int tid) {
  boost::mutex* tmp;
  _vector_mutex.lock();
  tmp = this->_stack_mutex[tid];
  _vector_mutex.unlock();
  return tmp;
}

inline void concurrency_handler::reset(void) {
  if (!_terminate) {
    _timer.expires_at(_timer.expires_at() + boost::posix_time::microseconds(_period));
    _timer.async_wait(boost::bind(&concurrency_handler::_handler, this));
  }
}

inline void concurrency_handler::add_thread(unsigned int tid) {
  _vector_mutex.lock();
  while(_event_stack.size() <= tid) {
    _event_stack.push_back(new stack<string>);
    _stack_mutex.push_back(new boost::mutex);
  }
  _vector_mutex.unlock();
}

string* demangle(string timer_name) {
  string* demangled = NULL;
#if defined(__GNUC__)
  int     status;
  char *realname = abi::__cxa_demangle(timer_name.c_str(), 0, 0, &status);
  if (status == 0) {
    char* index = strstr(realname, "<");
    if (index != NULL) {
      *index = 0; // terminate before templates for brevity
    }
    demangled = new string(realname);
    free(realname);
  } else {
    demangled = new string(timer_name);
  }
#else
  demangled = new string(timer_name);
#endif
  return demangled;
}

bool sort_functions(pair<string,int> first, pair<string,int> second) {
  if (first.second > second.second)
    return true;
  return false;
}

void concurrency_handler::output_samples(int node_id) {
  //cout << _states.size() << " samples seen:" << endl;
  ofstream myfile;
  stringstream datname;
  datname << "concurrency." << node_id << ".dat";
  myfile.open(datname.str().c_str());
  _function_mutex.lock();
  // limit ourselves to 5 functions.
  map<string, int> func_count;
  // initialize the map
  for (set<string>::iterator it=_functions.begin(); it!=_functions.end(); ++it) {
    func_count[*it] = 0;
  }
  // count all function instances
  for (unsigned int i = 0 ; i < _states.size() ; i++) {
    for (set<string>::iterator it=_functions.begin(); it!=_functions.end(); ++it) {
      if (_states[i]->find(*it) == _states[i]->end()) {
        continue;
      } else {
        func_count[*it] = func_count[*it] + (*(_states[i]))[*it];
      }
    }
  }
  // sort the map
  vector<pair<string,int> > my_vec(func_count.begin(), func_count.end());
  sort(my_vec.begin(),my_vec.end(),&sort_functions);
  set<string> top_x;
  for (vector<pair<string, int> >::iterator it=my_vec.begin(); it!=my_vec.end(); ++it) {
    //if (top_x.size() < 15 && (*it).first != "APEX THREAD MAIN")
    if (top_x.size() < 15)
      top_x.insert((*it).first);
  }

  // output the header
  for (set<string>::iterator it=_functions.begin(); it!=_functions.end(); ++it) {
    if (top_x.find(*it) != top_x.end()) {
      string* tmp = demangle(*it);
      myfile << "\"" << *tmp << "\"\t";
      delete (tmp);
    }
  }
  myfile << "\"other\"" << endl;

  size_t max_Y = 0;
  size_t max_X = _states.size();
  for (size_t i = 0 ; i < max_X ; i++) {
    unsigned int tmp_max = 0;
    int other = 0;
    for (set<string>::iterator it=_functions.begin(); it!=_functions.end(); ++it) {
      // this is the idle event.
      //if (*it == "APEX THREAD MAIN")
        //continue;
      int value = 0;
      // did we see this timer during this sample?
      if (_states[i]->find(*it) != _states[i]->end()) {
        value = (*(_states[i]))[*it];
      }
      // is this timer in the top X?
      if (top_x.find(*it) == top_x.end()) {
        other = other + value;
      } else {
        myfile << (*(_states[i]))[*it] << "\t";
        tmp_max += (*(_states[i]))[*it];
      }
    }
    myfile << other << "\t" << endl;
    tmp_max += other;
    if (tmp_max > max_Y) max_Y = tmp_max;
  }
  _function_mutex.unlock();
  myfile.close();

  stringstream plotname;
  plotname << "concurrency." << node_id << ".gnuplot";
  myfile.open(plotname.str().c_str());
  myfile << "set key outside bottom center invert box" << endl;
  myfile << "unset xtics" << endl;
  myfile << "set xrange[0:" << max_X << "]" << endl;
  myfile << "set yrange[0:" << max_Y << "]" << endl;
  myfile << "set xlabel \"Time\"" << endl;
  myfile << "set ylabel \"Concurrency\"" << endl;
  myfile << "# Select histogram data" << endl;
  myfile << "set style data histogram" << endl;
  myfile << "# Give the bars a plain fill pattern, and draw a solid line around them." << endl;
  myfile << "set style fill solid border" << endl;
  myfile << "set style histogram rowstacked" << endl;
  myfile << "set boxwidth 1.0 relative" << endl;
  myfile << "set palette rgb 33,13,10" << endl;
  myfile << "unset colorbox" << endl;
  myfile << "plot for [COL=1:" << top_x.size()+1;
  myfile << "] '" << datname.str().c_str();
  myfile << "' using COL:xticlabels(1) palette frac COL/" << top_x.size()+1;
  myfile << ". title columnheader" << endl;
  myfile.close();
}

}
