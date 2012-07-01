#include "component.h"
#include "engine.h"
#include "log.h"
#include "SDL.h"

#include <iostream>

#ifdef _WIN32
#define SOSUFFIX ".dll"
#elif __MACOS__ 
#define SOSUFFIX ".dylib"
#else
#define SOSUFFIX ".so"
#endif

namespace gear2d {
  namespace component {
    base::base() : cfactory(0) {
    }
    
    base::~base() {
    }
    
    parameterbase * base::exists(parameterbase::id pid) {
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
      logverb;
      component::family f; component::type t;
      f = s.family;
      t = s.type;
      if (t == "") t = f;
      
      factory::handler comhandler = 0;
      /* TODO: check if we're in windows or linux */
      if (file == "") {
        std::vector<std::string> paths;
        split(paths, compath, ',');
        for (int i = 0; i < paths.size(); i++) {
          file = paths[i] + '/' + f + "/lib" + t + SOSUFFIX;
//           std::cout << "trying " << file << std::endl;
          comhandler = SDL_LoadObject(file.c_str());
          if (comhandler != 0) break;
//           else if (t == f) {
//             /* might be the case where a component like t doesn't exists
//              * but there's another */
//             path p(paths[i] + '/' + f + "/");
//             std::set<path> founds;
//             try {
//               copy(directory_iterator(p), directory_iterator(), std::insert_iterator<std::set<path> >(founds, founds.begin()));
//             } catch (...) {
//               // do nothing actually, probably the path doesn't exists
//             }
//             // set makes sure the founds path is sorted
//             
//             if (founds.size() == 0) continue; // try the next... who know
//             p = *founds.begin();
//             file = p.string();
//             // so load it again :D
//             std::cout << "trying " << file << " (default search)" << std::endl;
//             comhandler = SDL_LoadObject(p.string().c_str());
//             if (comhandler != 0) break;
//           }
        }
      } else {
        comhandler = SDL_LoadObject(file.c_str());
      }
      
      if (comhandler == 0) {
        trace(SDL_GetError(), log::error);
        throw evil(std::string("It was not possible to load ") + file + ". Make sure it is under one of these folders: " + compath);
        return;
      }
      
      /* get the builder function */
      factory::builder combuilder = (factory::builder)SDL_LoadFunction(comhandler, "build");
      if (combuilder == 0) {
//         std::cerr << "(Component factory) Error loading component " << t << ": " << SDL_GetError() << std::endl;
        SDL_UnloadObject(comhandler);
        return;
      }
      
      /* register the handler and the builder */
      handlers[t] = comhandler;
      builders[f][t] = combuilder;
      
//       std::cerr << "(Component factory) Component type " << t << " loaded" << std::endl;
      
      return;
    }
    
    base * factory::build(component::selector s) {
      component::family f; component::type t;
      f = s.family;
      t = s.type;
      if (t == "") t = f;
      
      factory::builder b;
      
      /* try to locate component based on type */
      buildertable::iterator famit = builders.find(f);
      
      
      if (famit == builders.end()) {
//         std::cerr << "(Component factory) Component family " << f << " not found!" << std::endl;
        return 0;
      }
      
      std::map<component::type, factory::builder>::iterator typit = famit->second.end();
      if (f != t) { typit = famit->second.find(t); }
      else if (famit->second.size() > 0) { typit = famit->second.begin(); }
      
      if (typit == famit->second.end()) {
//         std::cerr << "(Component factory) Component type " << t << " of family " << f << " not found!" << std::endl;
        return 0;
      }
      
      /* build it with the given function */
      b = typit->second;
      base * component = b();
      component->cfactory = this;
//       cout << "(Component factory) Component type " << t << " built" << endl;
      return component;
    }
    
    factory::~factory() {
      while (!handlers.empty()) {
        factory::handler handler = handlers.begin()->second;
//         SDL_UnloadObject(handler); TODO this crashes. why?
        handlers.erase(handlers.begin());
      }
    }
  }
}
