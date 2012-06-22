#include "parameter.h"
#include "component.h"
#define CALLBACK(object,ptrToMember)  ((object).*(ptrToMember))



namespace gear2d {
  class parameterbase::callback {
    public:
      callback(component::base * com, component::call h)
      : com(com)
      , fp(h) { }
      
      void operator()(std::string pid, component::base * lastwrite, gear2d::object * owner) {
        if (com == 0) {
          throw evil("callback with null component");
        }
        if (fp == 0) {
          com->handle(pid, lastwrite, owner);
        } else {
          CALLBACK(*com, fp)(pid, lastwrite, owner);
        }
      }
      
    private:
      component::base * com;
      component::call fp;
  };
  
  void parameterbase::hook(component::base * c) {
    hook(c, 0);
  }
  
  void parameterbase::hook(component::base * c, component::call handlefp) {
    hooked.insert(new callback(c, handlefp));
  }
  
  void parameterbase::pull() {
    for (std::set<callback *>::iterator i = hooked.begin(); i != hooked.end(); i++) {
      if (*i == NULL) continue;
      callback & c = *(*i);
      c(pid, lastwrite, owner);
    }
  }
}