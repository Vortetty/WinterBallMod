--[[
 __          __ _         _                  _             _  _
 \ \        / /(_)       | |                | |           | || |
  \ \  /\  / /  _  _ __  | |_  ___  _ __    | |__    __ _ | || |
   \ \/  \/ /  | || '_ \ | __|/ _ \| '__|   | '_ \  / _` || || |
    \  /\  /   | || | | || |_|  __/| |      | |_) || (_| || || |
     \/  \/    |_||_| |_| \__|\___||_|      |_.__/  \__,_||_||_|
]]

function init()
    initCommonParameters()

    --
    -- Sit Variables
    -- 
    self.sitting = false
    SitPosition = mcontroller.position()

    --
    -- Rotation Variables
    --
    self.Rotation = false
    Rotation = 0

    --
    -- Scaling Variables
    --
    self.Resized = false
    Size = 1

    --
    -- NoClip Variables
    --
    self.NoClip = false
    NoClipPosition = mcontroller.position()
    status.setPersistentEffects("distortionspheretech", {{
        stat = "invisible",
        amount = 0
    }})

    --
    -- Blink Variables
    --
    self.mode = "none"
    self.timer = 0

    --
    -- Failsafe Variables
    --
    F_failsafe = false
    G_failsafe = false
    H_failsafe = false

    --
    -- Spawns blink particles
    -- TODO: Replace entire blink function with something fancy that lerps particles over to the new position in a random time from 0.1s to 1s
      -- TODO: Generate particle sets for all possible parent states(requires images of player) and make them into json files
      -- TODO: Load json files into memory and map them to the parent states
      -- TODO: Add Sine in-out easing function and expo out easing function, test what looks best
      -- TODO: Hide player, spawn particles, wait for them to all have lerped, unhide player and remove particles
    --
    function BlinkParticles()
        world.spawnProjectile("bullet-1", mcontroller.position(), entity.id(), {0, 0}, false, {
            processing = "?multiply=00000000",
            timeToLive = 0.1,
            damageType = "NoDamage",
            actionOnReap = {{
                action = "loop",
                count = 8,
                body = {{
                    action = "particle",
                    specification = {
                        type = "animated",
                        animation = "/animations/hoverbikethruster/thrustertrail.animation",
                        timeToLive = 0.8,
                        layer = "front",
                        variance = {
                            position = {-1.5, 1.5},
                            angularVelocity = math.random(0, 180),
                            rotation = 0,
                            velocity = {2, 2}
                        }
                    }
                }}
            }}
        })
    end

    --
    -- This function resets size and rotation (currently disabled features)
    --
    function ResetParameters()
        Size = 1
        Rotation = 0
        mcontroller.setRotation(math.pi * Rotation) -- Resets the rotation
        self.Resized = false -- Prevents collision issues involving
        self.Rotated = false -- Rotation and Resizing during ball form
    end

    --
    -- Displays popup texts
    --
    previousTime = os.clock()
    function TextNotification(textPopup)
        local currentTime = os.clock()
        if currentTime - previousTime > 1 then -- Prevents spam of popups
            world.spawnProjectile("bullet-1", mcontroller.position(), entity.id(), {0, 0}, false, {
                processing = "?multiply=00000000",
                timeToLive = 0.1,
                damageType = "NoDamage",
                actionOnReap = {{
                    action = "particle",
                    rotate = false,
                    specification = {
                        type = "text",
                        color = {0, 150, 255, 255},
                        destructionAction = "fade",
                        destructionTime = 2.0,
                        flippable = false,
                        layer = "front",
                        text = textPopup,
                        velocity = {0, 2},
                        timeToLive = 1.0,
                        size = 0.75
                    }
                }}
            })
            previousTime = currentTime
        end
    end
end

