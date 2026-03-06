-- At the top

slider_value = slider_value or 10.0

ft = 0

function update()
    -- if is_mouse_button_pressed(0) then -- left click (you'll need to expose this too)
    local mx, my = get_mouse_position()
end

-- Global so it persists

function draw()
    draw_text("Civil Modelling – click to test", 20, 40, 20)

    local mx, my = get_mouse_position()
    draw_circle(mx, my, slider_value, { 255, 255, 0, 255 })

    -- Example geometry
    draw_line(100, 100, 300, 400, 0, 120, 220)
    draw_line(300, 400, 600, 150, 80, 180, 255)
    -- In draw()
    slider_value = gui_slider(20, 120, 200, 30, slider_value, 1, 50, "Size", "px")
    -- draw_circle(800, 400, slider_value, { 200, 100, 50, 180 })
    draw_text("Circle size: " .. math.floor(slider_value), 20, 160, 20, GRAY)

    ft = ft + 1
end
