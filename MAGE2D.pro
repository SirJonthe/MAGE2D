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
	MiniLib/MTL/mtlString.cpp \
	MiniLib/MTL/mtlParser.cpp \
	MiniLib/MTL/mtlDirectory.cpp \
	Tools.cpp \
	GUI.cpp \
	Graphics.cpp \
	Transform.cpp \
	Collider.cpp \
	MiniLib/MTL/mtlRandom.cpp \
    Schedule.cpp \
    MiniLib/MTL/mtlMathParser.cpp \
    Thread.cpp

HEADERS += \
	Timer.h \
	Sprite.h \
	Sound.h \
	Object.h \
	Image.h \
	Engine.h \
	Common.h \
	MiniLib/MML/mmlVector.h \
	MiniLib/MML/mmlQuaternion.h \
	MiniLib/MML/mmlMatrix.h \
	MiniLib/MML/mmlMath.h \
	MiniLib/MML/mmlFixed.h \
	MiniLib/MTL/mtlString.h \
	MiniLib/MTL/mtlParser.h \
	MiniLib/MTL/mtlList.h \
	MiniLib/MTL/mtlDuplex.h \
	MiniLib/MTL/mtlDirectory.h \
	MiniLib/MTL/mtlBinaryTree.h \
	MiniLib/MTL/mtlAsset.h \
	MiniLib/MTL/mtlArray.h \
	Tools.h \
	Graphics.h \
	GUI.h \
	MiniLib/MTL/mtlPointer.h \
	MiniLib/MTL/mtlType.h \
	MiniLib/MTL/mtlStringMap.h \
	Platform.h \
	Transform.h \
	Collider.h \
	MiniLib/MTL/mtlHashTable.h \
	MiniLib/MTL/mtlRandom.h \
    Schedule.h \
    MiniLib/MTL/mtlMathParser.h \
    Thread.h

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
