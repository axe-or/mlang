function isComponent(o)
    return type(o) == 'table' and type(o.render) == 'function'
end

local stringableTypes = {
    ['string'] = true,
    ['number'] = true,
    ['boolean'] = true,
}

BasicContent = {}
BasicContent.__index = BasicContent

function BasicContent:new(value)
    assert(stringableTypes[type(value)], "Not a stringable type")
    local o = {
        value = value
    }
    setmetatable(o, BasicContent)
    return o
end

function BasicContent:render()
    -- TODO: Sanitize
    return tostring(self.value)
end

function wrap(content)
    if isComponent(content) then
        return content
    else
        return BasicContent:new(content)
    end
end

Tag = {}
Tag.__index = Tag

function Tag:new(name, obj)
    local o = {
        name = name,
        content = {},
        props = {},
    }
    setmetatable(o, self)

    -- Wrap in table if primitive is pass through
    if stringableTypes[type(obj)] then
        obj = { obj }
    elseif isComponent(obj) then
        obj = { obj }
    end

    -- Children items
    for i, val in ipairs(obj) do
        o.content[#o.content+1] = wrap(val)
    end

    -- Props
    -- TODO: Ordering.
    for key, val in pairs(obj) do
        if type(key) ~= "string" then
            goto continue
        end
        o.props[key] =val

        ::continue::
    end

    return o
end

-- TODO: Prop with explicit key-value
function Tag:render()
    local inner = ""
    for _, comp in ipairs(self.content) do
        inner = inner .. comp:render()
    end

    local props = ''
    for k, v in pairs(self.props) do
        -- TODO: Sanitize prop key and value
        -- TODO: camelCase to kebab-case
        props = props .. ([[ %s="%s"]]):format(k, v)
    end

    return ("<%s%s>%s</%s>"):format(self.name, props, inner, self.name)
end

function div(obj)
    return Tag:new('div', obj)
end

function h1(x) return Tag:new('h1', x) end
function h2(x) return Tag:new('h2', x) end
function h3(x) return Tag:new('h3', x) end
function h4(x) return Tag:new('h4', x) end

function ul(x) return Tag:new('ul', x) end
function li(x) return Tag:new('li', x) end

local t = div {
    h1 'Yo',
    ul {
        class="list",

        li 'Item 1',
        li 'item 2',
        li(h1 'item 3'),
    },
}

print(t:render())
