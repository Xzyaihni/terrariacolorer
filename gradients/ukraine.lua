function blend_colors(colors, value, fuzz)
	local c_colors = {colors[1]/255, colors[2]/255}

	local mid_distance = math.abs(0.5-value)
	local edge_distance = value
	if(edge_distance>0.5) then
		edge_distance = 1-edge_distance
	end

	local min_distance = (0.25-math.min(mid_distance, edge_distance)) * fuzz*8

	local c_color
	if(value<0.5) then
		c_color = c_colors[1]
	else
		c_color = c_colors[2]
	end


	local return_color = c_color * (1+min_distance)

	if(return_color<0) then
		return 0
	end

	if(return_color>1) then
		return 1
	end

	return return_color
end

--the function has to return a table with {r, g, b} values in range 0 to 1
--it also has to be named color_func and take 2 arguments
--both fuzz and value are between 0 to 1
function color_func(value, fuzz)
	local r = blend_colors({23, 255}, value, fuzz)
	local g = blend_colors({121, 255}, value, fuzz)
	local b = blend_colors({240, 70}, value, fuzz)

	return {r=r, g=g, b=b}
end
