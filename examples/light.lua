scene_info = SceneInfo() {
    frames = 1,
    image_width = 800,
    image_height = 600,
    samples = 2
}

function scene(clock)
    local camera = Perspective(25) { transform = 
        LookAt(2*y - 15*z, y, y) }
    local s = Scene() { camera = camera }
    local tex = Texture() {
        finish = Finish() { specular = 1.0; exponent = 128; },
        pigment = ColorPigment(red)
    }

    local floor = Floor() {
        texture = Texture() {
            pigment = ColorPigment(white)
        }
    }
    
    s:add_entities({Sphere(Translate(y)), floor})
    s:add_light(Light() {
        transform = Translate(3*y + 2*z)
    })
    return s
end