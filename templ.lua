Component = {}
Component.__index = Component

function Component:new(o)
    o = o or {}
    setmetatable(o, self)
    return o
end

function Component:__tostring()
    return self:render()
end

function Component:render()
    -- TODO: primitive types
    error('unimplemented render() method')
end

local function iscomponent(x)
    return type(x) == 'table' and getmetatable(x) == Component
end

Header = Component:new()
Header.__index = Header

function Header:new(inner, level)
    local o = {
        inner = inner,
        level = level or 1,
    }
    setmetatable(o, self)
    return o
end

function Header:render()
    local s = nil
    if iscomponent(self.inner) then
        s = self.inner:render()
    else
        s = tostring(self.inner)
    end
    return ('<h%d> %s </h%d>'):format(self.level, s, self.level)
end

function Header:__tostring()
    return self:render()
end

function h1(x) return Header:new(x, 1) end
function h2(x) return Header:new(x, 2) end
function h3(x) return Header:new(x, 3) end
function h4(x) return Header:new(x, 4) end

print(h1(h3(h1 'X')))
