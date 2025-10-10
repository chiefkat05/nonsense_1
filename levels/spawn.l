sound music2 ./snd/forest.mp3
trigger start playsound music2 0.0
trigger start fadeinsound music2 0.1

ui ./models/dialog/interaction.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0
ui ./models/dialog/takeprompt.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0

// crosshair
ui none none 32.0 32.0 0.5 0.5 2

//floor
object cube -4.27796 -7.25001 -1.38751 40 1 40 0 0 col -4.23487 -7.35001 -1.30315 40 1 40 end

var demomanheld 0.0

object ./models/characters/demoman.obj ./models/characters/ 8.1332 -6.1 -9.59273 0.323603 0.599998 0.268188 0 0 col 8.11809 -6.2 -9.60882 0.52522 1.99999 0.460638 end
trigger prompt text 0 1.0
ui ./models/dialog/chat1.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0
ui ./models/dialog/chat2.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0
trigger interacted text 3 1.0

// item test
object ./models/characters/demoman.obj ./models/characters/ 3.81796 -6.1 -7.04927 0.425686 0.500002 0.331361 10 0 col 3.84151 -6.00001 -7.06794 0.492748 1.2 0.62232 end
trigger prompt text 1 1.0
ui ./models/characters/demoman.obj ./models/characters/ 4.0 4.0 2.0 2.0 10
trigger interacted text last 0.0
trigger interacted getvar demomanheld set 1.0 0.0
trigger is demomanheld equalto 1.0 put -999.0 -999.0 -999.0 0.0