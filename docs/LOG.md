# Feat - cameraLimits.md

## Trying to limit camera vertical rotation

### Investigating the codebase

I'm a bit rusty on how LinearAlgebra and the codebase works, so I investigated:

The code where the camera is rotation is `PlayerSystem::moveCameraWithMouse(,)`.

It achieves camera rotation by altering the variable `rotation`, of type `glm::quat`, of the player's `Position_Component`. 

RenderSystem will later call `Camera::setMVPE(,)` which will query the `EntityManager` entity_manager for the player's position component and pass it to `Camera::setViewMatrix(,)`. 

This method will pass it to `Camera::getViewMatrix(,)` (a method which returns a `glm::mat4` suitable for our shader programs).

`getViewMatrix()` uses `MatrixStack::lookAt(vec3 eye, vec3 target, vec3 up)`, which wraps `glm::lookAt(,,)`.
  1. It first uses the cameras rotation to transform the point (0,0,1), the "no rotation identity", and stores the transformed point in the var `identity`
  2. It then adds the players position `vec3` to this `vec3` called `identity`
  3. It then calls `lookAt(position, identity, upVector)` 
    - This results in a correct mat4 which rotates the camera.

### Solution Try One

So it seems the `(0,0,1)` is the "default" rotation. Meaning that starting the game and turning 90degrees to the right should result in the "identity" being `(1,0,0)`. Testing this reveals it result in `(-1,0,0)`, so basically correct.

Testing also reveals that the camera passes over the "top" or "bottom" when the X or Z in this resulting identity switches from negative to positive (depending upon which way the camera is facing), but (most importantly) when the Y approachs + or - 1.0.

So I should be able to test if the Y is approaching a + or - threshold (0.95 maybe) and if it is provide a counter rotation somehow.

#### Figuring out how to calculate the counterrotation.

I should be able to calculate how much we need to rotate in the range [-1.0,1.0] and then convert this to radians (multiply by PI).

Then I can apply this rotation to the player's camera rotation the same way I move the camera with the mouse.

##### Fixing it

So that pretty much worked, but it's 'jumping' up everytime we exceed the limit, which shouldn't be happening. This means we are counter-rotating too much for how much we have exceeded the camera limits.

Testing more, it also doesn't seem to limit it vertically, for some reason.

Thinking about it, it's probably because we're converting from the range [-1,1] to [-pi,pi] when we need to do [-1,1]->[0,pi]. So we need to do (Y+1.0)/2.0 before anything. (to move [-1,1] to [0,1])

This doesn't work. Logically I'm beginning to understand why it doesn't make sense. Example: verticalLimit=0.8, identiy.y=0.83, deltaChange=0.03. (change+1)/2.0 = about 0.5. 0.5 change means putting the view about straight ahead, which is clearly way too much.
It does make sense because the deltaChange=0.03 is _how much to move along that range_, so it should be converted from 0.03 out of 2.0 total to out of 1.0 total, so just 0.03 * 1/2. 


##### Fixing it 2.0

So the fix mentioned at the ned of the last section _mostly_ worked. There is still problems with the view bouncing back into the limits, but it's not as pronounced as before. There is also still (a more important) problem of the view being about to "flip" by going over the top. Both of these should be adressed by doing some calculation _before_ the mouse view movement is applied. 

After some thought this is probably because we can't simply convert from [-1,1]->[0,1] -> [0, pi]. This is because our [-1,1] is the height from the X axis, and the ratio of an angle to Y is (obviously) not constant. So I think we need to use Trig to find the actual radians the camera is currently about the flat plane. 

So using SOH CAH TOA, Tan(Rad)=Opposite/Adjacent. Opposite is the Y, [-1,1] we've been using here. Adjacent should be (X^2+Z^2)^1/2. So we get Adjacent then do invTan(Y/Adjacent).

This pretty much worked. However, there is still jumping if the camera is moving quickly. We do need to investigate applying it before mouse view movement is...

##### More fixing

So clearly this shouldn't be this difficult, and I feel a bit silly. I simply don't understand what's going on with these rotations well enough. I should probably review them or something. Either way, this has motivated me to add debug visualizations to the engine (show rotations of objects and stuff).

My current attempt to achieve this is 

    if(xRotationDegrees < -xRotationLimit) {
        float counterRotationDegrees = (-xRotationLimit - xRotationDegrees);
        float counterRotationRadians = radians(counterRotationDegrees);
        glm::quat counterRotationY = glm::angleAxis(1.0f* counterRotationRadians, relative_x_axis);
        position->rotation = counterRotationY * position->rotation;
    }

Which works fine until you look to the left, and then it runs into problems of the EulerAngles crossing from 180to-180 and such. Also, it seems the way that quats store angles kinda 'drifts' over time, in that the Euler Angle's I'm getting out of the quat has a slowly increasing/decreasing rotation around the Z axis. 

##### Actual solution

What I need to do is get the camera's rotation around it's relative X axis and counterrotate if it's too extreme. So first I need to get the rotation.

Rotating the no rotation identity (0,0,1) by the current camera rotation should get me a vector whose angle from the XZ plane is what we want. I should then be able to do asin(thatVec.y) to get that angle.

This seems to pretty much work. I AM going to have to do some work to see if the current camera rotaiton will cross over the top or bottom of the view, because it's not possible to detect it after the fact.

##### More testing

So I've been working on this for a while and have done all matter of incorrect things.
I'm going to commit this log and then overwrite it and come back to this issue at another time.

My last attempt IS working. The camera is limited with no jittering. However there seem to be bugs with vertical camera movement not being straight up and down, and becoming 'bugged' (super messed up) if you spin at around at the lower limit.

The not straight up and down could have been there before, I'm just noticing it now.

I'm going to commit, keep this branch incomplete, and come back to the issue another time.