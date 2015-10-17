scene_info = SceneInfo() {
    frames = 1,
    image_width = 800,
    image_height = 800,
    samples = 2
}

-- EARTH TEXTURE IS FROM
-- http://www.shadedrelief.com/natural3/pages/textures.html

function scene(clock)
    local camera = Perspective(75, 1) { transform = 
        Translate(-2*z)
    }
    local s = Scene() { camera = camera }
    local tex = Texture() {
        finish = Finish() { specular = 0.0; diffuse = 1.0; },
        pigment = ImagePigment2D("earth.tga")
    }
    local earth = Sphere(Rotate(x, -13)) {
        texture = tex
    }
    
    s:add_entities({earth})
    s:add_light(Light() {
        transform = Translate(2*x + 2*y - 2*z),
        area = true,
        rows = 4,
        cols = 4
    })
    return s
end