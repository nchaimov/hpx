//  Copyright (c) 2014 University of Oregon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef TAUHANDLER_HPP
#define TAUHANDLER_HPP

#include "EventListener.hpp"

using namespace std;

namespace apex {

class TauListener : public EventListener {
private:
  void _init(void);
  bool _terminate;
public:
  TauListener (void);
  ~TauListener (void) { };
  void onEvent(EventData* eventData);
};

}

#endif // TAUHANDLER_HPP
