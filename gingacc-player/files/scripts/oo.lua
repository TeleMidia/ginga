--[[
-- Sistema de OO em Lua.
-- Tanto este sistema quanto as classes criadas por ele utilizam o sistema de
-- modulos do Lua 5.1.
--
-- Este sistema simples de OO não é implementado com uso de metatables, mas com
-- um conceito de hierarquia por cópia que é usado tanto na derivação de
-- classes como na instanciação de objetos e suas propriedades.
--
-- Por usar cópia as classes não são dinâmicas: uma alteração em tempo de
-- execução em um método de uma super-classe não será percebido pelas suas
-- sub-classes, assim como uma alteração em um método de uma classe não terá
-- efeito sobre suas instâncias já criadas.
--
-- Essa aparente fraqueza é na verdade uma das motivações da criação deste
-- sistema como veremos a seguir. 
-- Também por usar cópia a criação das classes e objetos é mais lenta, mas a
-- utilização durante o ciclo de vida é mais rápida, uma vez que os métodos e
-- propriedades do objeto estão em sua própria tabela.
-- Mais importante que isso é que a hierarquia também funciona para as
-- propriedades (em tabela) das classes, caso uma classe possua a mesma
-- propriedade de sua super-classe, os valores da tabela da sub-classe serão
-- completados pelos da super-classe, o mesmo vale na instanciação
-- objeto->classe onde o uso dessa característica é ainda mais frequente.
--
-- O uso de propriedades públicas é incentivado contra o uso de métodos
-- set()/get(), a versatilidade das propriedades é o argumento para isso:
-- Caso seja necessário tomar ações TEMP
-- Caso oo._REQ seja o valor de uma propriedade, esta será requerida no momento
-- da instanciação dos objetos e sua ausência acarretará em erro.
--
-- Criação de classes:
-- <example>
-- module 'C1'
-- prop1 = { a=1, b=2 }
-- prop2 = 1
-- function method1 (self) ... end
-- return oo.class(_M, oo.Object)
-- </example>
-- Cria "C1" herdando de Object, com suas propriedades e métodos.
-- Como pode ser observado no exemplo acima e ao contrário da maioria dos
-- sistemas OO em Lua ou em outra linguagem, a chamada à função oo.class(), que
-- propriamente cria a classe, deve ser feita após a implementação da mesma,
-- isto é, metódos e propriedades devem ser declarados antes da criação da
-- classe.
-- Isso se faz necessário pois é nesse momento que a classe criada copia tudo o
-- que for necessário de sua super-classe, portanto já deve conter seus métodos
-- e propriedades.
--
-- Instanciação de objetos:
-- <example>
-- o1 = C1{ prop1 = true }
-- </example>
-- "o1" terá automaticamente prop2=1, method1(), além de todos os métodos e
-- propriedades de oo.Object e assim por diante.
--
-- Construtor:
--
-- O construtor de uma classe pode ser extendido da seguinte forma:
-- <example>
-- function C1.new (cls, obj)
-- 	obj = C1._SUPER.new(cls, obj)  -- chama o construtor acima
-- 	self = obj                     -- cria um alias para usar aqui
-- 	...
-- 	return obj                     -- retorna o objeto criado
-- end
-- </example>
-- Campos Estáticos:
--
-- A convenção para métodos e propriedades estáticas é colocá-los precedidos de '_', sendo que
-- as propriedades devem também estar em maiúsculas e os métodos NÃO devem usar ':' em sua
-- definição:
-- <example>
-- C1 = {
-- 	_PROP = nil
-- }
-- function C1._method () ... end
-- -- e não: function C1:_method() ... end
-- </example>
-- Os campos estáticos das classes não são passados para suas instâncias (mas são para suas
-- sub-classes).
-- Para usar um campo estático, deve-se chamar explicitamente a classe:
-- <example>
-- o1 = C1()
-- C1._PROP = 10
-- -- e não: o1._PROP = 10
-- </example>
-- Reflexão:
--
-- Todos as classes possuem os campos "_SUPER" e "_NAME" que são respectivamente um ponteiro
-- para a super-classe e o nome da classe.
-- Todos os objetos possuem o campo "class" que é um ponteiro para sua classe.
-- <example>
-- print( o1.class._SUPER._NAME )
-- --> 'Object'
-- </example>
-- O método 'is()' está disponível para perguntar ao objeto ou classe se ela é descendente de
-- alguém:
-- <example>
-- o1.class.is('Object')
-- --> para qualquer objeto sempre vai dar true
-- </example>
--]]

local _G = _G
local string = string

