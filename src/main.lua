-- At the top
local points = {}


slider_value = slider_value or 10.0

ft = 0

function update()
    -- if is_mouse_button_pressed(0) then -- left click (you'll need to expose this too)
        local mx, my = get_mouse_position()
        -- local pt = add_point(mx, my)
        table.insert(points, { x = mx, y = my })
    -- end

    -- if gui_button(20, 200, 140, 30, "Clear Points") then
        
    if ft % 1000 == 0 then
        points = {}
    end
end

-- Global so it persists

function draw()
    draw_text("Civil Modelling – click to test", 20, 40, 20, DARKGRAY)

    local mx, my = get_mouse_position()
    draw_circle(mx, my, 12, { 255, 255, 0, 255 })

    -- Example geometry
    draw_line(100, 100, 300, 400, 0, 120, 220)
    draw_line(300, 400, 600, 150, 80, 180, 255)
    -- In draw()
    slider_value = gui_slider(20, 120, 200, 30, slider_value, 1, 50, "Size", "px")
    draw_circle(800, 400, slider_value, { 200, 100, 50, 180 })
    draw_text("Circle size: " .. math.floor(slider_value), 20, 160, 20, GRAY)
    draw_text("Num circles: " .. ft, 20, 300, 20, GRAY)

    ft = ft + 1

    for _, p in pairs(points) do
        draw_circle(p.x, p.y, 10, { 255, 0, 255 })
    end
end
