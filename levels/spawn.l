sound music2 ./snd/forest.mp3
trigger none start playsound music2 0.0
trigger none start fadeinsound music2 0.1

ui ./models/dialog/interaction.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0
ui ./models/dialog/takeprompt.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0

// crosshair
ui none none 32.0 32.0 0.5 0.5 2

//floor
object cube -4.27796 -7.25001 -1.38751 40 1 40 0 0 col -4.23487 -7.35001 -1.30315 40 1 40 end

var demomanheld 0.0
trigger none is demomanheld equalto 0.0 notrigger
trigger none is demomanheld equalto 1.0 notrigger

// where is the demoman ui icon thing??????
object ./models/characters/demoman.obj ./models/characters/ 8.1332 -6.1 -9.59273 0.323603 0.599998 0.268188 0 0 col 8.11809 -6.2 -9.60882 0.52522 1.99999 0.460638 end
trigger none prompt text 0 1.0
ui ./models/dialog/chat1.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0
ui ./models/dialog/chat2.obj ./models/dialog/ 32.0 16.0 -4.0 4.0 0
trigger 2 interacted text 3 1.0
trigger 3 interacted text 4 1.0

// item test
object ./models/characters/demoman.obj ./models/characters/ 3.81796 -6.1 -7.04927 0.425686 0.500002 0.331361 10 0 col 3.84151 -6.00001 -7.06794 0.492748 1.2 0.62232 end
trigger none prompt text 1 1.0
ui ./models/characters/demoman.obj ./models/characters/ 4.0 4.0 2.0 2.0 10
trigger none interacted getvar demomanheld set 1.0 0.0
trigger none is demomanheld equalto 1.0 text last 0.0
trigger none is demomanheld equalto 1.0 put -999.0 -999.0 -999.0 0.0

object ./models/characters/demoman.obj ./models/characters/ -7.07972 -6.15 -13.0394 0.323603 0.599998 0.268188 2 0 col -7.05433 -5.95 -13.0105 0.64851 1.4 0.644278 end
trigger none prompt text 0 1.0
ui ./models/dialog/2chat1.obj ./models/dialog 32 16 -4.0 4.0 0
ui ./models/dialog/2chat2.obj ./models/dialog 32 16 -4.0 4.0 0
trigger 2 interacted text 6 1.0
trigger 3 interacted text 7 1.0
trigger 3 interacted getvar demomanheld set 0.0 0.0