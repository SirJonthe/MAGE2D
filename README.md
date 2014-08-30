SGE2D
=====

Legal notice
------------
Copyright Jonathan Karlsson (SirJonthe@github) 2014

* This library is free to use, share, and modify for anyone and for any purpose
  as long as credit goes where credit is due. In short, have the decency to
  credit the original author (and subsequent authors in case of modification).
* This code is provided "as-is", i.e. the author give no guarantees as to how
  well it will work, if at all, for whatever purpose.
* The author of this code takes no responsibility for any damages resulting
  directly or indirectly from the use of this code.
* This legal notice must be provided if the source code is redistributed.

About
-----
* SGE2D (Simple Game Engine 2D) is a small, simple, OpenGL hardware accelerated
  2D game engine aimed at amateur C++ game developers, enabling them to focus on
  writing game code rather than engine code.
* SGE2D is based around a game object design, where the coder adds a specialized game
  object that overloads virtual functions inherited from a generic game object which
  are then called by the engine at appropriate times.
* Includes a hardware accelerated graphics pipeline for pixel perfect rendering.
* Render pipeline is generic enough for developers to write their own renderers that
  work on custom graphics data.
* Includes routines for sound and music playback. Nothing too fancy.
* Asset managers that allow the user to load an asset once and only once without having
  to worry about the specifics.

Planned features
----------------
* Collisions.
  - Maybe pixel perfect...
* A small physics engine.
  - Maybe pixel perfect...
* Explicit scene graph.
* Simple GUI

Compiling
---------
* Download and link the following libraries to the project:
  - SDL
  - SDL_mixer
  - SDL_image
  - OpenGL
* Also depends on MiniLib
  - Source code included, no need to download separately.

NOTES
-----
* This software is still in very early development. There will be both missing and non-working
  features.
* Because the code is in early development, the author might refactor and/or alter anything in
  the code that will break compatibility with older versions of the code.
* Documentation is missing for now.
* This engine will change name to MAGE2D at a later date to avoid confusion between the already
  established SDL Game Engine 2D (SGE2D).

