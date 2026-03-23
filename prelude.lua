-- Prelude of core functions to use for code generation
function map(list, fn)
    assert(type(list) == 'table', "cannot map over non table")
    local res = {}

    if not list then
        return res
    end

    for i, v in ipairs(list) do
        res[i] = fn(v)
    end
    return res
end

function filter(table, pred)
    assert(type(table) == 'table', "cannot filter over non table")
    local res = {}
    for k, v in pairs(table) do
        if pred(v, k) then
            res[k] = v
        end
    end
    return res
end

function append(list, v)
    assert(type(list) == 'table', "cannot append to non table")
    list = list or {}
    list[#list+1] = v
    return list
end

-- Multi line string
function R(s)
  local lines = {}
  for line in (s .. "\n"):gmatch("([^\n]*)\n") do
    lines[#lines + 1] = line
  end

  while lines[1] and lines[1]:match("^%s*$") do
    table.remove(lines, 1)
  end

  while lines[#lines] and lines[#lines]:match("^%s*$") do
    table.remove(lines)
  end

  local min_indent = math.huge
  for _, line in ipairs(lines) do
    if not line:match("^%s*$") then
      local indent = #line:match("^(%s*)")
      if indent < min_indent then
        min_indent = indent
      end
    end
  end
  if min_indent == math.huge then min_indent = 0 end

  for i, line in ipairs(lines) do
    lines[i] = line:sub(min_indent + 1)
  end

  return table.concat(lines, "\n")
end

function exec_template(s, vars, max_depth)
    max_depth = max_depth or 1
    local prev = ''
    for _ = 1, max_depth, 1 do
        s = s:gsub("%$%(([%w_]+)%)", vars):gsub("%$([%w_]+)", vars)
        if prev == s then
            break
        end
        prev = s
    end

    return s
end

function file_read(path)
    local f = io.open(path, 'r')
    assert(f, 'failed to open file')
    local data = f:read('*a')
    f:close()
    return data
end

function file_write(path, data, with_header)
    local f = io.open(path, 'w')
    assert(f, 'failed to open file')

    if with_header then
        local now = os.date('%Y-%m-%d %H:%M:%S')
        local header = ('// File automatically generated at %s. DO NOT EDIT.\n\n'):format(now)
        f:write(header)
    end

    f:write(data)
    f:close()
end

function pascal_to_snake(s)
  s = s:gsub("(%l)(%u)", "%1_%2")     -- xY  -> x_Y
  s = s:gsub("(%u+)(%u%l)", "%1_%2")  -- HTTPServer -> HTTP_Server
  return s:lower()
end

function snake_to_pascal(s)
  return (s:gsub("(%a)([%a%d]*)", function(first, rest)
    return first:upper() .. rest
  end):gsub("_", ""))
end
