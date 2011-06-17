#ifndef gear2d_h
#define gear2d_h

/**
 * @file gear2d.h
 * @author Leonardo Guilherme de Freitas
 * @brief Gear2D main components
 * This file contains the needed includes for developing
 * with Gear2D
 * 
 * Gear2D is a flexible component-based data-centric game
 * engine, using a data repository as means of component
 * intercomunication.
 * 
 * Game objects are nothing but the holder of the data
 * repository and the container of the components.
 * 
 * Game object templates can be defined at the data level
 * using yaml files, which in turn is nothing more than a
 * key: value list for each component it must have.
 * 
 * Components can be loaded and attached in runtime without
 * the need of recompiling the engine.
 */


#include "parameter.h"
#include "object.h"
#include "component.h"
#include "definitions.h"
#include "engine.h"

/**
 * @brief Gear2D Engine main namespace.
 * Everything created or managed by gear2d shall be
 * withing this namespace
 */
namespace gear2d {
}
#endif