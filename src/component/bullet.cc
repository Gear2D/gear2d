#include "component.h"
using namespace gear2d;

#include <string>
using namespace std;

#include "btBulletDynamicsCommon.h"

/* TODO collision filtering through collision tags
 * (how? set up a call back?)
 *
 * TODO hook on size so we can update physics size and inertia
 */

class bullet : public gear2d::component::base {
	public:
		bullet() : trigger(false), fx(0.0f), fy(0.0f) { }
		virtual ~bullet() { voters--; }
		
		virtual component::family family() { return "simulation"; }
		virtual component::type type() { return "bullet"; }
		virtual string depends() { return "spatial"; }
		virtual void handle(parameterbase::id pid, base* lastwrite, object::id owner) {
			if (lastwrite == this) return;
			if (pid == "x" || pid == "y" || pid == "z") {
				transform.setOrigin(btVector3(read<float>("x"), read<float>("y"), read<float>("z")));
				motionstate->setWorldTransform(transform);
				body->setWorldTransform(transform);
				return;
			}
			
			/* Set up this as a trigger object,
			 * that is, it doesn't have any contact response, it will only report
			 * collisions */
			if (pid == "simulation.trigger") {

			}
			
			if (pid == "simulation.shape") {
				
			}
			
			if (pid == "simulation.force.x") {
				fx += read<float>("simulation.force.x");
 				cout << "got x " << pid << " " << read<float>("simulation.force.x") << endl;
			}
			if(pid == "simulation.force.y") {
				fy += read<float>("simulation.force.y");
// 				cout << "got y" << endl;
			}
			
			if (pid == "mass") {
				float mass = read<float>("mass");
				
				// body is going to kinematic mode
				if (body->isStaticOrKinematicObject()) {
					// if there is no change in mass, why bother.
					if (mass == 0) return;
					
					// body needs to be recreated.
					world->removeRigidBody(body);
					delete body;
					const float & x = read<float>("x"); const float & y = read<float>("y"); const float & z = read<float>("z");
					const float & w = read<float>("w"); const float & h = read<float>("h"); const float & d = read<float>("d");
					setupbody(x, y, z, w, h, d, mass, trigger);
					world->addRigidBody(body);
					return;
				}
				
				btVector3 inertia;
				body->getCollisionShape()->calculateLocalInertia(mass, inertia);
				body->setMassProps(mass, inertia);
			}
		}
		virtual void setup(object::signature & sig) {
			if (!initialized) initialize();
			/* this means initializing our rigid body and putting it into world */
			const float & x = read<float>("x"); const float & y = read<float>("y"); const float & z = read<float>("z");
			const float & w = read<float>("w"); const float & h = read<float>("h"); const float & d = read<float>("d");
			
			trigger = eval<bool>(sig["simulation.trigger"], false);
			write<bool>("simulation.trigger", trigger);
			write<string>("simulation.shape", "box");
			write<float>("simulation.force.x", 0.0f);
			write<float>("simulation.force.y", 0.0f);
			hook("simulation.force.x");
			hook("simulation.force.y");
			hook("simulation.trigger");
			hook("x"); hook("y"); hook("z"); hook("w"); hook("h"); hook("d");
			
			
			float mass = eval<float>(sig["mass"], 0);
			write("mass", mass);
			hook("mass");
			int flags = 0;
// 			if (trigger == true) flags = btRigidBody::CF_NO_CONTACT_RESPONSE;
			setupbody(x, y, h, w, h, d, mass, flags);
			world->addRigidBody(body);
			voters++;
		}
		
		virtual void update(timediff dt) {
			voting++;
			if (voting >= voters) { globalupdate(dt); voting = 0; }
			body->applyCentralForce(btVector3(fx, fy, 0.0f));
			cout << "fx " << fx << " fy " << fy << endl;
 			fx = 0;
 			fy = 0;
			motionstate->getWorldTransform(transform);
			btVector3 & position = transform.getOrigin();
			
			// update position
			btBoxShape * bs = (btBoxShape *)body->getCollisionShape();
			btVector3 he = bs->getHalfExtentsWithoutMargin();
// 			cout << position.x() << " " << position.y() << " " << position.z() << " " << he.x() << " " << he.y() << " " << he.z() << endl;
			write<float>("x", (float) position.x() - he.x());
			write<float>("y", (float) position.y() - he.y());
			write<float>("z", (float) position.z() - he.z());
// 			write("simulation.force.x", 0.0f);
// 			write("simulation.force.y", 0.0f);
			
		}
		
	private:
		// set up default rigid body properties.
		void setupbody(const float & x, const float & y, const float &z, const float & w, const float & h, const float & d, float mass, int flags) {
			btCollisionShape * shape = new btBoxShape(btVector3(w/2, h/2, d/2));
			btVector3 he = ((btBoxShape*)shape)->getHalfExtentsWithoutMargin();
			btVector3 inertia;
			shape->calculateLocalInertia(mass, inertia);
			
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(x, y, z));
// 			transform.setOrigin(btVector3(x + he.x(), y + he.y(), z + he.z()));
			motionstate = new btDefaultMotionState(transform);		
			
			body = new btRigidBody(mass, motionstate, shape, inertia);
			body->setRestitution(.9f);
			
			// our linkback from rigid body to the sim component
			body->setUserPointer(this);
			
			// set up thins so it its both kinematic and doesn't have any contact response.
			// we will wait until a collider component is added to attach collider trigger
			// and for a dynamics component to make it have any contact response.
// 			body->setCollisionFlags(body->getCollisionFlags() | flags);
		}
		
	private:
		// rigid body properties
		btRigidBody * body;
		
		// this gives us where the object is
		btMotionState * motionstate;
		
		// way of exchanging transform things.
		btTransform transform;
		
		// amount of applied force
		float fx;
		float fy;
		
		bool trigger;
		
	private:
		/* initialize bullet things */
		static void initialize(btVector3 g = btVector3(0, 100, 0)) {
			initialized = true;
			broadphase = new btDbvtBroadphase();
			cconfig = new btDefaultCollisionConfiguration();
			dispatcher = new btCollisionDispatcher(cconfig);
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, cconfig);
			world->setGravity(g);
		}
		
		/* update the world so we can have updated positionings */
		static void globalupdate(float dt) {
			world->stepSimulation(dt);
		}
		
	private:
		// initialized once globally
		static bool initialized;
		
		// number of votes to update
		static int voting;
		static int voters;
		
		// bullet things
		static btBroadphaseInterface * broadphase;
		static btDefaultCollisionConfiguration* cconfig;
		static btCollisionDispatcher* dispatcher;
		static btSequentialImpulseConstraintSolver* solver;
		static btDiscreteDynamicsWorld* world;
};

btBroadphaseInterface * bullet::broadphase;
btDefaultCollisionConfiguration* bullet::cconfig;
btCollisionDispatcher* bullet::dispatcher;
btSequentialImpulseConstraintSolver* bullet::solver;
btDiscreteDynamicsWorld* bullet::world;

bool bullet::initialized = false;
int bullet::voting = 0;
int bullet::voters = 0;

extern "C" {
	component::base * build() { return new bullet(); }
}