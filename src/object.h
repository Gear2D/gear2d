#ifndef gear2d_object_h
#define gear2d_object_h

#include <map>
#include <list>
using std::map;
using std::list;


#include "definitions.h"
#include "parameter.h"

/**
 * @file object.h
 * @author Leonardo Guilherme de Freitas
 * This defines the common object that holds its components,
 * a table of properties, a table of messages. Your component
 * only need to use it if you are creating game objects inside a component
 * (thats ok, but avoid doing so). */

namespace gear2d {
	namespace component { class base; class factory; typedef std::string type; typedef std::string family; }
	/**
	 * @brief Gear2D game object component-container
	 * This is the owner of components in Gear2D
	 * game engine. Components should be attached
	 * to these.
	 * 
	 * @warning You are allowed to manually create game objects
	 * and manually attach components to it, but thats yet experimental
	 * and may be unsupported on the future. */
	class object {
		public:
			/** @brief Type of objects identifier type */
			typedef std::string type;
			
			/** @brief Object ID that uniquely identifies an object */
			typedef object * id;
			
			/** @brief Blueprint type of this type of objects, so others can be created */
			typedef std::map<std::string, std::string> signature;
			
		public:
				/**
				 * @brief Object factory able to load and configure its components
				 * This is a object factory that, given an id, its able to load
				 * the file with its components and configure them. An object factory
				 * needs an component factory to work */
				class factory {
					public:
						/**
						 * @brief Signature that should be present in every object
						 * @warning Everything in the global signature will be really global,
						 * that means that if one component changes it, all components will
						 * affect all components, so its REALLY global. */
						object::signature commonsig;
					public:
						/**
						 * @brief Creates a new object factory
						 * @p cfactory Component factory to be used when building objects
						 * Object factories depends on a component factory to
						 * build its components. */
						factory(component::factory & cfactory);
						
						/**
						 * @brief Load a set of parameters stored in a file
						 * @p objtype Type of the object
						 * @p file File where the parameters are stored
						 * Loads a file (with the yaml format) that has the
						 * parameters for building the object.
						 * Note that if the file is not specified, a file
						 * named \<objtype\>.yaml is searched. If that file
						 * is not found, nothing is done. */
						void load(object::type objtype, std::string filename = "");
						
						/**
						 * @brief Locate an object that has been loaded by this engine
						 * @p objtype Type of the object
						 * @return ID-handler of the first found object
						 */
						object::id locate(object::type objtype);
						
						/**
						 * @brief Set the parameters for loading the object
						 * @p objtype Type of the object to be registered
						 * @p sig Signature of a object
						 * This is used to define how a factory would initialize
						 * the component to be added. */
						void set(object::type objtype, object::signature sig);
						
						/**
						 * @brief Build a component previously stored in the factory
						 * @p objtype Type of the already-registered object to be built
						 * This uses information from set() and component factory to
						 * create a new object with the registered components */
						object::id build(object::type objtype);
						
					private:
						
						/* recursive build method. catches attaching evil, loading
						 * and attaching needed dependencies */
						void innerbuild(object * o, std::string depends);
					
					private:
						 /* This is the used component factory */
						 component::factory & cfactory;
						 
						 /* This is the mapping between objects and its components and parameters */
						 map<object::type, object::signature> signatures;
						 
						 map<object::type, std::list<object::id> > loadedobjs;
						 
						 friend class object;
				};

		public:
			/**
			 * @brief Object factory that built this object
			 * This is set whenever the object is built using
			 * a factory. */
			object::factory * ofactory;
			map< parameterbase::id, parameterbase::value >::const_iterator it;
		
		private:
			object(object::signature & sig);

		public:
			~object();
			
			/** 
			 * @brief Return the name of this object */
			std::string name();
			
			/**
			 * @brief Return the object id for this object */
			id oid();
			
			/** @brief Attach a component to this object
			 ** @param c Pointer to component to be added
			 ** Attach a component to this object using its
			 ** pointer.
			 **
			 ** @warning If your component happen to depend
			 ** on another, they must be loaded prior to the
			 ** attachment of \ref c, or the attachment will
			 ** fail if they're not found
			 **
			 ** @warning If another component of the same
			 ** type is already attached, it WILL BE REPLACED
			 ** WITH NO MERCY.
			 **
			 ** @warning Components are owned by the object
			 ** and WILL BE DELETED BY THE OBJECT in its
			 ** destruction. Never use heap-allocated components.
			 ** Never. Never Never. Pretty Never with sugar.
			 ** Hopefully you will need not to worry about it... */
			void attach(component::base * c);
			
			/** @brief Deattach a component, returning it
			 ** @param type Type of component to be deattached
			 ** @return The now deattached component
			 ** Deattach a component from the object and return it.
			 ** The user can do whatever he wants with it now.
			 **
			 ** @warning When a component is deattached, the object
			 ** gives the sole responsibility of the component to
			 ** the user of deattach(). That means that YOU ARE
			 ** RESPONSIBLE FOR DELETING IT. Or you can attach it
			 ** elsewhere. */
			component::base * deattach(component::type type);
			
			/**
			 * @brief Access a named parameter
			 * @p pid Parameter id inside the object
			 * @return a @a parameterbase::value reference
			 * Use this to access a parameter inside the component */
			parameterbase::value get(parameterbase::id pid);
			
			/**
			 * @brief Set a named parameter for the given value
			 * @p pid Parameter id inside the object
			 * @p v Parameter value reference
			 * Use this to give the parameter a new generic value.
			 * @warning The value @a v will NOT be cloned. That means
			 * even if another object holds it, it will be holded here to.
			 * If thats not what you mean, just call set() with v->clone().
			 * If thats really what you meant, remember to set v->dodestroy to false
			 * or you may have multiple delete's or missing references and thats an very ugly crash. Really. */
			void set(parameterbase::id pid, parameterbase::value v);
			
			/**
			 * @brief Copy the parameters from other to this
			 * @p other Object to be copied from */
			void copy(object::id other);
			
			/**
			 * @brief Access a component by its family
			 * @p f Family of the component
			 * @return A pointer to the component or NULL if it does not exists
			 */
			gear2d::component::base * component(gear2d::component::family f);
			
			/**
			 * @brief Marks this object to be deleted
			 * Marks this object to be deleted in the begin of
			 * the next frame. */
			void destroy();
			
		private:
			/* container of its components */
			typedef map<component::family, component::base *> componentcontainer;
			componentcontainer components;
			
			/* parameters for this game object */
			parameterbase::table parameters;
			
			/* marks its deletion */
			bool destroyed;
			
			/* own signature */
			object::signature sig;
			
			friend class gear2d::object::factory;
	};
}
#endif