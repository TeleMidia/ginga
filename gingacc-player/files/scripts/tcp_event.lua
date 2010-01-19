local socket = require 'socket'
local _G, assert, ipairs, print
    = _G, assert, ipairs, print

local str_len = string.len
local t_remove = table.remove

module(...)

local T = {}

local function f_disconnect (conn)
    assert(conn)
    conn:close()
    local t = T[conn]
    T[conn] = nil
    for i, conn in ipairs(T) do
        if conn == conn then
            t_remove(T, i)
            break
        end
    end
    _G.event.post('in', {
        class = 'tcp',
        type  = 'disconnect',
        connection = conn,
    })
end

local function f_out (evt)
    assert(evt.class == 'tcp')

    if evt.type == 'connect' then
        local conn = socket.tcp()
        T[#T+1] = conn
        T[conn] = {
            host = evt.host,
            port = evt.port,
        }
        conn:settimeout(0)
        local obj = conn:connect(evt.host, evt.port)
--print('out conn', evt.host, evt.port, obj)
--print('SIZE',#T)
        assert(obj == nil)
    --local sendt, recvt =  socket.select(T, T)
--error'oi'

    elseif evt.type == 'data' then
--print('out data', evt.value)
--print('SIZE',#T)
        assert(evt.connection)
        local tot = evt.connection:send(evt.value)
        assert(tot == str_len(evt.value))

    elseif evt.type == 'disconnect' then
        f_disconnect(evt.connection)
    end
end

-- TODO: tratar disconnect vindo do servidor
local function f_in ()
--print('f_in() --- size',#T)
    local recvt, sendt =  socket.select(T, T, 0)

    for _, conn in ipairs(recvt) do
        local t = T[conn]
        if t and t.connection then
            local ret, err, partial = conn:receive('*a')
            ret = assert(ret or partial)
--print('RECVT',conn, ret, err, partial)
            if str_len(ret) > 0 then
                _G.event.post('in', {
                    class      = 'tcp',
                    type       = 'data',
                    value      = ret,
                    connection = conn,
                })
            end
            if err == 'closed' then
                f_disconnect(conn)
            end
--print('RECV')
        end
    end

    for _, conn in ipairs(sendt) do
--print('sendt',_)
        local t = T[conn]
        if t and (not t.connection) then
            t.connection = conn
            local ret, err = conn:connect(t.host, t.port)
--print('SENDT',conn, t.host, t.port, ret, err)
            --assert( (ret==nil) and (err=='already connected') )
            _G.event.post('in', {
                class = 'tcp',
                type  = 'connect',
                host  = t.host,
                port  = t.port,
                connection = conn,
            })
        end
    end

--print('SIZE', #T)
    return #T
end

return { f_in, f_out }
