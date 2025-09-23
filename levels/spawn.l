//       x pos   y pos   z pos   x scale   y scale   z scale   image id   collision type
//cube   0.0     0.0     0.0     1.0       1.0       1.0       0          0
//pcube is the same but all position and scale numbers will be pixel-perfect. (putting 64 as the y scale will make it 64 pixels tall, for example.)
//"pixels" in this case is referring to the individual pixel size of the texture images, not the literal pixels on your screen.
//each texture is 16 pixels tall and wide, and the sign is 12 pixels wide and 8 pixels tall. so if you make a sign, the best thing you can do is make it a
//pcube and set the scale to '12 8 2' or something similar as you can see below.

//quad test
quad -2.5 1.5 0.0 1.0 1.0 1.0 0 1
trigger touch move -1.0 0.0 0.0 0.0

//pyramid test
pyramid 2.0 1.0 0.0 1.0 1.0 1.0 0 1
trigger seen move 0.0 1.0 0.0 0.0

//tri test
tri 4.0 1.0 0.0 1.0 1.0 1.0 0 1

//floor
cube 0.0 -1.0 0.0 40.0 1.0 40.0 2 0
//outside floor
cube 0.0 -1.5 0.0 400.0 0.25 400.0 2 1

//walls
cube -19.5 0.5 0.0 1.0 2.0 40.0 1 0
cube 19.5 0.5 0.0 1.0 2.0 40.0 1 0
cube 0.0 0.5 -19.5 40.0 2.0 1.0 1 0
cube 0.0 0.5 19.5 40.0 2.0 1.0 1 0

//pillar
cube 0.0 1.0 0.0 1.0 3.0 1.0 0 0

//sign
pcube 0 16 8 12 8 2 3 0

//stairway
cube 0.0 2.25 4.0 2.0 0.5 2.0 0 0
cube 0.0 1.75 6.0 2.0 0.5 2.0 0 0
cube 0.0 1.25 8.0 2.0 0.5 2.0 0 0
cube -2.0 0.75 8.0 2.0 0.5 2.0 0 0
cube -4.0 0.25 8.0 2.0 0.5 2.0 0 0
cube -4.0 -0.25 6.0 2.0 0.5 2.0 0 0

// pixel_cube test
pcube 128 0 0 16 16 32 2 0
pcube 112 -4 0 16 8 32 2 0
pcube 144 -4 0 16 8 32 2 0