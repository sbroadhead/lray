scene_info = SceneInfo() {
    frames = 1,
    image_width = 800,
    image_height = 450,
    samples = 2
}

function scene(clock)
    local camera = Perspective(75, 16/9) { transform = 
        LookAt(-6*x + 2*y - z, y, y) }
    local s = Scene() { camera = camera, background_color = black }
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
        ambient = black,
        diffuse = 0.0,
        specular = 1.0,
        exponent = 160,
        refraction = 1.0,
        refraction_index = 1.1,
        absorption = 0.2
    }
    
    local teapot_finish = Finish() {
        diffuse = 0.7,
        specular = 0.9,
        exponent = 32,
        reflection = 0.3
    }
    
    local bunny = mesh_from_obj("bunny.obj") {
        transform = Translate(-2*x) * Scale(.2, .2, .2) * Rotate(y, 180),
        smooth = true,
        texture = Texture() {
            finish = bunny_finish
        }
    }
    
    local teapot = mesh_from_obj("teapot.obj") {
        transform = Translate(2*x) * Rotate(y, 90),
        smooth = true,
        texture = Texture() {
            finish = teapot_finish,
            pigment = ColorPigment(green)
        }
    }
    
    s:add_entities({bunny, teapot, floor})
    s:add_light(Light() {
        transform = Translate(-2*x + 5*y - 2*z),
        area = true,
        rows = 4,
        cols = 4
    })
    return s
end