ncl = {
  'context',
  'ncl',
  -- list of ports
  {'m2@lambda'},
  -- list of children
  {
    {'media', 'm1', {src = 'samples/clock.ogv'}, {a1 = {'3s'}}},
    {'media', 'm2', {src = 'samples/gnu.png'}}
  },
  -- list of links
  {
    -- link #1
    {
      -- list of conditions
      {
        {'start', 'm1@a1', {true}}
      },
      -- list of actions
      {
        {'stop', 'm1@lambda'},
        {'start', 'm2@lambda', nil, {delay = '0s', duration = '0s'}}
      }
    },
    -- link #2
    {
      -- list of conditions
      {
        {'stop', 'm2<BLUE>', {true}}
      },
      -- list of actions
      {
        {'stop', 'm2@lambda'},
        {'start', 'm1@lambda'}
      }
    }
  }
}
return ncl
