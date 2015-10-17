scene_info = SceneInfo() {
    frames = 1,
    image_width = 800,
    image_height = 300,
    samples = 2
}

function scene(clock)
    local camera = Perspective(75, 8/3) { transform = 
        LookAt(2*y - 5*z, y, y) }
    local s = Scene() { camera = camera }
    local tex = Texture() {
        finish = Finish() { specular = 1.0; exponent = 128; },
        pigment = ColorPigment(red)
    }

    local floor = Floor() {
        texture = Texture() {
            pigment = CheckerPigment2D() {
                pigment1 = ColorPigment(white),
                pigment2 = ColorPigment(red)
            }
        }
    }
    
    local bunny_finish = Finish() {
        diffuse = 1.0,
        specular = 0.9,
        exponent = 160
    }
    local master_bunny = mesh_from_obj("bunny.obj") {
        transform = Scale(.2, .2, .2) * Rotate(y, 180),
        smooth = true
    }
    local checker_bunny = master_bunny:copy() { 
        transform = Translate(-2*x) * master_bunny.transform,
        texture = Texture() {
            finish = bunny_finish,
            pigment = CheckerPigment3D() {
                pigment1 = ColorPigment(Color(1.0, 0.50, 0.72)),
                pigment2 = ColorPigment(Color(0.5, 1.0, 0.5)),
                texture_map = LinearMap() { transform = Scale(4.0, 4.0, 4.0) }
            }
        }
    }
    local marble_bunny = master_bunny:copy() { 
        texture = Texture() {
            finish = bunny_finish,
            pigment = MarblePigment3D() {
                color1 = Color(0.7, 0.7, 1.0),
                color2 = Color(0.3, 0.3, 0.7),
                period = Vector(3.0, 3.0, 3.0),
                power = 3.0,
                size = 0.2,
                texture_map = LinearMap() { transform = Scale(4.0, 4.0, 4.0) }
            }
        }
    }
    local wood_bunny = master_bunny:copy() { 
        transform = Translate(2*x) * master_bunny.transform,
        texture = Texture() {
            finish = bunny_finish,
            pigment = WoodPigment3D() {
                color1 = Color(1.0, 0.89, 0.48),
                color2 = Color(0.67, 0.4, 0.24),
                period = Vector(512.0, 512.0, 512.0),
                power = 4.0,
                size = 0.2,
                texture_map = LinearMap() { transform = Scale(4.0, 4.0, 4.0) }
            }
        }
    }
    
    s:add_entities({checker_bunny, marble_bunny, wood_bunny, floor})
    s:add_light(Light() {
        transform = Translate(2*x + 5*y - 2*z),
        area = true,
        rows = 4,
        cols = 4
    })
    return s
end