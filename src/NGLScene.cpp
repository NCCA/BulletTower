#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>
#include <ngl/Quaternion.h>
#include "PhysicsWorld.h"
#include "CollisionShape.h"

constexpr float BOXW = 10.0f;
constexpr float BOXH = 1.1f;
constexpr float BOXD = 1.1f;
NGLScene::NGLScene()
{

  m_numBlocks = 135;

  setTitle("Bullet Physics and  NGL Demo");
  m_animate = true;
  m_randomPlace = false;
  m_bboxDraw = false;
  m_wireframe = false;
  m_physics = new PhysicsWorld();
  m_physics->setGravity(ngl::Vec3(0.0f, -10.0f, 0.0f));
  m_physics->addGroundPlane(ngl::Vec3(0.0f, 0.0f, 0.0f), ngl::Vec3(50.0f, 0.01f, 50.0f));
  ngl::Random::setSeed();

  CollisionShape *shapes = CollisionShape::instance();

  shapes->addBox("box", BOXW, BOXH, BOXD);
  shapes->addSphere("sphere", 1.0f);
  m_x = 0.0f;
  m_y = 10.0f;
  m_rot = 0.0;
  addCube();
  m_radius = 20;
  m_firePos.set(cos(ngl::radians((m_rot))) * m_radius, m_y, sin(ngl::radians(m_rot)) * m_radius);
}
constexpr float EXT = 5.0f;

void NGLScene::addCube()
{

  ngl::Vec3 pos;
  float angle;
  ngl::Vec3 axis(0, 1, 0);
  float y = 0.0f;

  while (y <= m_numBlocks)
  {
    for (float x = -EXT; x <= EXT; x += BOXD)
    {
      pos.set(x, y, 0);
      angle = 90.0f;
      m_physics->addBox("box", pos, axis, angle);
    }
    y += BOXH;
    for (float z = -EXT; z <= EXT; z += BOXD)
    {
      pos.set(0, y, z);
      angle = 0.0f;
      m_physics->addBox("box", pos, axis, angle);
    }
    y += BOXH;
  }
}

void NGLScene::addSphere()
{

  ngl::Vec3 dir = ngl::Vec3(0, m_y, 0) - m_firePos;
  m_physics->addSphere("sphere", m_firePos, dir * 5, 30, ngl::Random::getRandomVec3() * 20);
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
  delete m_physics;
}

void NGLScene::resizeGL(int _w, int _h)
{
  m_project = ngl::perspective(45.0f, static_cast<float>(_w) / _h, 0.05f, 350.0f);
  m_win.width = static_cast<int>(_w * devicePixelRatio());
  m_win.height = static_cast<int>(_h * devicePixelRatio());
}

void NGLScene::initializeGL()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::initialize();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);

  // now to load the shader and set the values
  // grab an instance of shader manager

  ngl::ShaderLib::use("nglDiffuseShader");
  ngl::ShaderLib::setUniform("Colour", 1.0f, 1.0f, 0.0f, 1.0f);
  ngl::ShaderLib::setUniform("lightPos", 1.0f, 1.0f, 1.0f);
  ngl::ShaderLib::setUniform("lightDiffuse", 1.0f, 1.0f, 1.0f, 1.0f);

  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0.0f, 10.0f, 120.0f);
  ngl::Vec3 to(0.0f, 0.0f, 0.0f);
  ngl::Vec3 up(0.0f, 1.0f, 0.0f);
  // now load to our new camera
  m_view = ngl::lookAt(from, to, up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_project = ngl::perspective(45.0f, 720.0f / 576.0f, 0.05f, 350.0f);

  ngl::VAOPrimitives::createSphere("sphere", 0.5, 40);
  ngl::VAOPrimitives::createLineGrid("plane", 240, 240, 40);

  startTimer(10);
  // as re-size is not explicitly called we need to do this.
  glViewport(0, 0, width(), height());
  m_text = std::make_unique<ngl::Text>("fonts/Arial.ttf", 18);
  m_text->setScreenSize(width(), height());
  m_x = 0.0f;
  m_y = 10.0f;
}

