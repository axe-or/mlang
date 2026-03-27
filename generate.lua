require 'prelude'

local array_types = {'int', 'u8', 'String'}
local array_includes = {}

function main()
	generate_dynamic_arrays('dynamic_array.h', array_types)
end

function generate_dynamic_arrays(out, types, includes)
	local gen = ''
	includes = includes or {}

	for _, inc in ipairs(includes) do
		gen = gen .. ('#include "%s"\n'):format(inc)
	end

	for _, v in ipairs(array_types) do
		gen = gen .. generate_dynamic_array(v)
	end

	file_write(out, gen, true)
	print('GEN ', out)
end

local dynamic_array_templ = file_read('dynamic_array.txt')

function generate_dynamic_array(inner_type, array_type, prefix)
    array_type = array_type or ('%sArray'):format(snake_to_pascal(inner_type))
    prefix = prefix or pascal_to_snake(array_type)

    return exec_template(dynamic_array_templ, {
        type = inner_type,
        arrayType = array_type,
        prefix = prefix,
    }, 2)
end

-- Entrypoint
local ok, error = pcall(main)
if not ok then
	print('error: ' .. error)
	os.exit(1)
end
