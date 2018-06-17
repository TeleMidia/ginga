ncl = {
  'context',
  'ncl',
  -- list of ports
  {'m1@lambda'},
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
        {'start', 'm1@a1', {'or', {'1', '==', '1'}, {true}}},
        {'start', 'm1@a1', {'and', {'1', '==', '1'}, {true}}},
        {'start', 'm1@a1', {'and', {'1', '>=', '1'}, {true}}},
        {'start', 'm1@a1', {'and', {'1', '<=', '1'}, {true}}},
        {'start', 'm1@a1', {'and', {'2', '>=', '1'}, {true}}},
        {'start', 'm1@a1', {'and', {'2', '>', '1'}, {true}}},
        {'start', 'm1@a1', {'and', {'1', '<', '2'}, {true}}},
        {'start', 'm1@a1', {'and', {'2', '!=', '1'}, {true}}}
      },
      -- list of actions
      {
        {'stop', 'm1@lambda'},
        {'start', 'm2@lambda'}
      }
    }
  }
}
return ncl
