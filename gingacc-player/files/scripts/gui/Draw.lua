--[[
-- This is an abstract class and represents the idea of what is a draw (short for drawable).
-- It's not just an interface as it also defines some properties and implements many methods.
-- All the visibility, positioning and collision detection support is implemented in this class.
-- The only abstract method is <ref>draw</ref> and all derived sub-classes must implement it.
-- The <ref>gui.lua#draws</ref> maintains a collection of these objects demanding that any other draw should derive from this class.
--
-- <section> <title>Positioning System</title>
-- The area avaiable for a Draw is always relative to its parent, this means that its (0,0) is in its parent Draw (0,0) and not in main window's (0,0).
-- It's possible to set positioning for a Draw in two places: when creating it through the constructor or using <ref>set</ref> method.
-- Absolute and relative positionings are avaiable (both relative to parent).
--
-- <b>Absolute positioning:</b>
-- <code>p1</code>: left/top most point
-- <code>pc</code>: center/middle point
-- <code>p2</code>: right/bottom most point
-- <code>d</code>:  dimension
--
-- Example:
-- <example>draw = TextDraw{ x={pc=200}, y={p2=100} , ... }
-- </example>
--
-- <img border="1" src="pos.png" align="left"/>
-- <img border="1" src="rel.png" align="right"/>
--
-- <br clear="all"/>
-- <b>Relative positioning:</b>
-- Here the values varies from 0 to 1000 ranging proporcionally from 0% to 100% to parent's dimensions.
-- <code>pp1</code>: left/top most point of draw (p1) is positioned in this percentage relative to parent dimension.
-- <code>ppc</code>: center/middle point
-- <code>pp2</code>: right/bottom most point
-- <code>pk</code>:  constant adjustment
-- <code>dd</code>:  dimension of draw is set to this percentage of parent's dimension
--
-- The following examples corresponds to the image on the right:
--
-- <example>-- Example 1:
-- draw:set{ x={pp1=0}, y={pp1=0} }
-- -- Example 2:
-- draw:set{ x={ppc=500}, y={ppc=500} }
-- -- Example 3:
-- draw:set{ x={ppc=1000}, y={pp1=500} }
-- -- Example 4:
-- -- also sets to half the parent's width
-- draw:set{ x={pp1=0, dd=500}, y={pp2=1000} }
-- -- Example 5:
-- -- also adjusts 10 pixels left
-- draw:set{ x={pp2=1000, pk=-10}, y={pp1=1000} }
-- </example>
--
-- </section>
--]]

local _G = _G
local oo = require 'oo'

module (...)
local gui   = _G.require(_PACKAGE..'gui')
local event = _G.require(_PACKAGE..'event')

--[[
-- Maintains <b>self</b> positioning.
-- See the Postioning section for more information.
--]]
x = {p1,pc,p2,pp1,ppc,pp2, d,dd, P={v,t}, D={v,t}}
-- The same as in <ref>x</ref>.
y = {p1,pc,p2,pp1,ppc,pp2,pk=0, d,dd,dk=0, P={v,t}, D={v,t}}

--[[
-- Every draw has as parent and all positioning are relative to it.
-- The default parent for every draw is the main window.
--]]
parent = nil -- TEMP was: gui.parent

--[[
-- Current visibility status for <b>self</b>.
-- Not visibles draws are not subject for drawing and collision detection.
--]]
visible = true

--[[
-- Current collision status for <b>self</b>.
-- If <code>false</code> then <b>self</b> isn't subject for collision detection.
-- If is a <code>function</code> value it is not only checked for collisions but also this function is called when collided with other draw.
-- The signature of the function is:
-- <example>
-- function (self, draw2) end
-- </example>
-- Where draw2 is the collided draw with <b>self</b>.
-- Any other non-nil value assumes <b>self</b> as subject for collision detection and takes no action.
--]]
collide = false

--[[
-- Method responsible for drawing it on screen.
-- Here is an abstract method required to be implemented for sub-classes.
-- This method is called for every draw in <ref>gui.lua#draws</ref> whenever a call to <ref>set</ref> generates 'draw' event <ref>event.lua#post</ref>.
-- It's signature is:
-- <example>
-- function draw (self) end
-- </example>
--]]
draw = oo._REQ

local _INT = {}

local _SHIFT = {
	left = 0, center = -1, right  = -2,
	top  = 0, middle = -1, bottom = -2,
}

