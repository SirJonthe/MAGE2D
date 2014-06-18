SGE2D
-----

Legal notice
------------
Copyright Jonathan Karlsson (SirJonthe@github) 2014

* This library is free to use, share, and modify for anyone and for any purpose
  as long as credit goes where credit is due. In short, have the decency to
  credit the original author (and subsequent authors in case of modification).
* This code is provided "as-is", i.e. the author give no guarantees as to how
  well it will work, if at all, for whatever purpose.
* The author of this code takes no responsibility for any damages
  resulting directly or indirectly from the use of this code.

About
-----
* SGE2D (Simple Game Engine 2D) is a small, simple, OpenGL hardware accelerated
  2D game engine that aims to get developers started making a game in C++ as quickly
  as possible by omitting the need to write engine code.
* SGE2D is based around a game object design, where the coder adds a specialized game
  object that overloads virtual functions inherited from a generic game object which
  are then called by the engine at appropriate times.
* Includes a hardware accelerated graphics pipeline for pixel perfect rendering.
* Render pipeline is generic enough for developers to write their own renderers that
  work on custom graphics data.
* Includes routines for sound and music playback.

Planned features
----------------
* Collisions.
  - Various primitive shapes.
  - Pixel perfect.
* A small physics engine.
  - Various primitive shapes.
  - Maybe pixel perfect...

Compiling
---------
* Download and link the following libraries to the project:
  - SDL
  - SDL_mixer
  - OpenGL
* Also depends on MiniLib (included in project for now)

NOTES
-----
* This software is still in very early development. There will be both missing and non-working
  features.
* Documentation is missing.

