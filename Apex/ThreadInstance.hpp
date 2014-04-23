//  Copyright (c) 2014 University of Oregon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef THREADINSTANCE_H
#define THREADINSTANCE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <boost/thread/tss.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/atomic.hpp>
#include <map>

using namespace std;

namespace apex {

class ThreadInstance {
private:
  // TAU id of the thread
  int _ID;
  // "name" of the thread
  string *_topLevelTimerName;
  // is this an HPX worker thread?
  bool _isWorker;
  // map from name to thread id - common to all threads
  static map<string, int> _nameMap;
  static boost::mutex _nameMapMutex;
  // map from thread id to isWorker
  static map<int, bool> _workerMap;
  static boost::mutex _workerMapMutex;
  static boost::atomic_int _numThreads;
  // thread specific data
  static boost::thread_specific_ptr<ThreadInstance> _instance;
  // constructor
  ThreadInstance (void) : _ID(-1), _topLevelTimerName(NULL), _isWorker(false) { };
public:
  static ThreadInstance* Instance(void);
  static int getID(void);
  static string getName(void);
  static void setName(string name);
  static void setWorker(bool isWorker);
  static int mapNameToID(string name);
  static bool mapIDToWorker(int id);
  static int getNumThreads(void) { return _numThreads; };
};

}

#endif // THREADINSTANCE_H