function loadConfigJson() -- Loads the config.json file
    self.ballConfigJson = root.assetJson("/tech/distortionsphere/config.jsonc")
    self.ballFrames = self.ballConfigJson["drawables"]
    self.ballScale = self.ballConfigJson["scale"] or 1
    self.ballRadius = self.ballScale/2
    self.ballFrameCount = self.ballConfigJson["frameCount"]
    self.allowInterractWhileInBall = self.ballConfigJson["allowInterract"]
    self.animationSpeedDivisor = self.ballConfigJson["animationSpeedDivisor"] or 1
end

function initCommonParameters()
    self.angularVelocity = 0
    self.active = false -- Sphere state active
    self.angle = 0
    self.transformFadeTimer = 0

    self.energyCost = config.getParameter("energyCost")
    --self.ballFrames = config.getParameter("ballFrames")
    self.ballSpeed = config.getParameter("ballSpeed")

    loadConfigJson()

    self.transformFadeTime = config.getParameter("transformFadeTime", 0.3)
    self.transformedMovementParameters = config.getParameter("transformedMovementParameters")
    self.transformedMovementParameters.runSpeed = self.ballSpeed
    self.transformedMovementParameters.walkSpeed = self.ballSpeed
    self.transformedMovementParameters.collisionPoly = { 
        {-0.425*self.ballScale, -0.225*self.ballScale}, 
        {-0.225*self.ballScale, -0.425*self.ballScale}, 
        {0.225*self.ballScale, -0.425*self.ballScale}, 
        {0.425*self.ballScale, -0.225*self.ballScale}, 
        {0.425*self.ballScale, 0.225*self.ballScale}, 
        {0.225*self.ballScale, 0.425*self.ballScale}, 
        {-0.225*self.ballScale, 0.425*self.ballScale}, 
        {-0.425*self.ballScale, 0.225*self.ballScale} 
    }
    self.basePoly = mcontroller.baseParameters().standingPoly
end



function attemptActivation()
    if not self.active and not tech.parentLounging() -- and not status.statPositive("activeMovementAbilities")
    and status.overConsumeResource("energy", self.energyCost) then

        local pos = transformPosition()
        if pos then
            mcontroller.setPosition(pos)
            activate()
        end
    elseif self.active then
        local pos = restorePosition()
        if pos then
            mcontroller.setPosition(pos)
            deactivate()
        else
            -- error noise?
        end
    end
end

function storePosition()
    if self.active then
        storage.restorePosition = restorePosition()

        -- try to restore position. if techs are being switched, this will work and the storage will
        -- be cleared anyway. if the client's disconnecting, this won't work but the storage will remain to
        -- restore the position later in update()
        if storage.restorePosition then
            storage.lastActivePosition = mcontroller.position()
            mcontroller.setPosition(storage.restorePosition)
        end
    end
end

function restoreStoredPosition()
    if storage.restorePosition then
        -- restore position if the player was logged out (in the same planet/universe) with the tech active
        if vec2.mag(vec2.sub(mcontroller.position(), storage.lastActivePosition)) < 1 then
            mcontroller.setPosition(storage.restorePosition)
        end
        storage.lastActivePosition = nil
        storage.restorePosition = nil
    end
end

function updateAngularVelocity(dt)
    if mcontroller.onGround() then
        -- If we are on the ground, assume we are rolling without slipping to
        -- determine the angular velocity
        local positionDiff = world.distance(self.lastPosition or mcontroller.position(), mcontroller.position())
        self.angularVelocity = -(vec2.mag(positionDiff)/vec2.mag({self.animationSpeedDivisor, self.animationSpeedDivisor})) / dt / self.ballRadius

        if positionDiff[1] > 0 then
            self.angularVelocity = -self.angularVelocity
        end
    end
end

function uninit()
    storePosition()
    deactivate()
