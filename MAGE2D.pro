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
	Tools.cpp \
	GUI.cpp \
	Graphics.cpp \
	Transform.cpp \
	Collider.cpp

HEADERS += \
	Timer.h \
	Sprite.h \
	Sound.h \
	Object.h \
	Image.h \
	Engine.h \
	Common.h \
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
	GUI.h \
	MTL/mtlPointer.h \
	MTL/mtlType.h \
	MTL/mtlStringMap.h \
	Platform.h \
	Transform.h \
	Collider.h

OTHER_FILES += \
	TODO.txt \
	README.md

macx: {
	OBJECTIVE_SOURCES += \
		SDLmain.m

	HEADERS += \
		SDLmain.h
}

win32: {
	# on Windows we need to manually add SDL and GLEW search paths
	LIBS += \
		-lopengl32 \
		-lglew32 \
		-lSDL \
		-lSDLmain \
		-lSDL_image \
		-lSDL_mixer
}

macx: {
	LIBS += \
		-framework Cocoa \
		-framework OpenGL \
		-framework SDL \
		-framework SDL_image \
		-framework SDL_mixer
}

unix:!macx: { # unix-like, e.g. linux, freeBSD
	LIBS += \
		-lGL \
		-lSDL \
		-lSDLmain \
		-lSDL_image \
		-lSDL_mixer
}
