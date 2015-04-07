# Varon Sector [Ludum Dare 22 entry]
Timeline:  December 2011

A 2D platformer and shooter set on an alien planet.

Overview
--------

The Ludum Dare competitions are challenges for game developers to create a game from scratch in 48 hours.  Competitions are held three times a year, with a new theme for each challenge voted upon by the participants.  This can provide an excellent opportunity for a person to push themselves and create something new.

Ludum Dare 22 was held on December 16-19th with the theme "alone."

For my entry, I created a simple 2-dimensional platforming game titled "Varon Sector."  The player controls an astronaut who has crash landed on an alien planet with no other survivors.  The goal is to search for the communications device that fell from the ship in the hopes of gaining contact with rescuers.


Gameplay
--------

Controls:
Z - Shoot
X - Jump
C - Switch current target
ESC - Pause / exit
SPACE - Skip through cutscenes

In Varon Sector, most of the gameplay is based on typical genre controls for running around, jumping, and shooting at enemies.  One unique thing Varon provides is the targeting system.  In vertical or side-scrolling space shooters, the player will always be firing in one direction--typically either up or to the right.  This leads to players focusing on aiming without having to worry about an auto-targeting system failing.  To emulate this in Varon, players will fire in a constant direction at the currently selected enemy.  The direction is subject to variation:  players will fire in one of eight directions, whichever lines up most closely with the direction towards the current target.

A few level hazards are provided.  Lava pools and rotating spike balls on chains will harm players who are not careful.

Enemies come in a few different varieties, each with their own movement and firing patterns.

Due to the time constraints, a level editor was never built.  Instead, levels were designed by hand on paper.  Coordinates of each vertex were calculated and typed onto the computer.  This did not lend itself well to long-term maintenance, but this is not the point of Ludum Dare.

The game ends when the player finds the communications device.  Unfortunately it has suffered too much damage.  The result of this is the player is stranded on the planet forever:  alone.
