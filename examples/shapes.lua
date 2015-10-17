scene_info = SceneInfo() {
    frames = 1,
    image_width = 1200,
    image_height = 300,
    samples = 2
}

function scene(clock)
    local camera = Perspective(35, 4) { transform = 
        LookAt(2*y - 14*z, .5*y, y) }
    local s = Scene() { camera = camera }
    local tex = Texture() {
        finish = Finish() { specular = 1.0; exponent = 128; }
    }
    local sphere = Sphere(Translate(-3*x + .5*y) * Scale(.5, .5, .5)) { texture = tex }
    local cube = Cube(Translate(-1*x + .5*y)) { texture = tex }
    local cone = Cone(Translate(1*x) * Scale(.5, 1.0, .5)) { texture = tex }
    local cylinder = Cylinder(Translate(3*x) * Scale(.5, 1.0, .5)) { texture = tex }
    local floor = Floor() {
        texture = Texture() {
            pigment = CheckerPigment2D() {
                pigment1 = ColorPigment(white),
                pigment2 = ColorPigment(Color(0.0, 0.4, 0.0))
            }
        }
    }
    
    s:add_entities({sphere, cube, cone, cylinder, floor})
    s:add_light(Light() {
        transform = Translate(3*y - 2*z),
        area = true,
        rows = 8,
        cols = 8
    })
    return s
end