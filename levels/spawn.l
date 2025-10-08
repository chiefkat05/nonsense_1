sound music2 ./snd/forest.mp3
trigger start playsound music2 0.0
trigger start fadeinsound music2 0.1

// crosshair
ui 32.0 32.0 0.5 0.5 1

//floor
object cube -2.1082 -6.30001 -2.60086 40 1 40 0 0 col -4.23487 -7.35001 -1.30315 40 1 40 

// figure out why there are three of these???? inserted into level during setup
object ./models/pedestal.obj ./models/ -0.360488 -2.45 1.88369 1.64134 1 3.51682 4 0 end

object cube -2.46093 0.149997 -0.374034 2.60082 2.2 2.06857 4 0 col -2.47903 1.6 -0.30532 2.60082 2.2 2.06857 end
object ./models/pedestal.obj -2.46093 0.149997 -0.374034 2.60082 2.2 2.06857 4 0 col -2.6429 -1.05 -3.80294 7.74389 2.2 0.422732  end
object cube -7.69999 0.149997 0.723474 2.60082 2.2 2.06857 6 0 end
object ./models/pedestal.obj ./models/ 5.43094 0.349999 5.26085 1 1 1 3 0 end
object cube -9.25166 0.149997 -1.87364 2.60082 2.2 2.06857 4 0 col -11.4142 -1.5 -6.25302 2.06613 2.7 18.0675 end
