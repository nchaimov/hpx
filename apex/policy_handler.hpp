//  Copyright (c) 2014 University of Oregon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef POLICYHANDLER_HPP
#define POLICYHANDLER_HPP

#include "handler.hpp"
#include "event_listener.hpp"
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <list>

#include <boost/atomic/atomic.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/shared_ptr.hpp>

#ifdef SIGEV_THREAD_ID
#ifndef sigev_notify_thread_id
#define sigev_notify_thread_id _sigev_un._tid
#endif /* ifndef sigev_notify_thread_id */
#endif /* ifdef SIGEV_THREAD_ID */

namespace apex {

struct policy_instance {
  int id;
  bool (*test_function)(void* arg1);
  void (*action_function)(void* arg2);

  policy_instance(int id_, bool (*test_function_)(void* arg1),
    void (*action_function_)(void* arg2)) : id(id_),
    test_function(test_function_), action_function(action_function_) {};
};

class policy_handler : public handler, public event_listener {
private:
  typedef boost::shared_mutex mutex_type;

  void _init(void);
  std::list<boost::shared_ptr<policy_instance> > startup_policies;
  std::list<boost::shared_ptr<policy_instance> > shutdown_policies;
  std::list<boost::shared_ptr<policy_instance> > new_node_policies;
  std::list<boost::shared_ptr<policy_instance> > new_thread_policies;
  std::list<boost::shared_ptr<policy_instance> > start_event_policies;
  std::list<boost::shared_ptr<policy_instance> > stop_event_policies;
  std::list<boost::shared_ptr<policy_instance> > sample_value_policies;
  std::list<boost::shared_ptr<policy_instance> > periodic_policies;
  mutex_type startup_mutex;
  mutex_type shutdown_mutex;
  mutex_type new_node_mutex;
  mutex_type new_thread_mutex;
  mutex_type start_event_mutex;
  mutex_type stop_event_mutex;
  mutex_type sample_value_mutex;
  mutex_type periodic_mutex;
  void call_policies(
    const std::list<boost::shared_ptr<policy_instance> > & policies,
    event_data * event_data_);
  boost::atomic_int next_id;
public:
  policy_handler (void);
  policy_handler (unsigned int period);
  ~policy_handler (void) { };
  void on_event(event_data* event_data_);
  int register_policy(const std::set<_event_type> & when,
                      bool (*test_function)(void* arg1),
                      void (*action_function)(void* arg2));
  void _handler(void);
  void reset(void);
};

}

#endif // POLICYHANDLER_HPP
