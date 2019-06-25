TARGET=BulletTower
OBJECTS_DIR=obj
# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
	cache()
	DEFINES +=QT5BUILD
}


MOC_DIR=moc
CONFIG-=app_bundle
QT+=gui opengl core
SOURCES+= $$PWD/src/main.cpp \
          $$PWD/src/NGLScene.cpp \
          $$PWD/src/PhysicsWorld.cpp \
          $$PWD/src/CollisionShape.cpp

HEADERS+= $$PWD/include/NGLScene.h \
          $$PWD/include/PhysicsWorld.h \
          $$PWD/include/CollisionShape.h \
          $$PWD/include/WindowParams.h


INCLUDEPATH +=./include

DESTDIR=./

CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS+=$$system(pkg-config --cflags bullet)
LIBS+= $$system(pkg-config --libs bullet)
win32 :{
    message(Make sure that Bullet is installed using vcpkg install SDL2 )
  #  message("package dir is" $$VCPK)
  INCLUDEPATH += $$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\include\
  INCLUDEPATH += $$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\include\bullet

  PRE_TARGETDEPS+=$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\Bullet3Common.lib
  LIBS+=-L$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\ -lBullet3Common

  PRE_TARGETDEPS+=$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\BulletCollision.lib
  LIBS+=-L$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\ -lBulletCollision

  PRE_TARGETDEPS+=$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\BulletDynamics.lib
  LIBS+=-L$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\ -lBulletDynamics

  PRE_TARGETDEPS+=$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\LinearMath.lib
  LIBS+=-L$$(HOMEDRIVE)\\$$(HOMEPATH)\vcpkg\installed\x86-windows\lib\ -lLinearMath

}

NGLPATH=$$(NGLDIR)
isEmpty(NGLPATH){ # note brace must be here
        !win32:message("including $HOME/NGL")
        !win32:include($(HOME)/NGL/UseNGL.pri)
        win32:include($(HOMEDRIVE)\$(HOMEPATH)\NGL\UseNGL.pri)
}
else{ # note brace must be here
	message("Using custom NGL location")
	include($(NGLDIR)/UseNGL.pri)
}
