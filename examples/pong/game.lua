--[[ Copyright (C) 2013-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of NCLua.

NCLua is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

NCLua is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with NCLua.  If not, see <https://www.gnu.org/licenses/>.  ]]--

local canvas = canvas
local dir = dir
local event = event
local w, h = canvas:attrSize ()
local player_size = h/8
local player_pos = h/2 - player_size/2
local ball_pos_x = w/2
local ball_pos_y = h/2
local ball_spd_x = 0
local ball_spd_y = 0
local ball_size = 8
local joystick = 0
local highscore = 0
local score = 0
_ENV = nil

local function clamp(value, min, max)
    if value < min then
        return min
    elseif value > max then
        return max
    else
        return value
    end
end

local function reset()
    highscore = clamp(highscore, score, highscore)
    score = 0
    player_pos = h/2 - player_size/2
    ball_pos_x = w/2
    ball_pos_y = h/2
    ball_spd_x = 5
    ball_spd_y = 1
end

local function draw ()
    canvas:attrColor (0, 0, 0, 0)
    canvas:clear ()
    canvas:attrColor('white')
    canvas:drawRect('fill', 4, player_pos, 8, player_size)
    canvas:drawRect('fill', ball_pos_x, ball_pos_y, ball_size, ball_size)
    canvas:attrFont('sans', 32)
    canvas:drawText(w/4, 16, score)
    canvas:drawText((w/4 * 3), 16, highscore)
    canvas:flush ()
end

local function move()
    ball_pos_x = ball_pos_x + ball_spd_x
    ball_pos_y = ball_pos_y + ball_spd_y
    player_pos = clamp(player_pos + (joystick * 7), 0, h - player_size)
end

local function cols()
    if ball_pos_x >= (w - ball_size) then
        ball_spd_x = ball_spd_x * -1
    end
    if ball_pos_y <= 0 or ball_pos_y >= (h - ball_size) then
        ball_spd_y = ball_spd_y * -1
    end
    if ball_pos_x <= 0 then 
        if clamp(ball_pos_y, player_pos, player_pos + player_size) == ball_pos_y then
            ball_spd_y = clamp(ball_spd_y + (player_pos % 10) - 5, -10, 10)
            ball_spd_x = ball_spd_x * -1.05
            score = score + 1
        else
            reset()
        end
    end
end

local function input(evt)
    if evt.class == 'key' and evt.type == 'release' and evt.key == 'CURSOR_UP' then
        joystick = -1
    elseif evt.class == 'key' and evt.type == 'release' and evt.key == 'CURSOR_DOWN' then
        joystick = 1
    elseif evt.class == 'key' then
        joystick = 0
    end
end

local function event_loop(evt)
    input(evt)
end

local function fixed_loop()
    move()
    cols()
    draw()
    event.timer(0, fixed_loop)
end

event.register(event_loop)
event.timer(0, fixed_loop)
reset()