void NGLScene::loadMatricesToShader()
{
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;

  MV = m_view * m_globalTransformMatrix * m_bodyTransform;
  MVP = m_project * MV;
  normalMatrix = MV;
  normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MVP", MVP);
  ngl::ShaderLib::setUniform("normalMatrix", normalMatrix);
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_win.width, m_win.height);
  ngl::ShaderLib::use("nglDiffuseShader");

  // Rotation based on the mouse position for our global transform
  auto rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  auto rotY = ngl::Mat4::rotateY(m_win.spinYFace);
  // create the rotation matrices
  // multiply the rotations
  m_globalTransformMatrix = rotX * rotY;
  // add the translations
  m_globalTransformMatrix.m_m[3][0] = m_modelPos.m_x;
  m_globalTransformMatrix.m_m[3][1] = m_modelPos.m_y;
  m_globalTransformMatrix.m_m[3][2] = m_modelPos.m_z;
  // set this in the TX stack

  // get the VBO instance and draw the built in teapot
  unsigned int bodies = m_physics->getNumCollisionObjects();
  for (unsigned int i = 1; i < bodies; ++i)
  {

    m_bodyTransform = m_physics->getTransformMatrix(i);
    loadMatricesToShader();
    auto scale = ngl::Mat4::scale(BOXW, BOXH, BOXD);

    ngl::ShaderLib::setUniform("Colour", 0.0f, 0.0f, 1.0f, 1.0f);
    switch (m_physics->getCollisionShape(i))
    {
    case BOX_SHAPE_PROXYTYPE:
      ngl::ShaderLib::setUniform("Colour", 1.0f, 0.0f, 0.0f, 1.0f);
      m_bodyTransform *= scale;
      loadMatricesToShader();

      ngl::VAOPrimitives::draw("cube");
      break;
    case SPHERE_SHAPE_PROXYTYPE:
      ngl::ShaderLib::setUniform("Colour", 0.0f, 1.0f, 0.0f, 1.0f);
      ngl::VAOPrimitives::draw("sphere");

      break;
    }
  }

  m_bodyTransform = ngl::Mat4::translate(m_firePos.m_x, m_firePos.m_y, m_firePos.m_z);
  ngl::ShaderLib::setUniform("Colour", 0.0f, 0.0f, 1.0f, 1.0f);
  loadMatricesToShader();
  ngl::VAOPrimitives::draw("cube");

  ngl::ShaderLib::setUniform("Colour", 1.0f, 1.0f, 1.0f, 1.0f);

  m_bodyTransform.identity();
  loadMatricesToShader();

  ngl::VAOPrimitives::draw("plane");
  m_text->setColour(1, 1, 1);
  m_text->renderText(10, 700, fmt::format("Number of Bodies={}", bodies - 1));
  m_text->renderText(10, 680, fmt::format("x={} y={}", m_x, m_y));
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent(QMouseEvent *_event)
{
// note the method buttons() is the button state when event was called
// that is different from button() which is used to check which button was
// pressed when the mousePress/Release event is generated
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
  auto position = _event->position();
#else
  auto position = _event->pos();
#endif
  if (m_win.rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx = position.x() - m_win.origX;
    int diffy = position.y() - m_win.origY;
    m_win.spinXFace += static_cast<int>(0.5f * diffy);
    m_win.spinYFace += static_cast<int>(0.5f * diffx);
    m_win.origX = position.x();
    m_win.origY = position.y();
    update();
  }
  // right mouse translate code
  else if (m_win.translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = static_cast<int>(position.x() - m_win.origXPos);
    int diffY = static_cast<int>(position.y() - m_win.origYPos);
    m_win.origXPos = position.x();
    m_win.origYPos = position.y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent(QMouseEvent *_event)
{
// that method is called when the mouse button is pressed in this case we
// store the value where the maouse was clicked (x,y) and set the Rotate flag to true
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
  auto position = _event->position();
#else
  auto position = _event->pos();
#endif

  if (_event->button() == Qt::LeftButton)
  {
    m_win.origX = position.x();
    m_win.origY = position.y();
    m_win.rotate = true;
  }
  // right mouse translate mode
  else if (_event->button() == Qt::RightButton)
  {
    m_win.origXPos = position.x();
    m_win.origYPos = position.y();
    m_win.translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent(QMouseEvent *_event)
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_win.rotate = false;
  }
  // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_win.translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

  // check the diff of the wheel position (0 means no change)
  if (_event->angleDelta().x() > 0)
  {
    m_modelPos.m_z += ZOOM;
  }
  else if (_event->angleDelta().x() < 0)
  {
    m_modelPos.m_z -= ZOOM;
  }
  update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the NGLScene
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape:
    QGuiApplication::exit(EXIT_SUCCESS);
    break;
  // turn on wirframe rendering
  case Qt::Key_W:
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  // turn off wire frame
  case Qt::Key_S:
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  // show full screen
  case Qt::Key_F:
    showFullScreen();
    break;
  // show windowed
  case Qt::Key_N:
    showNormal();
    break;
  case Qt::Key_Space:
    toggleAnimation();
    break;
  case Qt::Key_X:
    stepAnimation();
    break;
  case Qt::Key_1:
    addCube();
    break;
  case Qt::Key_2:
    addSphere();
    break;
  case Qt::Key_Left:
    m_rot += 5;
    break;
  case Qt::Key_Right:
    m_rot -= 5;
    break;
  case Qt::Key_Down:
    m_y -= 1.0f;
    break;
  case Qt::Key_Up:
    m_y += 1.0f;
    break;
  case Qt::Key_Plus:
  case Qt::Key_Equal:
    m_numBlocks += 5;
    if (m_numBlocks > 300)
      m_numBlocks = 300;
    resetSim();
    addCube();
    break;
  case Qt::Key_Minus:
    m_numBlocks -= 5;
    if (m_numBlocks < 10)
      m_numBlocks = 10;
    resetSim();
    addCube();
    break;

  case Qt::Key_0:
    resetSim();
    addCube();
    break;

  default:
    break;
  }
  // finally update the GLWindow and re-draw
  // if (isExposed())
  m_firePos.set(cos(ngl::radians((m_rot))) * m_radius, m_y, sin(ngl::radians(m_rot)) * m_radius);

  update();
}

void NGLScene::resetSim()
{
  m_physics->reset();
}

void NGLScene::timerEvent(QTimerEvent *_e)
{
  if (m_animate == true)
  {
    m_physics->step(1.0f / 60.0f, 1);
  }
  update();
}
void NGLScene::stepAnimation()
{
  m_physics->step(1.0f / 20.0f, 10);
}
