-- $Id: hrace.lua,v 1.1 2009/02/09 12:45:22 root Exp $

-- hrace.lua
-- 06feb2009, gflima@telemidia.puc-rio.br
--
-- A simple horse race game.



-- Compose an animation object into canvas.

local function draw_anim (self)
   -- Select current frame.
   local ini = math.floor (self.dx / self.nframes)
   self.img:attrCrop ((ini * self.frame), 0, ini, self.dy)
   canvas:compose (self.x, self.y, self.img)
end



-- Compose a text object into canvas.

local function draw_text (self)
   canvas:attrColor(self.color)
   canvas:attrFont(self.font.face, self.font.height)
   canvas:drawText(self.x, self.y, self.text)
end



-- Globals.

clock = {
   -- Text.
   text='0.0', font={face='vera', height=25}, color='yellow', x=300, y=210,
   draw=draw_text
}

horses = {
   -- 1 RED.
   { name='red',
     anim={ img=canvas:new ('media/anim_red.png'),
            nframes=5, frame=0, x=nil, y=30, dx=nil, dy=nil,
            draw=draw_anim
         },
     track={ start=90,  -- Start pixel.
     _end=495,          -- End pixel.
     completed=0 }      -- % completed.
  },

   -- 2 GREEN.
   { name='green',
     anim={ img=canvas:new ('media/anim_green.png'),
            nframes=5, frame=0, x=nil, y=49, dx=nil, dy=nil,
            draw=draw_anim
         },
     -- track={ start=65, _end=500, completed=0 }
     track={ start=83, _end=500, completed=0 }
  },

   -- 3 YELLOW.
   { name='yellow',
     anim={ img=canvas:new ('media/anim_yellow.png'),
            nframes=5, frame=0, x=nil, y=77, dx=nil, dy=nil,
            draw=draw_anim
         },
     -- track={ start=48, _end=513, completed=0 }
     track={ start=68, _end=513, completed=0 }
  },

   -- 4 BLUE.
   { name='blue',
     anim={ img=canvas:new ('media/anim_blue.png'),
            nframes=5, frame=0, x=nil, y=116, dx=nil, dy=nil,
            draw=draw_anim
         },
     -- track={ start=23, _end=522, completed=0 }
     track={ start=51, _end=522, completed=0 }
  }

}

-- Initialize horses.
do
   for i=1, #horses do
      -- Set image dimensions.
      local dx, dy = horses[i].anim.img:attrSize()
      horses[i].anim.dx = dx
      horses[i].anim.dy = dy

      -- Set initial horiziontal position.
      horses[i].x = horses[i].track.start
   end
end

-- Drawable objects.  Former objects are composed first.
PICTURES = { clock }

-- Append horses to PICTURE.
do
   n = #PICTURES
   for i=n + 1, #horses + n do
      PICTURES[i] = horses[i - n].anim
   end
end



-- Redraw all pictures.

local function redraw ()

   -- Fill canvas with an transparent color.
   canvas:attrColor (0, 0, 0, 0)
   canvas:clear ()

   for i=1, #PICTURES do
      PICTURES[i]:draw()
   end
   canvas:flush ()
end



-- Update horses horizontal position (x) according to DT (time difference).
-- If some horse wins (reaches its track border) returns its name,
-- otherwise returns nil.

local function move_horses (dt)
   for i=1, #horses do
      local h = horses[i]

      -- Update horse animation frame for each completed unit.
      h.anim.frame = math.floor (h.track.completed) % h.anim.nframes

      -- Update % completed.
      h.track.completed = h.track.completed + dt * math.random (-1, 7)/200

      -- Increment x pos based on % completed.
      local tracksz = h.track._end - h.track.start
      h.anim.x = h.track.start + (tracksz * h.track.completed)/100

      -- Check for winner.
      if h.track.completed >= 100 then
         return h.name
      end
   end
   return nil   -- Nobody won
end



-- Event handler.

local function handler (evt)

   -- Initialize environment.
   if evt.class == 'ncl' and evt.type == 'presentation'
      and evt.action == 'start' then
      -- Initialize PRNG.
      math.randomseed(os.time())

      event.post ('in', { class='user', time=event.uptime () })
   end

   -- Game loop.
   if evt.class == 'user' and evt.time ~= nil then
      local now = event.uptime()

      -- Move clock.
      clock.text = string.format ('%.2f', now/1000)

      -- Move horses.
      local winner = move_horses (now - evt.time)

      if winner == nil then
         -- Race still in progress.
         redraw()
         event.post ('in', { class='user', time=now })
      else
         -- We have a winner!
         print (winner, 'win!')
         event.post ('out', { class='ncl', type='presentation',
                              label=winner, action='start' })
      end
   end
end



event.register (handler)

-- End: hrace.lua
