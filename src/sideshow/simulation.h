/**
 * @file simulation.h
 * @author Leonardo Guilherme de Freitas
 * @brief Provides physics simulation
 * for components so they can use it to
 * simulate physics to objects they're attached
 * to. This is better as a library because both
 * colliders and rigidbodies (and joints and a lot
 * of other phyisics-related objects) depends on
 * such simulation availability, but some
 * doesn't depend on others (joints doesn't depend
 * on colliders, and such and such), so this is better
 * place outside as a library.
 * 
 * In summary, this is a wrapper to Bullet Dynamics.
 */

#include "gear2d.h"
using namespace gear2d;

class simulation {
	
};