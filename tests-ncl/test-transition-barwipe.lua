local ncl = {'context', 'ncl',

   -- Ports.
   {'m1@lambda', 'm2@lambda', 'm3@lambda', 'm4@lambda'},

   -- Children
   {
      {'media', 'm1',
       {top='0',
        left='0',
        width='50%',
        height='50%',
        src='samples/vector.svg',
        transIn={type='barWipe',
                 dur='1s',},
        transOut={type='barWipe',
                  dur='1s'}},
      },
      {'media', 'm2',
       {top='0',
        left='50%',
        width='50%',
        height='50%',
        src='samples/vector.svg',
        transIn={type='barWipe',
                 dur='1s',},
        transOut={type='barWipe',
                  dur='1s'}},
      },
      {'media', 'm3',
       {top='50%',
        left='0',
        width='50%',
        height='50%',
        src='samples/vector.svg',
        transIn={type='barWipe',
                 dur='1s',},
        transOut={type='barWipe',
                  dur='1s'}},
      },
      {'media', 'm4',
       {top='50%',
        left='50%',
        width='50%',
        height='50%',
        src='samples/vector.svg',
        transIn={type='barWipe',
                 dur='1s',},
        transOut={type='barWipe',
                  dur='1s'}},
      },
   },
}

-- Finally, return the constructed table to the caller.  This means that
-- whomever calls this script (e.g., the LuaParser) will receive the
-- constructed table.
return ncl
