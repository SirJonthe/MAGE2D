MAGE2D
======

Legal notice
------------
Copyright Jonathan Karlsson (SirJonthe@github) 2014-2015

* This library is free to use, share, and modify for anyone and for any purpose as long as credit goes where credit is due. In short, have the decency to credit the original author (and subsequent authors in case of modification).
* This code is provided "as-is", i.e. the author give no guarantees as to how well it will work, if at all, for whatever purpose.
* The author of this code takes no responsibility for any damages resulting directly or indirectly from the use of this code.
* This legal notice must be provided if the source code is redistributed.

About
-----
* MAGE2D (Minimal Amateur Game Engine 2D) is a small, simple, OpenGL hardware accelerated 2D game engine aimed at amateur C++ game developers, enabling them to focus on writing game code rather than engine code.
* MAGE2D is based around a game object design, where the coder adds a specialized game object that overloads virtual functions inherited from a generic game object which are then called by the engine at appropriate times.
* Includes a hardware accelerated graphics pipeline for pixel perfect rendering.
* Render pipeline is generic enough for developers to write their own renderers that work on custom graphics data.
* Includes simple routines for sound and music playback. Nothing too fancy yet.
* Asset managers that allow the user to load an asset once and only once without having to worry about the specifics.

Planned features
----------------
* Simple GUI
* A small physics engine.
  - Polygon colliders.
* Explicit scene graph.
* A few dev tools (sprite editor, console, tile editor, profiler/debugger, etc.)
* Maybe pixel perfect colliders for collision detection and physics.
* Once I feel satisfied with the current state of the engine I will get to writing
  documentation.

Compiling
---------
* Download and link the following libraries to the project:
  - SDL
  - SDL_mixer
  - SDL_image
  - OpenGL
* Also depends on MiniLib
  - Source code included as a Git submodule.
  - Just doing a git clone of this directory will not download submodule files. Easiest way to get these is to do a "git clone --recursive [MAGE2D repo]".
* When using G++, whenever you alter a class do a clean rebuild. I often get heap corruptions in the binary if I do not. I'm not sure why this is...

NOTES
-----
* This software is still in very early development. There will be both missing and non-working features.
* Because the code is in early development, the author might refactor and/or alter anything in the code that will break compatibility with older versions of the code.
* Documentation is missing for now.