end
function update(args)
    restoreStoredPosition()
    if not args.moves["special1"] then
        F_failsafe = false -- Toggles failsafe once key is let go
    end
    if not args.moves["special2"] then
        G_failsafe = false -- Toggles failsafe once key is let go
    end
    if not args.moves["special3"] then
        H_failsafe = false -- Toggles failsafe once key is let go
    end

    --[[
      _____                   _       _                     _ _
     |_   _|                 | |     | |                   | | |
       | |  _ __  _ __  _   _| |_    | |__   __ _ _ __   __| | | ___ _ __ ___
       | | | '_ \| '_ \| | | | __|   | '_ \ / _` | '_ \ / _` | |/ _ \ '__/ __|
      _| |_| | | | |_) | |_| | |_    | | | | (_| | | | | (_| | |  __/ |  \__ \
     |_____|_| |_| .__/ \__,_|\__|   |_| |_|\__,_|_| |_|\__,_|_|\___|_|  |___/
                 | |
                 |_|
    ]]

    -- +------------+
    -- | Ball input |
    -- +------------+
    if not F_failsafe and args.moves["special1"] and args.moves["jump"] then
        F_failsafe = true -- Prevents multiple inputs
        ResetParameters() -- Reset collision poly
        attemptActivation() -- Attempt to activate ball form
    end
    -- +-------------+
    -- | Blink input |
    -- +-------------+
    if args.moves["special1"] and args.moves["down"] and not F_failsafe then
        self.mode = "startBlink" -- Begins transition into the animation of blinking
        F_failsafe = true -- Prevents multiple inputs
    end
    -- +--------------+
    -- | NoClip input |
    -- +--------------+
    if args.moves["special1"] and args.moves["up"] and not F_failsafe and not self.sitting then
        F_failsafe = true -- Prevents multiple inputs
        self.NoClip = not self.NoClip -- Toggles no-clip on/off
        NoClipPosition = mcontroller.position() -- Sets starting no-clip position to current position
        currentVelocity = {0, 0} --mcontroller.velocity() -- Sets starting no-clip velocity to current velocity
    end
    -- +-----------+
    -- | Sit input |
    -- +-----------+
    if args.moves["special2"] and not args.moves["run"] and not G_failsafe and not self.NoClip then
        G_failsafe = true -- Prevents multiple inputs
        self.sitting = not self.sitting -- Toggles sit on/off
        SitPosition = mcontroller.position() -- Sets starting sit position to current position
    end

    --[[
       _____            _             _ _
      / ____|          | |           | | |
     | |     ___  _ __ | |_ _ __ ___ | | | ___ _ __ ___
     | |    / _ \| '_ \| __| '__/ _ \| | |/ _ \ '__/ __|
     | |___| (_) | | | | |_| | | (_) | | |  __/ |  \__ \
      \_____\___/|_| |_|\__|_|  \___/|_|_|\___|_|  |___/
    ]]

    -- +-----------------+
    -- | Ball controller |
    -- +-----------------+
    if self.active then
        mcontroller.controlParameters(self.transformedMovementParameters) -- Sets poly to match ball form
        status.setResourcePercentage("energyRegenBlock", 1.0) -- Prevents energy recovery while in ball form
        updateAngularVelocity(args.dt)
        updateRotationFrame(args.dt)
    end
    updateTransformFade(args.dt)
    self.lastPosition = mcontroller.position()

    -- +------------------+
    -- | Blink controller |
    -- +------------------+
    if self.mode == "startBlink" then
        status.setPersistentEffects("distortionspheretech", {{
            stat = "invisible",
            amount = 1
        }}) -- Sets player invisible during blink
        mcontroller.setVelocity({0, 0}) -- Prevents movement/momentum
        self.mode = "out" -- Transition to next state in blink animation
        self.timer = 0
    elseif self.mode == "out" then

        mcontroller.setVelocity({0, 0}) -- Prevents movement/momentum
        self.timer = self.timer + args.dt

        if self.timer > 0.15 then
            BlinkParticles() -- Calls function to spawn particles

            mcontroller.setPosition(tech.aimPosition()) -- Sets player at cursor position
            if self.NoClip then
                NoClipPosition = tech.aimPosition()
            end -- Updates the noclip position if active
            if self.sitting then
                SitPosition = tech.aimPosition()
            end -- Updates the sit position if active

            self.mode = "in" -- Transitions to last blink state
            self.timer = 0
        end

    elseif self.mode == "in" then

        mcontroller.setVelocity({0, 0}) -- Prevents movement/momentum
        self.timer = self.timer + args.dt

        if self.timer > 0.15 then
            BlinkParticles() -- Calls function to spawn particles
            status.setPersistentEffects("distortionspheretech", {{
                stat = "invisible",
                amount = 0
            }}) -- Sets player to visible once blink ends
            self.mode = "none" -- Transition to end state, marking function as finished
        end

    end

    -- +-------------------+
    -- | NoClip controller |
    -- +-------------------+
    if self.NoClip then
        mcontroller.controlParameters({ -- Performs the resize
            standingPoly = {},
            crouchingPoly = {},
        })
        -- Makes sure player doesn't recieve fall damage while in Noclip
        mcontroller.setVelocity({0, 0})
        currentPosition = mcontroller.position()
        targetVelocity = {0, 0}
        newVelocity = {0, 0}
        tech.setParentState("Fly")
        
        newVelocity = {
            currentVelocity[1] * (1 - math.min(args.dt*2.5, 1)),
            currentVelocity[2] * (1 - math.min(args.dt*2.5, 1))
        }

        -- Normal Noclip
        if args.moves["left"] and currentVelocity[1] > -1 then
            newVelocity[1] = currentVelocity[1] - (0.75 * args.dt)
        end
        if args.moves["right"] and currentVelocity[1] < 1 then
            newVelocity[1] = currentVelocity[1] + (0.75 * args.dt)
        end
        if args.moves["up"] and currentVelocity[2] < 1 then
            newVelocity[2] = currentVelocity[2] + (0.75 * args.dt)
        end
        if args.moves["down"] and currentVelocity[2] > -1 then
            newVelocity[2] = currentVelocity[2] - (0.75 * args.dt)
        end

        mcontroller.setPosition(vec2.add(currentPosition, newVelocity))
        currentVelocity = newVelocity
    end

    -- +----------------+
    -- | Sit controller |
    -- +----------------+
    if self.sitting then

        -- Adds each movement to adjustment sit position
        if args.moves["left"] then
            SitPosition = vec2.add(SitPosition, {-0.05, 0})
        elseif args.moves["right"] then
            SitPosition = vec2.add(SitPosition, {0.05, 0})
        elseif args.moves["up"] then
            SitPosition = vec2.add(SitPosition, {0, 0.05})
        elseif args.moves["down"] then
            SitPosition = vec2.add(SitPosition, {0, -0.05})
        end

        -- This repositions player according to each movement while sitting is active
        mcontroller.setPosition(SitPosition)
        mcontroller.setVelocity({0, 0}) -- Prevents movement/momentum
    end

    -- +--------------------------+
    -- | Parent state controllers |
    -- +--------------------------+
    if self.sitting then
        tech.setParentState("sit") -- Sets player state to sit pose
    elseif self.NoClip then
        tech.setParentState("fly") -- Sets player state to sit pose
    else
        tech.setParentState() -- Toggles off forced sit pose
    end

    -- +-----------------------------+
    -- | Rotation Functions DISABLED |
    -- +-----------------------------+
        -- Slower rotation speed
        --if args.moves["special3"] and args.moves["right"] and not args.moves["run"] and not self.active then
        --    self.Rotated = true -- Activates rotation
        --    Rotation = Rotation - 0.01 -- Adjusts rotation
        --elseif args.moves["special3"] and args.moves["left"] and not args.moves["run"] and not self.active then
        --    self.Rotated = true -- Activates rotation
        --    Rotation = Rotation + 0.01 -- Adjusts rotation

        --    -- Normal rotation speed
        --elseif args.moves["special3"] and args.moves["right"] and not self.active then
        --    self.Rotated = true -- Activates rotation
        --    Rotation = Rotation - 0.05 -- Adjusts rotation
        --elseif args.moves["special3"] and args.moves["left"] and not self.active then
        --    self.Rotated = true -- Activates rotation
        --    Rotation = Rotation + 0.05 -- Adjusts rotation
        --end

        --if self.Rotated then -- Performs the rotation
        --    mcontroller.setRotation(math.pi * Rotation)
        --end

    -- +----------------------------+
    -- | Scaling Functions DISABLED |
    -- +----------------------------+
        -- if args.moves["special2"] and args.moves["up"] and not G_failsafe and not self.active then
        --  status.setStatusProperty("savedScaling", Size)  --Saves current size value
        -- TextNotification("Saving size value: "..tostring(Size))  --Displays popup of current size value
        -- G_failsafe = true  --Prevents multiple inputs
        -- end

        -- if args.moves["special2"] and args.moves["down"] and not G_failsafe and not self.active then
        --  Size = status.statusProperty("savedScaling", 1)  --Sets size to previously saved value, if no value is saved use 1
        -- self.Resized = true  --Activates resize
        -- G_failsafe = true  --Prevents multiple inputs
        -- end

        --if args.moves["special3"] and args.moves["down"] and not self.active then
        --    self.Resized = true -- Activates resize
        --    Size = Size / 1.01 -- Adjusts resize
        --end
    
        --if args.moves["special3"] and args.moves["up"] and not self.active then
        --    self.Resized = true -- Activates resize
        --    Size = Size * 1.01 -- Adjusts resize
        --end
    
        --if self.Resized and not self.NoClip then
        --    if Size > 10 then -- Upper bounds the resize
        --        Size = 10
        --    elseif Size < 0.01 then -- Lower bounds the resize
        --        Size = 0.01
        --    end
    
        --    tech.setParentDirectives("?scalenearest=" .. tostring(Size)) -- Makes visual player image match the resize performed
        --    mcontroller.controlParameters({ -- Performs the resize
        --        standingPoly = {{-0.75 * Size, -2.0 * Size}, {-0.35 * Size, -2.5 * Size}, {0.35 * Size, -2.5 * Size},
        --                        {0.75 * Size, -2.0 * Size}, {0.75 * Size, 0.65 * Size}, {0.35 * Size, 1.22 * Size},
        --                        {-0.35 * Size, 1.22 * Size}, {-0.75 * Size, 0.65 * Size}},
        --        crouchingPoly = {{-0.75 * Size, -2.0 * Size}, {-0.35 * Size, -2.5 * Size}, {0.35 * Size, -2.5 * Size},
        --                         {0.75 * Size, -2.0 * Size}, {0.75 * Size, -1.0 * Size}, {0.35 * Size, -0.5 * Size},
        --                         {-0.35 * Size, -0.5 * Size}, {-0.75 * Size, -1.0 * Size}}
        --    })
        --end

end


--[[
  ____        _ _                     _             _ _ _                  __                  _   _
 |  _ \      | | |                   | |           | | (_)                / _|                | | (_)
 | |_) | __ _| | |     ___ ___  _ __ | |_ _ __ ___ | | |_ _ __   __ _    | |_ _   _ _ __   ___| |_ _  ___  _ __  ___
 |  _ < / _` | | |    / __/ _ \| '_ \| __| '__/ _ \| | | | '_ \ / _` |   |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
 | |_) | (_| | | |   | (_| (_) | | | | |_| | | (_) | | | | | | | (_| |   | | | |_| | | | | (__| |_| | (_) | | | \__ \
 |____/ \__,_|_|_|    \___\___/|_| |_|\__|_|  \___/|_|_|_|_| |_|\__, |   |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                                                 __/ |  
                                                                |___/    
]]

-- TODO: Make program to generate the json(cross-platform ofc)
function updateRotationFrame(dt)
    self.angle = math.fmod(math.pi * 2 + self.angle + self.angularVelocity * dt, math.pi * 2)
    
    -- Convert angle to frame number
    -- self.angle is a number between 0 and pi
      -- For the default animation of 16 frames, you can make it select any frame using `pi-(pi/16 * <frame>)`
    -- The way this works is pretty much  angle -> percentage -> number of frames passed -> frame #
    local rotationFrame = math.floor(self.angle / math.pi * self.ballFrameCount) % self.ballFrameCount
    --animator.setGlobalTag("rotationFrame", rotationFrame)
    animator.setGlobalTag("ballDirectives", self.ballFrames[rotationFrame + 1])
end

function updateTransformFade(dt)
    if self.transformFadeTimer > 0 then
        self.transformFadeTimer = math.max(0, self.transformFadeTimer - dt)
        -- animator.setGlobalTag("ballDirectives", string.format("?fade=FFFFFFFF;%.1f", math.min(1.0, self.transformFadeTimer / (self.transformFadeTime - 0.15))))
    elseif self.transformFadeTimer < 0 then
        self.transformFadeTimer = math.min(0, self.transformFadeTimer + dt)
        tech.setParentDirectives(string.format("?fade=FFFFFFFF;%.1f", math.min(1.0, -self.transformFadeTimer /
            (self.transformFadeTime - 0.15))))
    else
        -- animator.setGlobalTag("ballDirectives", "")
        tech.setParentDirectives()
    end
end

function positionOffset()
    return minY(self.transformedMovementParameters.collisionPoly) - minY(self.basePoly)
end

function transformPosition(pos)
    pos = pos or mcontroller.position()
    local groundPos = world.resolvePolyCollision(self.transformedMovementParameters.collisionPoly,
        {pos[1], pos[2] - positionOffset()}, 1)
    if groundPos then
        return groundPos
    else
        return world.resolvePolyCollision(self.transformedMovementParameters.collisionPoly, pos, 1)
    end
end

function restorePosition(pos)
    pos = pos or mcontroller.position()
    local groundPos = world.resolvePolyCollision(self.basePoly, {pos[1], pos[2] + positionOffset()}, 1)
    if groundPos then
        return groundPos
    else
        return world.resolvePolyCollision(self.basePoly, pos, 1)
    end
end

function activate()
    if not self.active then
        animator.burstParticleEmitter("activateParticles")
        animator.playSound("activate")
        animator.setAnimationState("ballState", "activate")
        self.angularVelocity = 0
        self.angle = 0
        self.transformFadeTimer = self.transformFadeTime
    end
    tech.setParentHidden(true)
    tech.setParentOffset({0, positionOffset()})
    if not self.allowInterractWhileInBall then
        tech.setToolUsageSuppressed(true)
        status.setPersistentEffects("movementAbility", {{stat = "activeMovementAbilities", amount = 1}})
    end
    -- status.setPersistentEffects("movementAbility", {{stat = "activeMovementAbilities", amount = 1}})
    status.setPersistentEffects("ballInfo", {{
        stat = "active",
        amount = 1
    }})
    self.active = true
    -- animator.setGlobalTag("ballDirectives", "")
end

function deactivate()
    if self.active then
        animator.burstParticleEmitter("deactivateParticles")
        animator.playSound("deactivate")
        animator.setAnimationState("ballState", "deactivate")
        self.transformFadeTimer = -self.transformFadeTime
    else
        animator.setAnimationState("ballState", "off")
    end
    animator.setGlobalTag("ballDirectives", "")
    tech.setParentHidden(false)
    tech.setParentOffset({0, 0})
    tech.setToolUsageSuppressed(false)
    status.clearPersistentEffects("movementAbility")
    status.clearPersistentEffects("ballInfo")
    self.angle = 0
    self.active = false
end

function minY(poly)
    local lowest = 0
    for _, point in pairs(poly) do
        if point[2] < lowest then
            lowest = point[2]
        end
    end
    return lowest
end