--[[
-- Returns pc, p1 and p2, given:
-- p: position
-- dp: parent's dimension
-- l: type of alignment ('left'/'top', 'center'/'middle', 'right'/'bottom)
--]]
function _shift (p, dp, l)
	l = l or 'center' -- ou middle, tanto faz
	local p1 = p + (_SHIFT[l] * dp / 2)
	local p2 = p1 + dp
	local pc = p1 + dp/2
	return pc, p1, p2
end

--[[
-- Checks if <b>self</b> intersects with Draw B.
-- B: Draw being checked against <b>self</b>
-- returns true/false
--]]
function getIntersection (self, B)--, int)
	local Ax1, Ax2, Ay1, Ay2 = self.x.p1, self.x.p2, self.y.p1, self.y.p2
	local Bx1, Bx2, By1, By2 = B.x.p1, B.x.p2, B.y.p1, B.y.p2

	if (not self) or (not B) or
		Ax1 > Bx2 or
		Ax2 < Bx1 or
		Ay1 > By2 or
		Ay2 < By1 then
		return false
	end

	-- so esta interessado se colidiu
	if not int then return true end

	--[[
	-- retangulo de intersecao
	int.x1 = math.max( Ax1, Bx1 )
	int.x2 = math.min( Ax2, Bx2 )
	int.dx = int.x2 - int.x1
	int.y1 = math.max( Ay1, By1 )
	int.y2 = math.min( Ay2, By2 )
	int.dy = int.y2 - int.y1
	int.offX = int.x1 - Ax1
	int.offY = int.y1 - Ay1

	-- intersecao completa: se int == (self ou B)
	--int.complete = (int.x1 == Ax1) and (int.x2 == Ax2) and (int.y1 == A1.y1) and (int.y2 == A1.y2)
	--int.complete = int.complete or (int.x1 == B.x1) and (int.x2 == B.x2) and (int.y1 == B.y1) and (int.y2 == B.y2)

	return int
	]]--
end

--[[
-- <hide/>
-- Default constructor for all Draws.
-- Calls <ref>set</ref> automatically.
-- set: (false) used for classes requiring initialization prior to calling <ref>set</ref>.
-- In this case, <b>self</b> is required to call <ref>set</ref> later in its derived constructor.
--]]
function new (cls, obj, set)
	set = set or (set == nil)
	local self = _SUPER.new(cls, obj)
	self.parent = self.parent or gui.parent
	if set then
		self:set(self)
	end
	return self
end

local _R = {x=nil,y=nil,xp=true}
--[[
-- Moves <b>self</b> <code>dx</code>, <code>dy</code> pixels.
--]]
function move (self, dx, dy)
	_R.x, _R.y = dx+self.x.pc, dy+self.y.pc
	self:set(_R)
end

local _NIL = {}
local _D = {
	d = {pct=false}, dd = {pct=true}
}
local _P = {
	 p1 = {pct=false, align='top'},  pc = {pct=false, align='center'},  p2 = {pct=false, align='bottom'},
	pp1 = {pct=true,  align='top'}, ppc = {pct=true,  align='center'}, pp2 = {pct=true,  align='bottom'},
}

--[[
-- Controls <b>self</b> state.
-- All the positioning and visibility is set through this method.
-- Sub-classes requiring to set other properties should do this extending this method (see <ref>oo.lua#TEMP</ref>).
-- Generates a 'draw' event calling <ref>event.lua#post</ref>.
-- <code>T</code> can be:
-- <ul>
-- <li> nil: TEMP </li>
-- <li> false: sets <b>self</b> visibility to false. </li>
-- <li> table: expects the format as in Positioning System section. </li>
-- </ul>
--]]
function set (self, T)
	event.post('draw', self)

	if T == false then
		self.visible = false
		return
	end

	T = T or self  -- nil should continue below
	self.visible = true
	if T == true then
		return
	end

	-- assume mudanca de posicao centralizada (xp=express)
	-- considera apenas t.x/t.y
	-- TEMP: nao testei depois de incluir parent.p1
	if T.xp
	then
		local X, Y = self.x, self.y
		local dx2, dy2 = X.d/2, Y.d/2
		local x, y = T.x, T.y
		if x then
			x = x + self.parent.x.p1
			X.p1, X.pc, X.p2 = x-dx2, x, x+dx2
			X.P.v, X.P.t = x, _P.pc
		end
		if y then
			y = y + self.parent.y.p1
			Y.p1, Y.pc, Y.p2 = y-dy2, y, y+dy2
			Y.P.v, Y.P.t = y, _P.pc
		end

	-- SET() PADRAO
	else
		-- Em x e y:
		for _, axis in _G.ipairs(gui._AXIS)
		do
			local t, s = T[axis] or _NIL, self[axis]

			s.pk = t.pk or s.pk
			s.dk = t.dk or s.dk
			local parentD = self.parent[axis].d

			-- POSICAO
			local P = s.P
			if not P.v then
				P.v, P.t = -10000, _P.p2  -- default: fora da tela
			end
			for p, _t in _G.pairs(_P) do  -- itera entre p1,pp1,pc,etc
				if t[p] then              -- pega o primeiro que encontrar
					P.v = t[p]
					P.t = _t
					break
				end
			end
			local pv = _G.assert(P.v, axis..': '..self.id)
			if P.t.pct then
				if     pv >= 0 then pv = (parentD or 0) * pv / 1000
				elseif pv <  0 then pv = (parentD or 0) + pv end
			end
			if s.pk then pv = pv + s.pk end

			-- TAMANHO
			local D = s.D
			for d, _t in _G.pairs(_D) do
				if t[d] then
					D.v = t[d]
					D.t = _t
					break
				end
			end
			local dv = _G.assert(D.v, 'missing "'..axis..'" dimension for '..self.id)
			if D.t.pct then
				if     dv >= 0 then dv = (parentD or 0) * dv / 1000
				elseif dv <  0 then dv = (parentD or 0) + dv end
			end
			if s.dk then dv = dv + s.dk end

			-- ALTERA
			s.d, s.pc, s.p1, s.p2 = dv, _shift(pv, dv, P.t.align)
		end
	end
end

return oo.class(_M)
