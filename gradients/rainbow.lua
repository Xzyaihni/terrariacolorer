function value_from(edge, value, fuzz)
	local start_distance = value-edge
	if(start_distance<0) then
		start_distance = start_distance + 1
	end

	local closest_distance = start_distance

	if(closest_distance>0.5) then
		closest_distance = 1-closest_distance
	end

	local return_value = closest_distance*(1+fuzz*4)

	if(return_value<0) then
		return 0
	end

	if(return_value>1) then
		return 1
	end

	return return_value
end

--the function has to return a table with {r, g, b} values in range 0 to 1
--it also has to be named color_func and take 2 arguments
--both fuzz and value are between 0 to 1
function color_func(value, fuzz)
	local r = value_from(0, value, fuzz)
	local g = value_from(0.33, value, fuzz)
	local b = value_from(0.66, value, fuzz)

	return {r=r, g=g, b=b}
end
