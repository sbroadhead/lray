scene_info = SceneInfo() { frames = 20 }
function scene(clock)
    local camera = Perspective(45) { transform =
        Rotate(y, 360*clock) * Translate(-5*z) }
    local s = Scene() { camera = camera }
    for i = 1,20 do
        local tf = Rotate(z, (i/20)*360) * Translate(x)
            * Scale(.1, .1, .1)
        local sphere = Sphere(tf)
        s:add_entity(sphere)
    end
    s:add_light(Light() { transform = Translate(x - 3*z) })
    return s
end