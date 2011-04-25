#ifndef gear2d_component_h
#define gear2d_component_h

#include <string>
#include <map>
#include <list>

#include "definitions.h"
#include "parameter.h"
#include "object.h"

/** 
 * @file component.h
 * @author Leonardo Guilherme de Freitas
 * @brief This file contains the base component interface
 * 
 * Every component MUST implement this base class if its meant
 * to be a component.
 * 
 * TODO: a component creation how-to
 */

namespace gear2d {
	class object;
	class engine;

	/** @brief Component namespace
	 ** Where all components shall live
	 ** Every component should be declared within
	 ** this namespace */
	namespace component {
		/** @brief Component type identifier */
		typedef std::string type;
		
		/** 
		 * @brief Base class for components
		 * All classes meant to be component shall
		 * derive on this class as it provide the
		 * common interface for all of them
		 *
		 * @warning Only one type of a component could
		 * be added. If you need to group multiple components
		 * of the same type, create a custom component as a
		 * container of them.
		 * 
		 * @warning A component only receives update(), message
		 * or anything when attached to a game object. */
		class base {
			friend class gear2d::object;
			friend class gear2d::engine;
			public:
				/**
				 * @brief Build base part of components
				 * This constructor do some setup for components,
				 * mostly registering with the engine */
				base();
				virtual ~base();
				
				/** @brief Returns the type of this component */
				virtual component::type type() = 0;
				
				/** @brief Retiurn a list of components that conflict with this */
				/* TODO: add conflict management */
				virtual std::list<component::type> conflict() { return std::list<component::type>(); }
				
				/**
				 * @brief Where you should put initialization routines
				 * This is set to give your component an opportunity
				 * to initialize (since its not done in the construction).
				 * This is granted to be called when everythings ready for
				 * parameter accessing and listenind, so this is pleace
				 * to hook your listeners */
				virtual void init() = 0;
				
				/** 
				 * @brief Return a table of parameter templates
				 * This will return a table of parameters provided or
				 * required for this component, along with its default
				 * values that is clone-suitable */
				virtual const parameterbase::table & parameters() = 0;
				
				/**
				 * @brief Updates the state of this component
				 * @param dt Delta time relative to last frame */
				virtual void update(timediff dt) = 0;
				
				/**
				 * @brief Access a shared parameter
				 * @p pid Name of the parameter
				 * Access a parameter in its container
				 */
				template <typename datatype>
				parameter<datatype> & access(parameterbase::id pid) {
					parameterbase::value & v = owner->access(pid);
					if (v == 0) v = new parameter<datatype>;
					return *((parameter<datatype>*)v);
				}
				
				/**
				 * @brief Access a shared parameter in another object
				 * @p oid Pointer to the other object id
				 * @p pid Name of the parameter
				 * Access a shared parameter at the object specified
				 * by its oid.
				 */
				 template <typename datatype>
				parameter<datatype> & access(object::id oid, parameterbase::id pid) {
					parameterbase::value & v = oid->access(pid);
					if (v == 0) v = new parameter<datatype>;
					return *((parameter<datatype>*)v);
				}
				
				/** @brief Setup this component with a parameter map */
// 				virtual void setup(parameterbase::table & parameters) = 0;
				
				/** @brief Set a parameter in this component */
// 				virtual void set(parameterbase::id p, parameterbase::value v) = 0;


			protected:
				object::id owner;
		};
		
		class factory {
			public:
				/**
				 * @brief Path for system-wide components.
				 * If a component fails to be found at component/, it will
				 * be searched in compath */
				std::string compath;
				

			public:
				/** @brief The build function or functor that create that component */
				typedef base* (*builder)();
				
				/** @brief Adds a component builder to the factory
				 ** Register a component in the factory of components so it
				 ** can be accessed by its name in build() */
				void set(component::type t, factory::builder b);
				
				/**
				 * @brief Build a component with the type t
				 * @p t Type of the component registered with @a set()
				 * @return A pointer to the component
				 * Use this whenever you want to build a component. If the
				 * component was not found, a null pointer will be returned */
				base * build(component::type t);
				
				/**
				 * @brief Load a component from a shared plugin
				 * @p t Component type to look for
				 * @p file A optional argument specifying the plugin file */
				void load(component::type t, std::string filepath = "");
				
			private:
				typedef std::map<component::type, factory::builder> buildertable;
				buildertable builders;
				
				typedef void * handler;
				typedef std::map<component::type, handler> handlertable;
				handlertable handlers;
		};
	}
}

#endif