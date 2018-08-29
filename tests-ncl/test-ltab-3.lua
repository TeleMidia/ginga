ncl = {
  'context',
  'ncl',
  -- list of properties
  {},
  -- list of ports
  {'time@lambda', 's1@lambda'},
  -- list of children
  {
    {'media', 'settings',
      {type = 'application/x-ginga-settings', var = 'm1'}
    },
    {'media', 'time', nil, {{'a1', '3s'}}},
    {
      'switch',
      's1',
      -- list of components
      {
        {'media', 'm1', {src = 'samples/clock.ogv'}},
        {'media', 'm2', {src = 'samples/gnu.png'}}
      },
      -- list of rules
      {
        {'m1', {'$settings.var', '==', 'm1'}},
        {'m2', {'$settings.var', '==', 'm2'}}
      }
    }
  },
  -- list of links
  {
    -- link #1
    {
      -- list of conditions
      {
        {'start', 'time@a1', {true}}
      },
      -- list of actions
      {
        {'stop', 's1@lambda'},
        {'set', 'settings.var', 'm2'},
        {'start', 's1@lambda'}
      }
    }
  }
}
return ncl
