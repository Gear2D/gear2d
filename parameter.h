#ifndef gear2d_parameter_h
#define gear2d_parameter_h

#include "definitions.h"
#include <string>
#include <map>
#include <set>

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
			 * @brief Class to listen to parameter notifications
			 * This class is the base class for receiving
			 * value-changed notifications */
			template <typename datatype>
			class listener {
				public:
					listener() { };
					virtual ~listener() { };
					virtual void handle(id pid, const datatype & oldvalue, const datatype & newvalue) = 0;
			};
		public:
			parameterbase::id pid;
			
		public:
			parameterbase() { }
			
			/** @brief Clone this parameter and its value */
			virtual parameterbase::value clone() = 0;
			
			/**
			 * @brief Set this parameter based on a string value
			 * @p strvalue Value in a string form */
			virtual void set(std::string strvalue) = 0;
			
			/**
			 * @brief Set this parameter based on another
			 * @p other Another parameter */
			virtual void set(const parameterbase & other) = 0;
			
			virtual ~parameterbase() { };
	};
	
	/*
	 * @brief Tired of putting parameterbase all around?
	 * This is your solution. Use pbase instead
	 * Silly... */
	typedef parameterbase pbase;
	
	template<typename datatype>
	class parameter : public parameterbase {
		private:
			datatype raw;
			std::set<listener<datatype> *> hooked;
			
		public:
			parameter() { }
			parameter(datatype raw) : raw(raw) {
			
			}
			
			virtual void set(std::string strvalue) {
				/* attempt a lexical cast... */
				raw = lexcast<datatype>(strvalue);
			}
			
			virtual void set(const parameterbase & _other) {
				parameter<datatype> & other = (parameter<datatype>&)_other;
				raw = (datatype) other.raw;
			}
			
			/*virtual void set(const datatype & rhs) {
				raw = rhs;
			}*/
			
			virtual operator datatype() {
				return raw;
			}
			
			/**
			 * @brief Clone a parameter
			 * This is used to create a new parameter using an
			 * existing one. Useful when parameters are used
			 * as templates */
			virtual parameterbase * clone() {
				parameter<datatype> * cloned = new parameter<datatype>;
				cloned->raw = this->raw;
				cloned->pid = this->pid;
				cloned->hooked = this->hooked;
				return cloned;
			}
			
			/**
			 * @brief Set the internal value to the raw type value
			 * Sets the internal value to a based one and notify
			 * of the parameter being set */
			virtual parameter<datatype> & operator=(const datatype & rhs) {
				typename std::set<listener<datatype> *>::iterator i;
				for (i = hooked.begin(); i != hooked.end(); i++) {
					(*i)->handle(pid, raw, rhs);
				}
				raw = rhs;
				return *this;
			}
			
			/** 
			 * @brief Hook a new listener to this parameter
			 * Add a interested in this component for
			 * value-changed notifications */
			virtual void hook(listener<datatype> & l) {
				hooked.insert(&l);
			}
			
			virtual datatype & get() { return raw; }
			virtual ~parameter() { };
	};
}

#endif