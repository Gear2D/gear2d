#ifndef gear2d_parameter_h
#define gear2d_parameter_h

#include "definitions.h"
#include "log.h"
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
  namespace component { 
    class base;
  }
  class object;

  /**
   * @brief Base parameter class.
   * 
   * All parameters should inherit this, also all
   * parameters must be able to convert a string
   * to its value (because values will be loaded
   * as strings from data files).
   */
  class g2dapi parameterbase {
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
       * 
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
       * 
       * This pointer is a cross-reference to its owner. It is
       * set whenever you call set() in the object to insert a parameter 
       * @warning If you happen to use parameters outsite object API, managing
       * them is your responsibility
       */
      object * owner;
      
      /**
       * @brief Parameter ID of this parameter.
       * @warning If you happen to use parameters outsite object API, managing
       * them is your responsibility */
      parameterbase::id pid;
      
      /**
       * @brief Inform that this parameter must be destroyed
       * 
       * Inform to the parent object that this parameter must
       * be destroyed whenever its being deleted
       * This value is automatically set when a parameter is cloned from
       * another. */
      bool dodestroy;
      
    public:
      /** @brief Initializes an empty parameterbase */
      parameterbase() { dodestroy = true; lastwrite = 0; owner = 0; pid = ""; }
      
      /** @brief Clone this parameter and its value */
      virtual parameterbase::value clone() const = 0;
      
      /** @brief Set this parameter based on a generic other */
      virtual void set(const parameterbase * other) throw (evil) = 0;

      /** 
       * @brief Hook a new listener to this parameter
       * @param c Component to be notified of value-changes
       * 
       * Add an interested component in this component for
       * value-changed notifications. When there's a pending
       * notification, handle() will be called.
       */
      void hook(component::base * c);
      
      /**
       * @brief Hook a new listener to this parameter using a custom callback.
       * @param c Component to be notified of value-changes
       * @param h Callback function to be called in the component whenever
       * there is a value change.
       * 
       * @see gear2d::call
       */
      void hook(component::base * c, component::call h);
      
      /**
       * @brief Unhook a listener in this parameter
       * @param c Component to be unhooked */
      void unhook(component::base * c);
      
      /**
       * @brief Compare two parameters.
       * @param other Other parameter to compare.
       * 
       * This method basically compares if the owners are the same
       * and if the parameter ids are the same.
       */
      virtual bool operator==(const parameterbase & other) const {
        return (owner == owner) && (pid == other.pid);
      }
      
      /**
       * @brief Pull all the hooked-in components */
      void pull();
      
      
      virtual ~parameterbase() { };
      
    protected:
      class callback;
      std::set<callback *> hooked;
    
  };
  
  /*
   * @brief Tired of putting parameterbase all around?
   * This is your solution. Use pbase instead
   * Silly... */
  typedef parameterbase pbase;
  template<typename basetype> class parameter;

  /**
   * @brief Exception to signal a broken link.
   */
  class g2dapi badlink : public evil {
    public:
      badlink();
  };
  
  /**
   * @brief Link to a parameter.
   * @warning Prefer creating links to read() and write(),
   * because links store the parameter and will never lose them.
   * 
   * It works as a wrapper so you need not to do
   * read() and write() */
  template<typename basetype>
  class link {
    private:
      parameter<basetype> * target;
      component::base * writer;
      
    public:
      
      /**
       * @brief Build a link based on another link
       * @param other Other link
       */
      link(const link<basetype> & other)
      : target(other.target)
      , writer(other.writer)
      { }
      
      
      /**
       * @brief Default link constructor.
       */
      link(parameter<basetype> * target = 0, component::base * writer = 0)
      : target(target)
      , writer(writer)
      { }
      
      /**
       * @brief Writes into this parameter link.
       * @throw badlink When you try to write from a unlinked link
       */
      link<basetype> & operator=(const basetype & source) throw (gear2d::badlink) {
        if (target == 0) throw(gear2d::badlink());
        target->lastwrite = writer;
        target->set(source);
        return *this;
      }
      
      link<basetype> & operator=(const link<basetype> & other) {
        target = other.target;
        writer = other.writer;
        return *this;
      }
      
      /**
       * @brief Reads from this parameter link.
       * @throw badlink When you try to read from a unlinked link
       */
      operator basetype() const throw (gear2d::badlink) {
        if (target == 0) throw(gear2d::badlink());
        return target->get();
      }
      
      link<basetype> & operator +=(const basetype & source) {
        target->lastwrite = writer;
        target->set(target->get() + source);
        return *this;
      }
      
      basetype operator*() {
        if (target == 0) throw(gear2d::badlink());
        return ((basetype) target);
      }
  };
  
  /**
   * @class parameter
   * @brief Generic parameter class implementing parameterbase.
   * 
   * This class implements the parameterbase interface and it is
   * specialized with the type of the parameter to hold. It is a 
   * sort-of shared pointer (it hols a pointer to the raw data),
   * and provides listeners to value-change notifications.
   * 
   * It is the mother of the blackboard system.
   */
  template<typename datatype>
  class parameter : public parameterbase {
    private:
      datatype * raw;
      bool locked;
      bool mine;
      
    public:
      /**
       * @brief Create a parameter pointing to an already existing raw buffer.
       * @param raw Pointer to this pre-allocated raw. It will not be deleted.
       * 
       * @warning Don't keed with me and delete raw before destroying this.
       * I will crash and will laugh at you, because its your fault. You've been
       * warned. */
      parameter(datatype * raw) : raw(raw), locked(false), mine(false) { }
      
      /**
       * @brief Creates a new parameter */
      parameter() : raw(new datatype), locked(false), mine(true) { }
      
      /**
       * @brief Creates a new parameter using a raw value as base.
       * 
       * Copies the raw data into a new space */
      parameter(datatype raw) : raw(new datatype(raw)), locked(false), mine(false) { }
      
      /**
       * @brief Sets internal data of parameter.
       * @param raw Raw data to set
       * @throw evil You cannot write to a parameter while we're updating to the listeners
       * 
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
       * @brief Set this parameter based on a generic other.
       * @param other parameter to be copied from
       * 
       * This will call cast the parameter and set from its raw value.
       *
       * @warning There's no type checking or whatsoever.
       */
      virtual void set(const parameterbase * other) throw (evil) {
        modinfo("engine");
        const parameter<datatype> * p = static_cast<const parameter<datatype> *>(other);
        if (pid != p->pid) {
          trace("Something is definetly wrong.", pid, " is being set with ", p->pid, log::error);
        }
        *raw = *(p->raw);
//         set(*(p->raw));
      }
      /**
       * @brief Clone a parameter.
       * 
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
      
      virtual parameter<datatype> & operator=(const datatype & raw) {
        set(raw);
        return *this;
      }
      
      virtual bool operator==(const parameter<datatype> & other) {
        return (*raw == *(other.raw));
      }
      
      virtual ~parameter() { 
        if (mine) delete raw;
      };
  };
}

#endif
