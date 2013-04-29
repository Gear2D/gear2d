#include "parameter.h"
#include "component.h"

namespace gear2d {
  /** @class context
   *  @brief Represents the globally acessible context.
   */
  class context {
    private:
      static parameterbase::table parameters;

    private:
      component::base * writer;

    public:
      template <typename T>
      link<T> operator()(parameterbase::id & id) {
        parameter<T> * v = parameters[pid];
        if (v == NULL) { 
          v = new parameter<T>();
          parameters[pid] = v;
        }
        return link<T>(v, writer);
      }

      template <typename T>
      context & operator()(parameterbase::id & id, T & source) {
        parameter<T> * v = parameters[pid];
        if (v == NULL) { parameters[pid] = new parameter<T>(source); }
        else { v->set(source); }
      }
  };
}