-- luascene.lua 
-- lua support functions for raytracer

function union(tab1, tab2)
	for k,v in pairs(tab2) do
		tab1[k] = v
	end
	return tab1
end

-- Allow users to specifiy properties by calling objects with
-- an override table
for i,cls in ipairs { Scene,
					  Entity, Sphere, Cylinder, Cone,
					  Camera, Perspective,
					  Light,
					  Texture,
					  Pigment, ColorPigment, CheckerPigment2D, CheckerPigment3D, MarblePigment3D, WoodPigment3D, ImagePigment2D,
                      TextureMap2D, TextureMap3D,
                      UVMap,
                      LinearMap,
					  Finish,
                      SceneInfo } do
    cls.__call = union
end

-- Basic vectors
x = Vector(1, 0, 0)
y = Vector(0, 1, 0)
z = Vector(0, 0, 1)
origin = Vector(0, 0, 0)

-- Basic colors
red = Color(1, 0, 0)
green = Color(0, 1, 0)
blue = Color(0, 0, 1)
white = Color(1, 1, 1)
black = Color(0, 0, 0)
grey = function(amt) return Color(amt, amt, amt) end

-- Add a bunch of entities to a scene
function Scene:add_entities(ents)
	for k,v in pairs(ents) do
		self:add_entity(v)
	end
end

-- Add a bunch of lights to a scene
function Scene:add_lights(ents)
	for k,v in pairs(ents) do
		self:add_light(v)
	end
end

-- Make adding meshes easier
Meshtable = {}
Meshtable.__index = Meshtable
Meshtable.__call = union

function Mesh(i)
	local m = {
		vertices = {},
		indices = {},
		normals = {},
		uvs = {},
		transform = Scale(1.0, 1.0, 1.0),
		texture = Texture(),
		smooth = false,
        is_mesh_ = true
	}
	
	return setmetatable(m, Meshtable)
end

function Meshtable:copy()
    local m2 = {}
	for k,v in pairs(self) do
		m2[k] = v
	end
	return setmetatable(m2, getmetatable(self))
end

function Scene:add_mesh(mesh)
	local numverts = #mesh.vertices
	local numnormals = #mesh.normals
	local numuvs = #mesh.uvs
    
    local hasnormals = numnormals > 0
    local hasuvs = numuvs > 0
    local num_indices = 1 + (hasuvs and (hasnormals and 2 or 1) or (hasnormals and 2 or 0))
	
	local meshpoints = {}
	local meshfaces = {}
	local meshuvs = {}
    local meshnormals = {}
	for i,v in ipairs(mesh.vertices) do
		meshpoints[i] = Point(v[1], v[2], v[3])
	end
    
    for i,n in ipairs(mesh.normals) do
		meshnormals[i] = Normal(n[1], n[2], n[3])
	end
 
    for i,u in ipairs(mesh.uvs) do
		meshuvs[i] = UV(u[1], u[2])
	end
 
	for i,x in ipairs(mesh.indices) do
        for j = 1,3 do      
            if not ((type(x[j]) == "table") or (num_indices == 1 and type(x[j]) == "number")) then
                error("A mesh face vertex has the wrong number of indices. Expected: " .. num_indices)
            end
            
            if type(x[j]) == "number" then x[j] = { x[j] } end
        end        
               
        if x[1][1] > numverts or x[2][1] > numverts or x[3][1] > numverts then
            error("A face vertex index is out of range in: {" .. x[1][1] .. "," .. x[2][1] .. "," .. x[3][1] .. "}")
        end
        
		meshfaces[i] = MeshFace()
		meshfaces[i].v1 = x[1][1] - 1
		meshfaces[i].v2 = x[2][1] - 1
		meshfaces[i].v3 = x[3][1] - 1
        
        if (hasuvs) then
            if x[1][2] > numuvs or x[2][2] > numuvs or x[3][2] > numuvs then
                error("A face UV index is out of range in: {" .. x[1][2] .. "," .. x[2][2] .. "," .. x[3][2] .. "}")
            end
            meshfaces[i].t1 = x[1][2] - 1
            meshfaces[i].t2 = x[2][2] - 1
            meshfaces[i].t3 = x[3][2] - 1
        end
        
         if (hasnormals) then
            if x[1][3] > numnormals or x[2][3] > numnormals or x[3][3] > numnormals then
                error("A face normal index is out of range in: {" .. x[1][3] .. "," .. x[2][3] .. "," .. x[3][3] .. "}")
            end
            meshfaces[i].n1 = x[1][3] - 1
            meshfaces[i].n2 = x[2][3] - 1
            meshfaces[i].n3 = x[3][3] - 1
        end
	end
	AddMesh(mesh.transform, meshpoints, meshnormals, meshfaces, meshuvs, mesh.smooth, mesh.texture, self)
