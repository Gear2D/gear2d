#ifndef gear2d_component_h
#define gear2d_component_h

#include <string>
#include <map>
#include <list>
#include <iostream>
using std::cout;
using std::endl;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;
#include <iostream>
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
		
		/** @brief Component identifyier type */
		typedef component::base * id;
		
		/** @brief Component family identifier */
		typedef std::string family;
		
		/**
		 * @brief Helper class for handling type/id pairs
		 * Use this class whenever you want to be very specific
		 * about component description. This helps converting
		 * from string to family/type selector and vice-versa. */
		struct selector {
			/** @brief Family of this selector */
			component::family family;
			
			/** @brief Type of this selector */
			component::type type;
			
			/** @brief Builds an empty selector */
			selector();
			
			/** 
			 * @brief Builds a selector from a family and type
			 * @p family Family of this component
			 * @p type Type of this component
			 * @warning type will defaults to family if not specified */
			selector(component::family family, component::type type);
			
			/**
			 * @brief Builds a selector from a family/type string
			 * @p selstring Selector string as in family/type */
			selector(std::string selstring);
			
			/**
			 * @brief Convert a string as in family/type to this selector
			 * @p rhs String to be converted
			 * @return this
			 */
			selector & operator=(std::string rhs);
			
			/**
			 * @brief Copy a selector
			 * @p rhs Other selector
			 * @return this
			 */
			selector & operator=(const selector & rhs);
			
			/**
			 * @brief Compare family and type to other selector
			 * @p rhs Other selector
			 * @return true if family and type are equals, false if at least one is not equal */
			bool operator==(const selector & rhs);
			
			/**
			 * @brief Compare family and type to a selector string
			 * @p rhs Selector string
			 * @return true if the decomposed string matches family/type. false if not. */
			bool operator==(std::string rhs);
			
			/** 
			 * @brief Convert this selector to a family/type string
			 * @return family/type string */
			operator std::string();
			
			~selector() { }
		};
		
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
			friend class gear2d::component::factory;
			protected:
				base();
				
			public:
				virtual ~base();
				
				/** @brief Returns the type of this component */
				virtual component::type type() = 0;
				
				/** @brief Returns the family of this component.
				 * Defaults to type() */
				virtual component::family family() { return type(); }
				
				/**
				 * @brief Components that must exists before its attachment
				 * The object will call this function in the attachment process,
				 * and check dependencies one by one. If any of the specified
				 * components is not found attached, attaching will simply fail */
				virtual std::string depends() { return ""; }
				
				/**
				 * @brief Method that will be called to handle value changes
				 * @p id Parameter ID of the changed parameter
				 * @p lastwrite The last component to write in this parameter (likely the one who triggered it)
				 * You should implement this in your component if you ever
				 * happen to listen to a parameter. */
				virtual void handle(parameterbase::id pid, component::base * lastwrite) { ; }
				
				/**
				 * @brief Where you should put initialization routines
				 * @p sig Object original signature
				 * This is set to give your component an opportunity
				 * to initialize (since its not done in the construction).
				 * Using sig you can parse all the initial parameters
				 * passed to the object 
				 *
				 * This is garanteed to be called after the object was attached, so
				 * read, eval, and write operations will be garanteed to work.
				 */
				virtual void setup(object::signature & sig) = 0;
				
				/**
				 * @brief Updates the state of this component
				 * @param dt Delta time relative to last frame */
				virtual void update(timediff dt) = 0;
				
				/**
				 * @brief Reads a shared parameter and store on target
				 * @p pid Parameter id to be read
				 * @p target Target where the parameter value will be put
				 * @warning The parameter will be created if it does not exists
				 * 
				 * Reads a parameter and store it on target. */
				template<typename datatype>
				void read(parameterbase:: id pid, datatype & target) {
					parameter<datatype> * v = access<datatype>(pid);
					target = v->get();
				}
				
				/**
				 * @brief Reads a shared parameter and return its value
				 * @p pid Parameter id to be read
				 * @p datatype Type of the parameter
				 * @warning The parameter will be created if it does not exists */
				template<typename datatype>
				datatype read(parameterbase::id pid) {
					parameter<datatype> * v = access<datatype>(pid);
					return v->get();
				}
				
				/**
				 * @brief Writes in a shared parameter
				 * @p pid Parameter id to be write to
				 * @p source Source from it shall be copied
				 * @warning Be careful about type promotion. Be explicit if necessary.
				 * @warning The parameter will be created if it does not exists */
				template<typename datatype>
				void write(parameterbase::id pid, const datatype & source) {
					parameter<datatype> * v = access<datatype>(pid);
// 					cout << "debug: " << this->family()+"/"+this->type() << " writing to " << pid << endl;
					v->set(source);
					v->lastwrite = this;
				}
				
				/**
				 * @brief Perform read, add, write
				 * @p pid Parameter to be added
				 * @p sum Value to be added
				 * @warning Be careful about type promotion. Be explicit if necessary.
				 */
				template<typename datatype>
				void add(parameterbase::id pid, const datatype & sum) {
					datatype t;
					read(pid, t);
					write(pid, t + sum);
				}
				
				/**
				 * @brief Initialize a parameter from string
				 * @p pid Parameter id
				 * @p raw Raw string value
				 * @p def Default if raw is empty
				 * 
				 * Evaluates the raw string to datatype and then set the parameter
				 * to it, using eval(). eval may be specialized to mach your specific
				 * needs. Look at \ref eval for more info on how to do that.
				 * 
				 * @warning This suffer from the same issue as @ref component::base::set.
				 * it will be initialized only once. Latter is evil.
				 */
				template<typename datatype>
				void init(parameterbase::id pid, std::string raw, datatype def) {
					if (owner->get(pid) != 0) throw evil(this->type() + "tried init(" + pid + ")" + " but its already set.");
					if (raw == "") write(pid, def);
					else write(pid, eval<datatype>(raw, def));
					parameterbase::value v = owner->get(pid);
					v->lastwrite = this;
				}
				
				/**
				 * @brief Uses another parameter to set the pid
				 * @p pid Parameter id
				 * @p param Parameter to be associated with pid
				 * 
				 * Sets pid to point to an existing raw parameter. Likely used to provide
				 * data shared among many instances of the same component and meant to
				 * be changed by others.
				 *
				 * @warning This is dangerous as hell because we allow referencing data
				 * thats inside the parameter (even if its const) everywhere. Hell, this method
				 * is even encouraging this kind of usage.  Using this twice will
				 * make all the old references break in the ugliest form possible. If you happen
				 * to free the old parameter, its better to run for your nuclear shelter because
				 * this is gonna blow. Thats why it WILL fail (throw evil at you) if the object
				 * already have a parameter with the same name.
				 * 
				 * @warning Note that \ref read(), \ref write(), \ref add and \ref raw creates the parameter
				 * if it do not exists, so be assured to use bind() before.
				 */
				template <typename datatype>
				void bind(parameterbase::id pid, datatype & raw) throw (evil) {
					if (owner->get(pid) != 0) throw evil(this->type() + " tried bind(" + pid + ")" + " but its already set.");
					parameter<datatype> * p = new parameter<datatype>(&raw);
					p->dodestroy = false;
					owner->set(pid, p);
				}
				
				/**
				 * @brief Evaluate a string to a type
				 * @p raw Raw string to be evaluated
				 * @p def Default if raw is empty
				 * 
				 * This tries to do a lexical cast to the raw string to the
				 * destination (datatype) type, defaulting to boost::lexical_cast.
				 * 
				 * You can (and should!) do a template specialization of this method
				 * if you know better than lexical_cast:
				 * @code
				 * namespace gear2d {  namespace component {
				 * template<> eval<your-fancy-type>(std::string raw, your-fancy-type def) {
				 *     // do your stuff to convert raw to your-fancy-type
				 *     // and then return it
				 * }
				 * } }
				 * @endcode
				 * 
				 * The default works very well for basic types.
				 * @warning The lexical cast may fail if converting
				 * from string to some of the basic types like float
				 * or int if raw is empty. Check your arguments.
				 */
				template<typename datatype>
				datatype eval(std::string raw, datatype def) {
					datatype t;
					 try {
						 t = lexical_cast<datatype>(raw);
					 } catch (boost::bad_lexical_cast & blc) {
						t = def;
					 }
					 return t;
				}
				
				/**
				 * @brief Access a reference to the raw parameter
				 * Access a const reference to the raw parameter so
				 * you do not need to perform read() too many times
				 * (which perform a lot of function calls, including
				 * comparing strings).
				 * 
				 * @warning Please note that this will create the parameter
				 * if it does not exists.
				 */
				template<typename datatype>
				const datatype & raw(parameterbase::id pid) {
					parameter<datatype> * v = access<datatype>(pid);
					return *(*v);
				}

				/**
				 * @brief Hook this component to a parameter in parent
				 * @p pid Parameter id
				 * Add this component as a listener to the parameter, to be
				 * called whenever it changes its value. Be aware that if the
				 * parameter does not exists, it will not be created and it
				 * will fail silently. Use if (exists(pid)) hook(pid) if you
				 * want to know if/when it fails. If you know where that
				 * parameter is coming from, its better to add it to the dependency
				 * list, no?
				 * 
				 * @warning Please note that your component should implement
				 * a listener for the datatype you are hooking to or else
				 * it won't compile. */
				void hook(parameterbase::id pid) {
					parameterbase::value v = owner->get(pid);
					if (v == 0) return;
					cout << "debug: " << this->family()+"/"+this->type() << " (" << this << ") is hooking to " << pid << endl;
					v->hook(this);
				}
				
				/**
				 * @brief Query if the given parameter exists
				 * @p pid Parameter id
				 * @return parameter address if it does exists, (null) 0 if not.
				 * This is queries if the parameter exists. */
				parameterbase * exists(parameterbase::id pid);

				/**
				 * @brief Build another component based on selector
				 * @p s Component selector (family/type)
				 * Use the component factory to build another component.
				 * 
				 * @warning Please notice that this will only work
				 * if the component was built with a factory and will
				 * only work at setup() time, not at constructing time. */
				component::base * build(component::selector s);
				
				/**
				 * @brief Spawns another object
				 * @p t Type of the object to load
				 * This uses object factory to spawn another object
				 * loaded.
				 */
				object::id spawn(object::type t);
				
				/**
				 * @brief Clone the parent game object
				 * @return A pointer to a compononent of the same type that called clone()
				 * This will creates a new game object and
				 * then set its parameter table with this one. */
				component::base * clone();
				
				/**
				 * @brief Marks the owner for destruction
				 * Marks the owner object for destruction at the next frame.
				 * Please notice that this component will be put in the
				 * destruction list too. */
				void destroy();
				
			private:
				template<typename datatype>
				parameter<datatype> * access(parameterbase::id pid) {
					parameterbase::value v = owner->get(pid);
					if (v == 0) {
						v = new parameter<datatype>;
						owner->set(pid, v);
					}
					return (parameter<datatype>*)v;
				}
				
			protected:
				object::id owner;
				
			private:
				// component factory that build we
				component::factory * cfactory;
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
				void set(component::selector s, factory::builder b);
				
				/**
				 * @brief Build a component with the type t and family f
				 * @p f Family of the component registered with \ref set
				 * @p t Type of the component registered with \ref set
				 * @return A pointer to the component
				 * Use this whenever you want to build a component. If the
				 * component was not found, a null pointer will be returned */
				base * build(component::selector s);
				
				/**
				 * @brief Load a componend from a shared plugin
				 * @p s A selector in the form family/type
				 * @p filepath An optional argument specifying the plugin file */
				void load(component::selector s, std::string filepath = "");
				
			private:
				typedef std::map<component::family, std::map<component::type, factory::builder> > buildertable;
				buildertable builders;
				
				typedef void * handler;
				typedef std::map<component::type, handler> handlertable;
				handlertable handlers;
		};
	}
}

#endif