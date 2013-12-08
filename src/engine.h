#ifndef gear2d_engine_h
#define gear2d_engine_h

#include "definitions.h"
#include "object.h"

#include <list>
#include <map>

/**
 * @file engine.h
 * @author Leonardo Guilherme de Freitas
 * This class provides the entry point for any
 * game, the game engine. Game engine loads the first
 * objects to be instantiated and the first definitions */

namespace gear2d {
  namespace component { class base; class factory; }
  
  /**
   * @brief Gear2D main engine that binds everything.
   * 
   * This is where your game should start. Everything
   * that happens in Gear2D is controlled by this engine -
   * it commands registering and loading of components and
   * objects */
  class g2dapi engine {
    public:
      /**
       * @brief Returns the version of the engine.
       * @return Library version in a const char * format
       */
      static const char * version();
      
      /**
       * @brief Prepare the engine for running.
       * @param force Force the initialization even if already initialized
       * 
       * This should be called before any attaching
       * of components can be made */
      static void init(bool force = false);
      
      /**
       * @brief Load these configs on the engine.
       * @param configfile The name of the file to load config from
       * 
       * Loads a @p configfile and read scene configurations from it */
      static void load(std::string configfile = "gear2d.yaml");
      
      /**
       * @brief Load configurations from a config mapping.
       * @param config map of configurations
       */
      static void load(map<std::string, std::string> & config);

      /**
       * @brief Select a new scene config to load.
       * @param configfile The name of the file to load config from
       * 
       * At the end of the running frame, the engine will
       * load the file and re-run everything. All loaded
       * objects and components will be destroyed */
      static void next(std::string configfile);
      
      /**
       * @brief Add a new component to the update pipeline.
       * @param c Component pointer to be added
       * 
       * This add a new component to be updated every frame */
      static void add(component::base * c);
      
      /**
       * @brief Remove a component off the update pipeline.
       * @param c Component pointer to be deleted
       * @param rightnow Remove this component right now. This might break if the component is being updated.
       * 
       * This removes the component of the update pipeline. */
      static void remove(component::base * c, bool rightnow = false);
      
      /**
       * @brief Destroy/delete an game object and its components. */
      static void destroy(object::id oid);
       
       /**
        * @brief Run the engine.
        * 
        * Enters in the main loop and only stops when there is
        * no game components to run */
       static int run();
       
       /**
        * @brief Quits the engine.
        * 
        * Whenever possible, the engine will quit itself.
        */
       static int quit();
       
    private:
      engine() { };
      
    private:
      
      /* so we can filter the quit event and quit properly */
      static int eventfilter(void * userdata, void * _ev);
      
    private:
      
      /* config map */
      static std::map<std::string, std::string> * config;
      
      /* running component-type map */
      static std::map<component::family, std::set<component::base *> > * components;
      
      /* component factory */
      static component::factory * cfactory;
       
      /* object factory */
      static object::factory * ofactory;
      
      /* marks if the engine was initialized */
      static bool initialized;
      
      /* marks if the engine was even started */
      static bool started;
      
      /* components to be removed */
      static std::set<component::base *> * removedcom;
      
      /* objects to be destroyed */
      static std::set<object::id> * destroyedobj;
      
      /* scene file to switch */
      static std::string * nextscene;
      
  };

}
#endif
