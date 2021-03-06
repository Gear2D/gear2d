#include "component.h"
#include "engine.h"
#include "logtrace.h"
#include "object.h"

#include "SDL.h"


#include <iostream>

#define SOPREFIX "lib";
#ifdef _WIN32
# define SOSUFFIX ".dll"
# ifdef _MSC_VER
#   define SOPREFIX ""
# endif
#elif __MACOS__ 
# define SOSUFFIX ".dylib"
#else
# define SOSUFFIX ".so"
#endif

namespace gear2d {
  namespace component {
    
    class globalcomponent : public base {
      private:
        
        /* build separate object and component factory
         * so that we don't mess up users' factories while
         * being protected from their mess */
        component::factory cfactory;
        object::factory ofactory;
        
      public:
        virtual void setup(object::signature & sig) { }
        virtual component::type type() { return "globalcomponent"; }
        globalcomponent()
        : ofactory(cfactory)
        { 
          ofactory.set("globalcomponent", object::signature());
          owner = ofactory.build("globalcomponent");
          
          /* prevent spawning from this object */
          owner->ofactory = nullptr;
        }
        
      public:
        static globalcomponent * instance() {
          static globalcomponent * com = nullptr;
          if (com == nullptr) com = new globalcomponent();
          return com;
        }
    };
    
    component::base & component::base::globals = *globalcomponent::instance();
    
    base::base()
    : cfactory(0)
    {
    }
    
    base::~base() {
    }
    
    parameterbase * base::exists(const parameterbase::id & pid) {
      return owner->get(pid);
    }
    
    component::base * base::build(component::selector s) {
      component::base * com = cfactory->build(s);
      if (com == 0) {
        cfactory->load(s);
        com = cfactory->build(s);
      }
      return com;
    }
    
    object::id base::locate(object::type t) {
      return owner->ofactory->locate(t);
    }
    
    object::id base::spawn(object::type t) {
      object::id obj = 0;
      if (owner->ofactory != 0) {
        obj = owner->ofactory->build(t);
        if (obj == 0) {
          owner->ofactory->load(t);
          obj = owner->ofactory->build(t);
        }
      }
      
      return obj;
    }
    
    component::base * base::clone() {
      object::id other = spawn(owner->name());
      other->copy(owner);
      return other->component(this->family());
    }
    
    void base::destroy() {
      owner->destroy();
    }
    
    void base::handle(parameterbase::id pid, component::base * lastwrite, object::id owner) { ; }
    
    void base::load(std::string scene) {
      modinfo(family() + "/" + type() + "@" + owner->name());
      if (scene.find(".yaml") == std::string::npos) scene += ".yaml";
      engine::next(scene);
    }
    
    /* selector stuff */
    selector::selector() { }
    
    selector::selector(component::family f, component::type t)
    : family(f)
    , type(t) {
      if (t == "") { this->type = f; }
    }
    
    selector::selector(std::string selstring) {
      *this = selstring;
    }
    
    selector & selector::operator=(std::string rhs) {
      if (rhs == "") {
        family = type = "";
        return *this;
      }
      
      std::vector<std::string> splitsel;
      split(splitsel, rhs, '/');
      
      if (splitsel.size() < 1) {
        family = type = "";
        return *this;
      }
      
      family = splitsel[0];
      if (splitsel.size() >= 2) {
        type = splitsel[1];
      } else type = "";
      return *this;
    }
    
    selector & selector::operator=(const selector & rhs) {
      family = rhs.family;
      type = rhs.type;
	  return *this;
    }
    
    bool selector::operator==(const selector & rhs) {
      return ((family == rhs.family) && (type == rhs.type));
    }
    
    bool selector::operator==(std::string rhs) {
      return (((std::string)*this) == rhs);
    }
    
    selector::operator std::string() {
      if (family == "") return "";
      if (type == "") return family;
      return family + "/" + type;
    }
    
    /* factory stuff */
    void factory::set(component::selector s, factory::builder b) {
      component::family f; component::type t;
      f = s.family;
      t = s.type;
      if (t == "") t = f;
      if (b == 0) return;
      builders[f][t] = b;
    }
    
    void factory::load(selector s, std::string file) throw (evil) {
      modinfo("component-factory");
      component::family f; component::type t;
      f = s.family;
      t = s.type;
      if (t == "") t = f;
      std::string buildername = s.family + "_" + s.type + "_build";

#ifdef ANDROID
      void * handle = SDL_LoadObject("libmain.so");
#else
      void * handle = NULL;
#endif

      factory::builder combuilder = (factory::builder)SDL_LoadFunction(handle, buildername.c_str());
      if (combuilder == nullptr) {
        trace("Could not find ", buildername, "internally. Will need to load libraries");
        trace("Error was:", SDL_GetError());
      } else {
        trace("Found", buildername, "internally at address", combuilder);
        handlers[t] = nullptr;
        builders[f][t] = combuilder;
        return;
      }
        
        
      factory::handler comhandler = 0;
      /* TODO: check if we're in windows or linux */
      if (file == "") {
        std::vector<std::string> paths;
        split(paths, compath, ',');
        for (size_t i = 0; i < paths.size(); i++) {
          std::string & path = paths[i];
          file = trim(path) + "/" + f + "/"; file += SOPREFIX; file += t + SOSUFFIX;
          trace("Trying to load", file);
          comhandler = SDL_LoadObject(file.c_str());
          if (!comhandler) {
            trace.w("Error loading", file);
            trace.w("Error:",SDL_GetError());
          }
          if (comhandler != 0) break;
        }
      } else {
        comhandler = SDL_LoadObject(file.c_str());
      }
      
      if (comhandler == 0) {
        throw evil(std::string("Unable to find a suitable library for the component ") + (std::string)s);
        return;
      }
      
      /* get the builder function */
      
      combuilder = (factory::builder)SDL_LoadFunction(comhandler, buildername.c_str());
      if (combuilder == 0) {
        trace.e("I cannot recognize", file, "as an valid gear2d component. It will NOT be loaded");
        trace.e("Error was:", SDL_GetError());
        SDL_UnloadObject(comhandler);
        return;
      }
      
      /* register the handler and the builder */
      handlers[t] = comhandler;
      builders[f][t] = combuilder;
      
      trace("Sucessfully loaded a builder for", t, "from", file);
      
      return;
    }
    
    base * factory::build(component::selector s) {
      modinfo("component-factory");
      component::family f; component::type t;
      f = s.family;
      t = s.type;
      if (t == "") t = f;
      
      factory::builder b;
      
      /* try to locate component based on type */
      buildertable::iterator famit = builders.find(f);
      
      
      if (famit == builders.end()) {
        return 0;
      }
      
      std::map<component::type, factory::builder>::iterator typit = famit->second.end();
      if (f != t) { typit = famit->second.find(t); }
      else if (famit->second.size() > 0) { typit = famit->second.begin(); }
      
      if (typit == famit->second.end()) {
        return 0;
      }
      
      /* build it with the given function */
      b = typit->second;
      base * component = b();
      component->cfactory = this;
      trace.i("Component type", t, "sucessfuly built!");
      return component;
    }
    
    factory::~factory() {
      while (!handlers.empty()) {
//      factory::handler handler = handlers.begin()->second;
//      SDL_UnloadObject(handler); TODO this crashes. why?
        handlers.erase(handlers.begin());
      }
    }
  }
}