module 'oo'

-- token unico utilizado em complete() para identificar campos em t2 obrigatórios em t1
_REQ = {}

--[[
-- Valores de t1 sao completados com os de t2.
-- required: testa valores _REQ (instanciação de objetos com propriedade obrigatória)
-- Faz a hierarquia de classes e instanciação de objetos.
-- Caso os dois existam, mantem-se v1.
-- Casso o v1 seja '__nil__', mesmo que exista v2, v1 se tornará nil.
-- Caso ambos sejam tabelas, faz-se a recursao.
-- Example:
-- <example>
-- t1 = { v1 = false, v2 = {1,2,3},    v3 = 1 }
-- t2 = { v1 = 50,    v2 = {a = true}, v3 = {-1} }
-- complete(t1, t2, '^_')
-- t1 = { v1 = false, v2 = {1,2,3, a=true}, v3 = 1 }
--
-- -- Herança de Classes:
-- c2 é super de c1
-- c2 = { _cls1 = xxx, prop1 = xxx, prop2 = _REQ, meth1 = f1() }
-- c1 = { prop1 = yyy }
-- complete(c1, c2)
-- -->  { _cls1 = xxx, prop1 = yyy, prop2 = _REQ, meth1 = f1() }
--
-- -- Instância:
-- o1 é instância de c2
-- o1 = {}
-- complete(o1, c1, true)
-- -->  { prop1 = yyy, prop2 = _REQ, meth1 = f1() }
-- --> error: field prop2 is mandatory
-- </example>
--]]
function complete (t1, t2, required)
	_G.assert(_G.type(t1) == 'table')
	_G.assert(_G.type(t2) == 'table')
	for k, v2 in _G.pairs(t2)
	do
if string.sub(k, 1, 1) ~= '_' then  -- continue, campo ignorado
		local v1 = t1[k]
		if v1 == '__nil__' then
			t1[k] = nil
		elseif (_G.type(v1) == 'table') and v1.class then
			-- objeto e' opaco, nao completar

		elseif v2 == _REQ then
			if v1 == nil then
				_G.assert( not required, 'field '..k..' is mandatory' )
				t1[k] = _REQUIRED   -- T1 e' class, passa o _REQ adiante
			end

		elseif (_G.type(v2) == 'table') and (not v2.class) then
			if t1[k] == nil then t1[k] = {} end
			if _G.type(t1[k]) == 'table' then
                complete(t1[k], v2, required)
			end

		elseif v1 == nil then
			t1[k] = v2
		end
	end
end
	return t1
end

--[[
-- Cria uma nova classe retornando-a.
-- cls: tabela classe com suas propriedades e métodos
-- super: super-classe (default: Object)
--]]
function class (cls, super)
       _G.assert( (_G.type(cls)=='table') and cls._NAME, 'missing field _NAME on class' )
       cls._SUPER = super or Object
	   complete(cls, cls._SUPER, false)   -- required so no momento de instanciar objetos
       _G.setmetatable( cls, { __call  = function(self,...) return self.new(self,_G.unpack(arg)) end } )
       return cls
end

--[[
-- Retorna se o objeto ou classe e' (ou herda) de name.
-- cls: classe ou objeto sendo testado
-- name: nome a ser testado
--]]
function is (cls, name)
	if not cls then return false end
	if cls.class then cls = cls.class end
	if cls._NAME == name then return true end
	return is(cls._SUPER, name)
end

Object = {
	  _SUPER = nil          -- setado em class()
	, _NAME  = "oo.Object"  -- recebido com cls, em class()
	, class  = nil          -- setado em new()
	, id     = 'unamed'
	, new    = function(cls, obj)
				obj = obj or {}
				obj.class = cls
				complete(obj, cls, true)
				obj.id = obj.id .. '_' .. cls._NAME .. '_' .. _G.tostring(obj)
				return obj
			end
}

--[[
require 'oo'

A = { rect={x=-1000} }
A = class('A', A)

B = { rect={xx=100} }
B = class('B', B, A)

print('O '..tostring(Object))
print('A '..tostring(A), 'A._SUPER '..tostring(A._SUPER))
print('B '..tostring(B), 'B._SUPER '..tostring(B._SUPER))

b = B()
pr(A)
pr(B)
pr(b)
print('b.class '..tostring(b.class), 'b.class._SUPER '..tostring(b.class._SUPER))
_G.assert(b.class._SUPER == A)
_G.assert(b.class._SUPER._SUPER == Object)
_G.assert(B:is'Object' and b:is'A')

bb = b()
--]]
