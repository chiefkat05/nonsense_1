//       x pos   y pos   z pos   x scale   y scale   z scale   image id   collision type
//cube   0.0     0.0     0.0     1.0       1.0       1.0       0          0
//pcube is the same but all position and scale numbers will be pixel-perfect. (putting 64 as the y scale will make it 64 pixels tall, for example.)
//"pixels" in this case is referring to the individual pixel size of the texture images, not the literal pixels on your screen.
//each texture is 16 pixels tall and wide, and the sign is 12 pixels wide and 8 pixels tall. so if you make a sign, the best thing you can do is make it a
//pcube and set the scale to '12 8 2' or something similar as you can see below.

var num 0
trigger is num greaterthan 0.0 getvar num set 0.0 0.0
trigger is num lessthan 0.0 getvar num set 0.0 0.0

var gateopen 0

//quad test
quad -4.0 1.5 -0.5 1.0 1.0 1.0 0 1
trigger is num equalto 1.0 move 1.0 0.0 0.0 0.01
trigger is num equalto -1.0 move -1.0 0.0 0.0 0.01

//floor test
cube 0.0 1.5 -4.0 1.0 4.0 0.001 0 0

quad -5.0 1.0 0.0 0.5 0.5 0.5 1 1
trigger seen getvar num set -1 0.0
quad -3.0 1.0 0.0 0.5 0.5 0.5 1 1
trigger seen getvar num set 1 0.0

// pmove test
cube -4.0 -0.5 -4.0 1.0 1.0 1.0 1 0
trigger touch pscale 0 200 0 1.0

// level reload test
cube -15.0 0.0 15.0 1.0 1.0 1.0 4 0
trigger touch setlevel ./levels/spawn.l 0.0

//pyramid test
pyramid 2.0 1.0 0.0 1.0 1.0 1.0 0 1

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
trigger is gateopen equalto 1.0 move 0.0 -0.5 0.0 2.5
trigger is gateopen equalto 1.0 getvar gateopen add 1.0 0.0
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

//pixel_cube test
pcube 128 0 0 16 16 32 2 0
pcube 112 -4 0 16 8 32 2 0
pcube 144 -4 0 16 8 32 2 0

//second area gate button
cube 2.0 -0.5 -17.0 0.5 0.5 0.5 0 0
trigger touch getvar gateopen add 1.0 0.0

//second area
cube 0.0 0.5 -40.0 40.0 1.0 40.0 0 0
cube -19.5 1.0 -40.0 1.0 2.0 40.0 0 0
cube 19.5 1.0 -40.0 1.0 2.0 40.0 0 0
cube 0.0 1.0 -59.5 40.0 2.0 1.0 0 0

//arch
cube -10.0 5.0 -30.0 3.0 10.0 6.0 0 0
cube 10.0 5.0 -30.0 3.0 10.0 6.0 0 0
cube 0.0 10.0 -30.0 20.0 3.0 6.0 0 0

//tower walls
cube 0.0 30.0 -55.0 9.5 60.0 0.5 0 0
cube 0.0 27.0 -45.0 9.5 46.0 1.0 0 0
cube -5.0 30.0 -50.0 0.5 60.0 10.5 0 0
cube 5.0 30.0 -50.0 0.5 60.0 10.5 0 0
cube 0.0 60.0 -45.0 10.0 0.5 20.0 0 0


var elevator 0.0

//elevator hitbox
cube 0.0 1.0 -50.0 10.0 0.5 10.0 -1 1
trigger touch getvar elevator set 1.0 0.0

//elevator
cube 0.0 -2.0 -50.0 10.0 3.0 10.0 1 0
//go up when player detected
trigger is elevator equalto 1.0 move 0.0 10.0 0.0 5.0
trigger is elevator greaterthan 0.0 getvar elevator add 1.0 0.0
//go back down after a moment
trigger is elevator greaterthan 800.0 move 0.0 -10.0 0.0 5.0
trigger is elevator greaterthan 800.0 getvar elevator set 0.0 0.0

//sky 'box'
cube 0.0 49.5 -39.5 10.0 1.0 10.0 0 0
cube -5.0 55.0 -39.75 0.5 10.0 10.0 0 0
cube 5.0 55.0 -39.75 0.5 10.0 10.0 0 0
cube 0.0 50.0 -34.5 10.0 2.0 0.5 0 0

// second signpost
cube 0.0 50.0 -40.0 0.2 1.0 0.2 3 0
cube 0.0 51.0 -40.0 0.75 0.5 0.2 3 0