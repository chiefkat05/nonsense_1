sound music2 ./snd/forest.mp3
trigger start playsound music2 0.0
trigger start fadeinsound music2 0.1

// crosshair
ui none none 32.0 32.0 0.5 0.5 2

//floor
object cube -4.27796 -7.25001 -1.38751 40 1 40 0 0 col -4.23487 -7.35001 -1.30315 40 1 40 end

object ./models/characters/demoman.obj ./models/characters/ 6.88511 -6.1 -9.95279 0.323603 0.599998 0.268188 0 0 col 6.89199 -6.2 -9.99525 0.462194 1.99999 0.418766 end
ui ./models/dialog/interaction.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0
trigger prompt text 1.0
ui ./models/dialog/chat1.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0
trigger interacted text 1.0