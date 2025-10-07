sound music2 ./snd/forest.mp3
trigger start playsound music2 0.0
trigger start fadeinsound music2 0.1

// crosshair
ui 32.0 32.0 0.5 0.5 1

//floor

object cube 0 -3.1 0 40 1 40 9 0
// comm obj_path mtl_dir x y z sx sy sz image obj_type
// figure out why there are three of these???? inserted into level during setup
object ./models/pedestal.obj ./models/ 1 -2 0 1 1 1 0 0
// some errors with the editor.h




// skybox!
object cube 0 -2.5 0 40 1 40 9 0
object cube -3.51775 0.149997 0.324774 2.60082 2.2 2.06857 4 0
object ./models/pedestal.obj ./models/ -2.71813 -2 1.73653 1 1 1 3 0
object cube -7.69999 0.149997 0.723474 2.60082 2.2 2.06857 6 0