end

-- Load mesh from .obj
function mesh_from_obj(filename)
    local infaces = false
    local hasuvs = false
    local hasnormals = false
    
	m = Mesh()
	for line in io.lines(filename) do
		local fields = {}
        line:gsub("([^ ]+)", function(c) fields[#fields+1] = c end)
		
		if #fields > 0 then
        	if fields[1] == "f" and #fields == 4 then
                local idx = {}
                infaces = true
                
                for i = 2,4 do
                    local idx_fields = {}
                    if hasnormals then
                        if hasuvs then
                            fields[i]:gsub("([^/]+)", function(c) idx_fields[#idx_fields+1] = c end)
                            idx[i-1] = { tonumber(idx_fields[1]), tonumber(idx_fields[2]), tonumber(idx_fields[3]) }
                        else
                            fields[i]:gsub("([^//]+)", function(c) idx_fields[#idx_fields+1] = c end)
                            idx[i-1] = { tonumber(idx_fields[1]), nil, tonumber(idx_fields[2]) }
                        end
                    else
                        if hasuvs then
                            fields[i]:gsub("([^/]+)", function(c) idx_fields[#idx_fields+1] = c end)
                            idx[i-1] = { tonumber(idx_fields[1]), tonumber(idx_fields[2]) }
                        else
                            idx[i-1] = tonumber(fields[i])
                        end
                    end
                end
                
                table.insert(m.indices, {idx[1], idx[2], idx[3]})
            else
                if infaces and (fields[1] == "v" or fields[1] == "vt" or fields[1] == "vn") then
                    error("Error in OBJ loader: geometry definition after face definition")
                end
                
                if fields[1] == "v" and #fields >= 4 then
                    table.insert(m.vertices,
                        {tonumber(fields[2]), tonumber(fields[3]), tonumber(fields[4])})
                elseif fields[1] == "vt" and #fields >= 3 then
                    hasuvs = true
                    table.insert(m.uvs,
                        {tonumber(fields[2]), tonumber(fields[3])})
                elseif fields[1] == "vn" and #fields >= 4 then
                    hasnormals = true
                    table.insert(m.normals,
                        {tonumber(fields[2]), tonumber(fields[3]), tonumber(fields[4])})
                end
			end
		end
	end
	return m
end

n = io.open("cube.obj")
if not n then error("cube.obj not found") end
n:close()
Cubetable = mesh_from_obj("cube.obj")

function Cube(tf)
    c = {}
    for k,v in pairs(Cubetable) do c[k] = v end
    c.transform = tf
    return setmetatable(c, getmetatable(Cubetable))
end

-- Easy way to make a floor
function Floor()
    return Mesh() {
        vertices = {
            {-100.0, 0.0, -100.0},
            {100.0, 0.0, -100.0},
            {100.0, 0.0, 100.0},
            {-100.0, 0.0, 100.0},
        },
        uvs = {
            { -25.0, -25.0 },
            { 25.0, -25.0 },
            { 25.0, 25.0 },
            { -25.0, 25.0 }
        },
        indices = {
            { {1, 1}, {4, 4}, {2, 2} },
            { {2, 2}, {4, 4}, {3, 3} }
        }
    }
end

-- Add a mesh or entity to a scene
function Scene:add_entity(ent)
    if ent.is_mesh_ ~= nil then
        self:add_mesh(ent)
    else
        self:add_entity_(ent)
    end
end