TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    Timer.cpp \
    Sprite.cpp \
    Sound.cpp \
    Object.cpp \
    Image.cpp \
    Engine.cpp \
    MTL/mtlString.cpp \
    MTL/mtlParser.cpp \
    MTL/mtlDirectory.cpp \
    Transform.cpp \
    Tools.cpp \
    Renderer.cpp \
    Collider.cpp \
    Camera.cpp \
    Polygon.cpp \
    GUI.cpp

HEADERS += \
    Timer.h \
    Sprite.h \
    Sound.h \
    Object.h \
    Image.h \
    Engine.h \
    Common.h \
    Transform.h \
    MML/mmlVector.h \
    MML/mmlQuaternion.h \
    MML/mmlMatrix.h \
    MML/mmlMath.h \
    MML/mmlFixed.h \
    MTL/mtlVersion.h \
    MTL/mtlString.h \
    MTL/mtlParser.h \
    MTL/mtlList.h \
    MTL/mtlDuplex.h \
    MTL/mtlDirectory.h \
    MTL/mtlBinaryTree.h \
    MTL/mtlAsset.h \
    MTL/mtlArray.h \
    Tools.h \
    Graphics.h \
    Renderer.h \
    Collider.h \
    Camera.h \
    Polygon.h \
    GUI.h

OTHER_FILES += \
    TODO.txt \
    sprite_format.txt

LIBS += \
	-lSDL \
	-lSDLmain \
	-lSDL_mixer \
	-lGL \
	-lGLU

DEFINES += \
	GL_GLEXT_PROTOTYPES
