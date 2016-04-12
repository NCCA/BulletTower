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
SOURCES+= src/main.cpp \
					src/NGLScene.cpp \
					src/PhysicsWorld.cpp \
					src/CollisionShape.cpp

HEADERS+= include/NGLScene.h \
					include/PhysicsWorld.h \
					include/CollisionShape.h

INCLUDEPATH +=./include

DESTDIR=./

CONFIG += console
CONFIG -= app_bundle
INCLUDEPATH+=/usr/local/include/bullet
INCLUDEPATH+=/usr/local/include
LIBS+= -L/usr/local/lib -lBulletDynamics  -lBulletCollision  -lLinearMath

NGLPATH=$$(NGLDIR)
isEmpty(NGLPATH){ # note brace must be here
	message("including $HOME/NGL")
	include($(HOME)/NGL/UseNGL.pri)
}
else{ # note brace must be here
	message("Using custom NGL location")
	include($(NGLDIR)/UseNGL.pri)
}
