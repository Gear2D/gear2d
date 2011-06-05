#ifndef gear2d_parameter_h
#define gear2d_parameter_h

#include "definitions.h"
#include <string>
#include <map>
#include <set>
#include <list>

#include <iostream>

/**
 * @file parameter.h
 * @author Leonardo Guilherme de Freitas
 * @brief Defines parameter base class and parameter stuff
 * 
 * This is where the base parameter class are defined and and
 * a template specialization of it so you can have virtually
 * any type of parameter.
 */

namespace gear2d {
	namespace component { class base; }
	class object;

	/**
	 * @brief Base parameter class
	 * This is the base parameter container class.
	 * All parameters should inherit this, also all
	 * parameters must be able to convert a string
	 * to its value (because values will be loaded
	 * as strings from data files)
	 */
	class parameterbase {
		public:
			/** @brief ID type for identying parameters */
 			typedef std::string id;

			/** @brief Parameter value type. */
			typedef parameterbase * value;

			/** @brief Parameter mapping for initial component construction */
			typedef std::map<parameterbase::id, parameterbase::value> table;
			
		public:
			/**
			 * @brief Component ID that last performed a write on this parameter
			 * This value should represent the last write access from a component,
			 * so setting this value is component::base::write() responsibility.
			 * 
			 * @warning Handle self-assigned parameters yourself. That is, if you
			 * hook() a parameter and you happen to change it inside update(),
			 * handle() will be called and laswrite will be your component pointer.
			 * 
			 * @warning If you happen to use parameters outsite object API, managing
			 * them is your responsibility
			 */
			component::base * lastwrite;
			
			/** 
			 * @brief Object that own this parameters.
			 * This value represent where this parameter belongs. It is
			 * set whenever you call set() in the object to insert a parameter 
			 * @warning If you happen to use parameters outsite object API, managing
			 * them is your responsibility
			 */
			object * owner;
			
			/**
			 * @brief Parameter ID of this parameter.
			 * This value represents the id of this parameter
			 * 
			 * @warning If you happen to use parameters outsite object API, managing
			 * them is your responsibility */
			parameterbase::id pid;
			
			/**
			 * @brief Inform that this parameter must be destroyed
			 * Inform to the parent object that this parameter must
			 * be destroyed whenever its being deleted
			 * This value is automatically set when a parameter is cloned from
			 * another. */
			bool dodestroy;
			
		public:
			parameterbase() { dodestroy = true; lastwrite = 0; owner = 0; pid = ""; }
			
			/** @brief Clone this parameter and its value */
			virtual parameterbase::value clone() const = 0;
			
			/** @brief Set this parameter based on a generic other */
			virtual void set(const parameterbase * other) throw (evil) = 0;

			/** 
			 * @brief Hook a new listener to this parameter
			 * Add a interested in this component for
			 * value-changed notifications */
			void hook(component::base * c);
			
			virtual bool operator==(const parameterbase & other) const {
				return (owner == owner) && (pid == other.pid);
			}
			
			/**
			 * @brief Pull all the hooked-in components */
			void pull();
			
			
			virtual ~parameterbase() { };
			
		protected:
			std::set<component::base *> hooked;
		
	};
	
	/*
	 * @brief Tired of putting parameterbase all around?
	 * This is your solution. Use pbase instead
	 * Silly... */
	typedef parameterbase pbase;
	
	template<typename datatype>
	class parameter : public parameterbase {
		private:
			datatype * raw;
			bool locked;
			bool mine;
		public:
			/**
			 * @brief Create a parameter pointing to an already existing raw buffer
			 * @p raw Pointer to this pre-allocated raw. It will not be deleted.
			 * 
			 * @warning Don't keed with me and delete raw before destroying this.
			 * I will crash and will laugh at you, because its your fault. You've been
			 * warned, beach. */
			parameter(datatype * raw) : raw(raw), locked(false), mine(false) { }
			
			/**
			 * @brief Creates a new parameter */
			parameter() : raw(new datatype), locked(false), mine(true) { }
			
			/**
			 * @brief Creates a new parameter using a raw value as base
			 * Copies the raw into a new space */
			parameter(datatype raw) : raw(new datatype(raw)), locked(false), mine(false) { }
			
			/**
			 * @brief Sets internal data of parameter
			 * @p raw Raw data to set
			 * @throw evil You cannot write to a parameter while we're updating to the listeners
			 * If you ever happen to listen to a component value changed signals (hook()),
			 * do not try to write it in handle(). If you do, live with the evil you are causing.
			 * @code try { } catch (evil & e) { } @endcode
			 * 
			 * This usually happens when your component try to set() inside handle()
			 */
			virtual void set(const datatype & raw) throw(evil) {
				if (locked == true) {
					throw(evil(pid + ": someone tried to write while locked."));
				}
				*(this->raw) = raw;
				locked = true;
				pull();
				locked = false;
			}
			
			/**
			 * @brief Set this parameter based on a generic other
			 * @p other parameter to be copied from
			 * This will call cast the parameter and set from its raw value. */
			virtual void set(const parameterbase * other) throw (evil) {
				const parameter<datatype> * p = static_cast<const parameter<datatype> *>(other);
				if (pid != p->pid) {
					std::cerr << "Parameter: " << "Something might be wrong: " << pid << " is being set with " << p->pid;
				}
				*raw = *(p->raw);
// 				set(*(p->raw));
			}
			/**
			 * @brief Clone a parameter
			 * This is used to create a new parameter using an
			 * existing one. Useful when parameters are used
			 * as templates */
			virtual parameterbase * clone() const {
				parameter<datatype> * cloned = new parameter<datatype>(*raw); // call constructor with reference to copy.
				cloned->pid = this->pid;
				cloned->hooked = this->hooked;
				cloned->dodestroy = true; // cloned parameters must always be destroyed.
				return cloned;
			}
			
			/**
			 * @brief Return a const reference to the internal data */
			virtual const datatype & operator*() const { return *raw; }
			
			/**
			 * @brief Return a copy of the raw value */
			virtual datatype get() const { return *raw; }
			virtual operator datatype() {
				return get();
			}
			
			virtual ~parameter() { 
				if (mine) delete raw;
			};
	};
}

#endif