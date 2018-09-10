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
NGLPATH=$$(NGLDIR)
isEmpty(NGLPATH){ # note brace must be here
	message("including $HOME/NGL")
	include($(HOME)/NGL/UseNGL.pri)
}
else{ # note brace must be here
	message("Using custom NGL location")
	include($(NGLDIR)/UseNGL.pri)
}
