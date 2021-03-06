#include "ptest.h"
#include "Cello.h"

#if defined(__unix__)
#include <unistd.h>
#elif defined(_WIN32)
#undef in
#undef data
#include <windows.h>
#define in ,
#define data typedef struct
#endif

void cello_sleep(int ms) {
#if defined(__unix__)
  usleep(ms * 1000);
#elif defined(_WIN32)
  Sleep(ms);
#endif
}

PT_SUITE(suite_threading) {
  
  PT_TEST(test_new) {
    
    var in_func = $(Reference, False);
    
    lambda(f, args) {
      set(in_func, 0, True);
      return None;
    };
    
    var t = new(Thread, f);
    
    PT_ASSERT(not at(in_func, 0));
    call(t, None);
    join(t);
    PT_ASSERT(at(in_func, 0));
    
    delete(t);
  
  }
  
  PT_TEST(test_multiple) {
    
    var inside = new(List, 5, False, False, False, False, False);
    
    lambda(f, args) {
      set(inside, as_long(at(args, 0)), True);
      return None;
    };
    
    var threads = new(List, 5,
      new(Thread, f), new(Thread, f),
      new(Thread, f), new(Thread, f),
      new(Thread, f));
    
    var args = new(Array, Int, 5,
      $(Int, 0), $(Int, 1), $(Int, 2),
      $(Int, 3), $(Int, 4));
    
    for(int i = 0; i < len(threads); i++) {
      call(at(threads, i), at(args, i));
    }
    
    foreach(t in threads) {
      join(t);
      delete(t);
    }
    
    foreach(i in inside) {
      PT_ASSERT(i);
    }
    
    delete(args);
    delete(threads);
    delete(inside);
    
  }
  
  PT_TEST(test_mutex) {
    
    var mutex = new(Mutex);
    var total = $(Int, 0);
    
    lambda(f, args) {
      with(m in mutex) {
        add(total, $(Int, 1));
      }
      return None;
    };
    
    var threads = new(List, 5,
      new(Thread, f), new(Thread, f),
      new(Thread, f), new(Thread, f),
      new(Thread, f));
    
    PT_ASSERT(eq(total, $(Int, 0)));
    
    foreach(t in threads) {
      call(t, None);
    }
    
    foreach(t in threads) {
      join(t);
      delete(t);
    }
    
    PT_ASSERT(eq(total, $(Int, 5)));
    
    delete(threads);
    delete(mutex);
    
  }
  
  PT_TEST(test_exception) {
    
    lambda(f, args) {
      try {
        cello_sleep(20);
        PT_ASSERT(Exception_Depth() is 1);
      } catch(e) { }
      return None;
    };
    
    var t = new(Thread, f);
    
    call(t, None);
    cello_sleep(10);
    PT_ASSERT(Exception_Depth() is 0);
    join(t);
    
    delete(t);
    
  }
  
};
