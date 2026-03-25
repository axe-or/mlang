require 'prelude'

function main()
    local array_types = {'int', 'u8'}

    local gen = ''
    for _, v in ipairs(array_types) do
        gen = gen .. gen_dynamic_array(v)
    end

    print(gen)
    file_write('dynamic_array.h', gen, true)
end

local dynamic_array_templ = file_read('dynamic_array.txt')
function gen_dynamic_array(inner_type, array_type, prefix)
    array_type = array_type or ('%sArray'):format(snake_to_pascal(inner_type))
    prefix = prefix or pascal_to_snake(array_type)

    return exec_template(dynamic_array_templ, {
        type = inner_type,
        arrayType = array_type,
        prefix = prefix,
    }, 2)
end


main()
