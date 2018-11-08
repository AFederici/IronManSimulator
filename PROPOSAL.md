# AJ Federici - CS126 Project Proposal

## The project overview - Flick Soccer

My project idea is to be able to play a simple soccer minigame involving shooting into a goal as well as defending it.  I plan to have it be played using hand gestures as well.  The project can be broken down into a few parts - the graphics of the game, implementing finger/hand detection, the game itself (will need some sort of physics engine), and at minimum a naive AI to play against.

AI  ------> physics engine ---------||
						         V
Detection -> physics engine -> visuals

### Graphics
I imagine this part will be the most difficult/tricky to work with.  I will likely use a static image as a background and then only have to deal with moving players and the ball.  At a minimum I can use a colored rectangle or something very simple for the players so that I'm not held back by this portion of the project.  I would probably have the offensive player be limited to a constant y value and some set of x values offset to the left of the goal in order to make it seem more 3D like.   The ball may be a little harder to deal with, but I imagine taking the starting coordinates and the end coordinates from the physics engine and making it move in an arc from point a to point b.

OpenFrameworks game development article: https://openframeworks.cc/ofBook/chapters/game_design.html

### Finger/Hand Detection
My idea here is to use OpenCV for the finger and hand detection.  I have experience working with openCV for things like facial recognition and found an [article](https://picoledelimao.github.io/blog/2015/11/15/fingertip-detection-on-opencv/) on fingertip detection.  Being able to detect fingertips as well as having a hand outline should make it pretty easy to extrapolate features such as finger placement/center of the hand/etc.  Now, how the finger/hand detection will work in terms of the game is different for offense and defense.  As far as defense goes, this is simper as the movements will basically be moving your hand left and right and hopefully also when moved up will move the defender up, simulating a jump based, and the character will move back down over time based on some simple physics stuff.  This would be taken care of in the physics engine, which could prevent the user from jumping until their coordinates are back on the ground.  Essentially, the defender part only needs to track the overall position of a hand.  As far as offense, my idea is to use a flick mechanic.  Essentially, you will have the same left right movement of the hand overall like on defense, which will move the kicker and the ball left or right.  For shooting, the goal is to flick your finger out at an angle, and the detector will calculate that angle as well as the power put into the kick based upon how quickly the finger was fully extended.  The angle should be calculated by finding the furthest out distance from your hand to a connected component.  Image flicking straight, from the biew of the camera there would be basically no extension from your overall hand.  If you flick at a hard angle left or right, the overall outline of the hand will be much longer.  So, I would have to come up with some conversion between this hand length and a numerical angle.

### Physics Engine 
I imagine this part would be relatively simple, with the main struggle being testing the input values and maybe tweaking constants to get better outputs as I imagine the finger/hand detection will be kind of rough.  [This link](http://www.ofxaddons.com/categories/3-physics) has a few useful openFrameworks physics libraries that may be useful.  One of which is object collision which might be useful for when the goalie blocks a shot.

Additional source: http://bebensiganteng.com/index.php/2013/07/01/simple-physics-c/

### AI
The idea here is to at a minimum generate naive commands that can be sent to the phyiscs engine.  As a defender, simple left and right commands that would shift the posiiton some set amount left or right as well as possible random jumps.  Similarly, for offense there would be random left and right movements and send some random velocity and angle for the ball to the physics engine, just as the detector sends these values.


## Stretch Goals
While the project may already be challenging, in the case that I finish the base tasks or decide to put in an extra amount of work to the project for fun, there are a few stretch goals I have in mind.  The first would be to to allow a two player game.  I would implement this by essentially viewing anything detected on the left side of the screen as player one and anything on the right side as player two, allowing a goalie and a scorer to play at the same time.  Some challenges with this that I foresee is the confusion for player2 caused by the goalies movements being mirrored (moving right if the players hand moves left) as well as the possible strain of doing double the computation.  A second goal I have in mind is just creating a more complicated AI for scoring as well as playing goalie.
