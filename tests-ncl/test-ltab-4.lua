ncl = {
  'context',
  'ncl',
  -- list of ports
  {'time@lambda', 'm1@lambda'},
  -- list of children
  {
    {'media', 'time', nil},
    {
      'media',
      'm1',
      {
        src = 'samples/gnu.png',
        explicitDur = '2s',
        transIn = {
          type = 'barWipe',
          dur = '1s'
        },
        transOut = {
          type = 'barWipe',
          dur = '1s'
        }
      }
    }
  },
  -- list of links
  {nil}
}
return ncl
