--[[ Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <https://www.gnu.org/licenses/>. ]]--

--------------------------------------------------------------------------------
-- DRAWS
--------------------------------------------------------------------------------

local DX, DY = canvas:attrSize()

local DRAWS = {}

function redraw ()
   canvas:attrColor('black')
   canvas:drawRect('fill', 0, 0, DX, DY)
    for _, draw in ipairs(DRAWS) do
        draw:draw()
    end
    canvas:flush()
end

function drawRect (self)
    canvas:attrColor(self.color)
    canvas:drawRect('fill', self.x, self.y, self.dx, self.dy)
end

function drawText (self)
    canvas:attrColor(self.color)
    canvas:attrFont(self.font.face, self.font.height)
    canvas:drawText(self.x, self.y, self.text)
end

function drawImage (self)
    if self.frame then
        local x = self.frame * self.dx
        canvas:compose(self.x, self.y, self.cvs, x,0,self.dx,self.dy)
    else
        canvas:compose(self.x, self.y, self.cvs)
    end
end

function isColliding (A, B, axis)
    if not axis then
        return isColliding(A,B,'x') and isColliding(A,B,'y')
    end
    local Ap1, Ap2 = A[axis], A[axis]+A['d'..axis]
    local Bp1, Bp2 = B[axis], B[axis]+B['d'..axis]
	return ( not(A.killed or B.killed) and
             (
               ((Ap2 >= Bp1) and (Bp2 >= Ap1)) or
	           ((Bp2 >= Ap1) and (Ap2 >= Bp1))
             ))
end

--------------------------------------------------------------------------------
-- ANIMS
--------------------------------------------------------------------------------

local ANIMS = {}
local ANIMS_REV = setmetatable({}, {__mode='kv'})
local posted = false

function createAnim (f)
    local co = coroutine.create(f)
    if stepAnim(co) then
        ANIMS[co] = true
    end
    if not posted then
        event.post('in', {class='user', now=event.uptime()})
        posted = true
    end
    return co
end

function sleep (time)
    while time > 0 do time = time - dt() end
end

function stepAnim (co, dt)
    assert(coroutine.resume(co, dt))
    return coroutine.status(co) ~= 'dead'
end

function stepAnims (evt)
    if not evt.now then return end
    local now = event.uptime()
    local dt = now - evt.now
    evt.now = now
    for co in pairs(ANIMS) do
        if not stepAnim(co, dt) then
            ANIMS[co] = nil
        end
    end
    if next(ANIMS) then
        event.post('in', evt)
    else
        posted = false
    end
    redraw()
end

dt = coroutine.yield

event.register(stepAnims)

--------------------------------------------------------------------------------

function kill (draw)
    for i, d in ipairs(DRAWS) do
        if d == draw then
            table.remove(DRAWS, i)
            break
        end
    end
    if draw.co then
        ANIMS[draw.co] = nil
    end
    draw.killed = true
end

--------------------------------------------------------------------------------
-- FUNDO
--------------------------------------------------------------------------------
do
    local bg = {
        cvs  = canvas:new('background.png'),
        draw = drawImage,
    }
    local dx, dy = bg.cvs:attrSize()
    bg.x = (DX - dx) / 2
    bg.y = (DY - dy) / 2
    DRAWS[#DRAWS+1] = bg
end

--------------------------------------------------------------------------------
-- MUROS
--------------------------------------------------------------------------------
local MUROS
do
    MUROS = {
        { x=0,    y=0,    dx=DX, dy=2,  vert=false },
        { x=DX-2, y=0,    dx=2,  dy=DY, vert=true  },
        { x=0,    y=DY-2, dx=DX, dy=2,  vert=false },
        { x=0,    y=0,    dx=2,  dy=DY, vert=true  },
    }
    for _,muro in ipairs(MUROS) do
        muro.color  = 'white'
        muro.draw   = drawRect
        DRAWS[#DRAWS+1] = muro
    end
end

--------------------------------------------------------------------------------
-- PONTOS
--------------------------------------------------------------------------------
local pontos
do
    pontos = {
        text  = 0,
	    font  = {face='sans',height=25},
        color = 'red',
        draw  = drawText,
        x = 10,
        y = 10,
    }
    function pontos:add (v)
		self.text = self.text + v
	end
    DRAWS[#DRAWS+1] = pontos
end

--------------------------------------------------------------------------------
-- SATELITE
--------------------------------------------------------------------------------
do
    local sat = {
        cvs   = canvas:new('satellite.gif'),
        draw  = drawImage,
        frame = 0,
    }

	local _frames = 6
    local dx, dy = sat.cvs:attrSize()
    sat.dx = dx / _frames
    sat.dy = dy
    sat.x = 100
    sat.y = 100

    DRAWS[#DRAWS+1] = sat

    sat.co = createAnim(
        function ()
            sat.y = 100
            while true do
                sat.x = -sat.dx
                sleep(5000)
                local changeFrame = 25
                while sat.x < DX do
                    sat.x = sat.x + 100*dt()/1000
                    if sat.x > changeFrame then
                        sat.frame = (sat.frame + 1) % _frames
                        changeFrame = changeFrame + 25
                    end
                end
            end
        end)
end

--------------------------------------------------------------------------------
-- NAVE
--------------------------------------------------------------------------------

local nave
local handlerNave
local TIROS = setmetatable({}, {__mode='k'})
do
    local _frames = 4
    nave = {
        nave   = true,
        cvs    = canvas:new('ship.gif'),
        frames = _frames,
        frame  = 1,
        draw   = drawImage,
    }
    local dx, dy = nave.cvs:attrSize()
    dx = dx / _frames
    nave.x  = (DX - dx) / 2
    nave.y  = (DY - dy) / 2
    nave.dx = dx
    nave.dy = dy

    DRAWS[#DRAWS+1] = nave

	function atirar (self)
		local tiro = {
            dx=2, dy=8,
            color = 'wthie',
			draw  = drawRect,
		}
        DRAWS[#DRAWS+1] = tiro
        TIROS[tiro] = tiro
        pontos:add(-1)
        tiro.co = createAnim(
            function()
                tiro.x = nave.x + nave.dx/2
                tiro.y = nave.y
                while tiro.y > 0 do
                    tiro.y = tiro.y - 200*dt()/1000
                end
                kill(tiro)
	        end)
    end

    handlerNave = function (evt)
            if evt.class ~= 'key' then return end
		    if evt.type == 'release' then
			    nave.frame = 1
                redraw()
                return
		    end
		    if evt.type ~= 'press' then return end

            -- movimentos
		    if     evt.key == 'CURSOR_UP'    then
                nave.y = nave.y - 5
			    nave.frame = 1
		    elseif evt.key == 'CURSOR_DOWN'  then
                nave.y = nave.y + 5
			    nave.frame = 0
		    elseif evt.key == 'CURSOR_LEFT'  then
                nave.x = nave.x - 5
			    nave.frame = 2
		    elseif evt.key == 'CURSOR_RIGHT' then
                nave.x = nave.x + 5
			    nave.frame = 3
		    elseif evt.key == 'ENTER' then
                atirar()
		    end
            redraw()
        end
	event.register(handlerNave)
end

--------------------------------------------------------------------------------
-- PEDRAS
--------------------------------------------------------------------------------

local PEDRAS = setmetatable({}, {__mode='k'})
local IDX = #DRAWS + 1

local images = {
    big   = canvas:new('rock_big.gif'),
    small = canvas:new('rock_small.gif'),
}

math.randomseed(os.time())
local _frames = 6

function criarPedra (tamanho, parent, v)
	v = v or 50
	parent = parent or { x=math.random(30, DY-30), y=-50 }

	local pedra = {
        cvs    = images[tamanho],
        frames = _FRAMES,
        frame  = 1,
        draw   = drawImage,
        tamanho   = tamanho,
        x  = parent.x,
        y  = parent.y,
        vX = math.random(-v,v),
        vY = math.random(1,v),
    }
    local dx, dy = pedra.cvs:attrSize()
    pedra.dx = dx / _frames
    pedra.dy = dy
    pedra.v = math.sqrt(pedra.vX^2+pedra.vY^2)

    table.insert(DRAWS, IDX, pedra)
    PEDRAS[pedra] = pedra

    pedra.co = createAnim(
        function ()
            local d = 0
            local v = pedra.v
            while true do
                local dt = dt()/1000
                pedra.x = pedra.x + pedra.vX*dt
                pedra.y = pedra.y + pedra.vY*dt

                d = d + v*dt
                pedra.frame = math.floor(d/10) % _frames
            end
        end)
end

for i=1, 5 do criarPedra('big') end

createAnim(
    function()
        local v = 50
        while true do
            sleep(5000)
            v = v + 5
            criarPedra('big', nil, v)
        end
    end)

--------------------------------------------------------------------------------
-- COLISOES
--------------------------------------------------------------------------------

function destruirNave ()
    if nave.killed then return end
    event.unregister(handlerNave)
    kill(nave)

    local txt = {
        text  = 'Você perdeu!',
        font  = { face='sans',height=20 },
        color = 'red',
        draw  = drawText,
    }
    DRAWS[#DRAWS+1] = txt
    canvas:attrFont(txt.font.face, txt.font.height)
    local dx, dy = canvas:measureText(txt.text)
    txt.dx, txt.dy = dx, dy
    txt.x = (DX - dx) / 2
    txt.y = (DY - dy) / 2
end

createAnim(
    function()
        while dt() do
            collectgarbage()

            for _,muro in ipairs(MUROS) do
                -- muro vs nave
                if isColliding(muro, nave) then
                    destruirNave()
                end
            end

            for pedra in pairs(PEDRAS)
            do
                -- pedra vs nave
                if isColliding(pedra, nave) then
                    destruirNave()
                end

                -- pedra vs muro
                for _,muro in ipairs(MUROS) do
                    if isColliding(pedra, muro) then
			            if muro.vert then
                            if pedra.vX > 0 then
                                pedra.x = muro.x-muro.dx-pedra.dx-1
                            else
                                pedra.x = muro.x+muro.dx+1
                            end
				            pedra.vX = -pedra.vX
			            elseif (pedra.vY<0) or (pedra.y>5) then  -- evita colisao inicial no topo
                            if pedra.vY > 0 then
                                pedra.y = muro.y-muro.dy-pedra.dy-1
                            else
                                pedra.y = muro.y+muro.dy+1
                            end
				            pedra.vY = -pedra.vY
			            end
                        break
                    end
                end

                -- pedra vs tiro
                for tiro in pairs(TIROS) do
                    if isColliding(pedra, tiro) then
			            if pedra.tamanho == 'big' then
                            pontos:add(10)
				            criarPedra('small', pedra)
				            criarPedra('small', pedra)
                        else
                            pontos:add(15)
			            end
			            kill(tiro)
                        kill(pedra)
                        break
                    end
                end
            end
        end
    end)
