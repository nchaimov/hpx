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
#include <boost/thread/mutex.hpp>

#ifdef SIGEV_THREAD_ID
#ifndef sigev_notify_thread_id
#define sigev_notify_thread_id _sigev_un._tid
#endif /* ifndef sigev_notify_thread_id */
#endif /* ifdef SIGEV_THREAD_ID */

using namespace std;

namespace apex {

class policy_handler : public handler, public event_listener {
private:
  void _init(void);
public:
  policy_handler (void);
  policy_handler (unsigned int period);
  ~policy_handler (void) { };
  void on_event(event_data* event_data_);
  void _handler(void);
};

}

#endif // POLICYHANDLER_HPP
