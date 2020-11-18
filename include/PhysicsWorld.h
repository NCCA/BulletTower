#ifndef PHYSICSWORLD_H_
#define PHYSICSWORLD_H_

//----------------------------------------------------------------------------------------------------------------------
/// @brief this is a minimal Facade wrapper for the code here
//----------------------------------------------------------------------------------------------------------------------

#include <vector>
#include <bullet/btBulletDynamicsCommon.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Obj.h>
#include <memory>

class PhysicsWorld
{
  public :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor, this should really be a singleton as we have quite a few static members and only one world
    //----------------------------------------------------------------------------------------------------------------------
    PhysicsWorld();
    //----------------------------------------------------------------------------------------------------------------------
    // @brief dtor
    //----------------------------------------------------------------------------------------------------------------------
    ~PhysicsWorld();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief reset the simulation (remove all geo etc)
    //----------------------------------------------------------------------------------------------------------------------
    void reset();

    void setGravity(float _x, float _y, float _z)
    {
      m_dynamicsWorld->setGravity(btVector3(_x,_y,_z));
    }
    void setGravity(ngl::Vec3 _g)
    {
      m_dynamicsWorld->setGravity(btVector3(_g.m_x,_g.m_y,_g.m_z));
    }
    std::string getBodyNameAtIndex(unsigned int i) const{return m_bodies[i].name;}

    void addGroundPlane(const ngl::Vec3 &_pos,const ngl::Vec3 &_g);

    void addSphere(std::string _shapeName, const ngl::Vec3 &_pos, const ngl::Vec3 &_dir, ngl::Real _mass, const ngl::Vec3 &_inertia);
    void addBox(std::string _shapeName,const ngl::Vec3 &_pos,const ngl::Vec3 &_axis, ngl::Real _angle);
    void step(float _time, float _step);

    int getNumCollisionObjects()const
    {
      return m_dynamicsWorld->getNumCollisionObjects();
    }

    ngl::Vec3 getPosition(unsigned int _index);
    ngl::Mat4 getTransformMatrix(unsigned int _index);
    int getCollisionShape(unsigned int _index) const;
  private :
    // if we need to access the bodies we need to store our own version of the pointer
    // as there is no real way of accessing it from bullet once it is added,
    // with luck the index will be the same as the one in the
    // bullet sim
    typedef struct
    {
      std::string name;
      btRigidBody* body;
    }Body;
    btBroadphaseInterface*	m_broadphase;


    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_overlappingPairCache ;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;
    std::unique_ptr<btCollisionShape> m_groundShape;
    std::vector <Body> m_bodies;
};

#endif

