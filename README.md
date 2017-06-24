A simple raycasting demo showing a block based map.

Builds on Mac OSX El Capitan with XCode and requires the SDL2 library. Position tests have a seperate makefile and use the Catch unit test framework.

Screenshots
-----------
![s1](/screenshots/points_as_lines.png)<img height="10" hspace="10"/> ![s2](/screenshots/points_as_shapes.png)

![s3](/screenshots/stripes_untextured.png)<img height="10" hspace="10"/> ![s4](/screenshots/stripes_textured.png)

Controls
--------

|Key            | Function    |
|---------------|-------------|
| w             | Move forward |
| a             | Move backward |
| s             | Move left |
| d             | Move right |
| left arrow    | Turn left |
| right arrow   | Turn right |
| +             | Cycle through video modes |
| -             | See + |
| backspace     | Switch between stepped and gridline ray projection |
| enter         | Enable/disable textures (some video modes not effected) |
| [             | Increase field of view |
| ]             | Decrease field of view | 
