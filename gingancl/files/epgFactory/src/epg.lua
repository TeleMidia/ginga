local DIR = '/usr/local/lib/ginga/epgfactory/src/'

local epg = {
	events = {},
	doc = dofile(DIR..'epgTable.lua'),
	baseId = 'epgPrivateBase',
	documentId = 'epgFactoredDocument',

	addEvent = function (self, event)
		self.events[#self.events+1] = event
	end,
	
	removeEvents = function (self)
		self.events = {}
		self.doc = dofile(DIR..'epgTable.lua')
	end,
	
	startDocument = function(self)
		event.post('out', {
			class='ncl',
			type='ncledit',
			command='0x7,' .. self.documentId
		})
	end,
	stopDocument = function(self)
		event.post('out', {
			class='ncl',
			type='ncledit',
			command='0x8,' .. self.documentId 
		})
	end,
	generate = function(self)
	
		print( 'generate' )
		print( self.doc or 'nil')
		
		return '<?xml version="1.0" encoding="UTF-8"?>\n' .. self:visitNode(self.doc)
	end,
	visitNode = function (self, node)
		if not node or not node.tagName then return '' end
		local s = '<' .. node.tagName
		for k, v in pairs(node) do
			if type(v)=='string' and k~='tagName' then
				s = s .. ' ' .. k .. '="' .. v .. '"'
			end
		end
		s = s .. '>'
		for k,v in ipairs(node) do
			if type(v)=='table' then
				s = s .. '\n' .. self:visitNode(v)
			end
		end
		return s .. '\n</' .. node.tagName .. '>'
	end,
	visitDocument = function (self, node, ids)
		local tag = '<?xml version="1.0" encoding="UTF-8"?>\n' .. self:visitNode(node)
		local tagName = node.tagName or ''
		ids = (ids==nil and {regionBase='', region={'null'}, parent={''}, perspective={''}}) or ids

		if tagName=='regionBase' then ids.regionBase = node.id or '' end

		
		local codes = {
			openBase = '0x0',
			activateBase = '0x1',
			deactivateBase = '0x2',
			saveBase = '0x3',
			closeBase = '0x4',
			addDocument = '0x5',
			removeDocument = '0x6',
			startDocument = '0x7',
			stopDocument = '0x8',
			pauseDocument = '0x9',
			resumeDocument = '0xA',
			addRegion = '0xB',
			removeRegion = '0xC',
			addRegionBase = '0xD',
			removeRegionBase = '0xE',
			addRule = '0xF',
			removeRule = '0x10',
			addRuleBase = '0x11',
			removeRuleBase = '0x12',
			addConnector = '0x13',
			removeConnector = '0x14',
			addConnectorBase = '0x15',
			removeConnectorBase = '0x16',
			addDescriptor = '0x17',
			removeDescriptor = '0x18',
			addDescriptorSwitch = '0x19',
			removeDescriptorSwitch = '0x1A',
			addDescriptorBase = '0x1B',
			removeDescriptorBase = '0x1C',
			addTransition = '0x1D',
			removeTransition = '0x1E',
			addTransitionBase = '0x1F',
			removeTransitionBase = '0x20',
			addImportBase = '0x21',
			removeImportBase = '0x22',
			addImportedDocumentBase = '0x23',
			removeImportedDocumentBase = '0x24',
			addImportNcl = '0x25',
			removeImportNcl = '0x26',
			addNode = '0x27',
			removeNode = '0x28',
			addInterface = '0x29',
			removeInterface = '0x2A',
			addLink = '0x2B',
			removeLink = '0x2C',
			setPropertyValue = '0x2D'
		}

		local evt
		evt = {
			class='ncl',
			type='ncledit',
			command=''
		}
--[[
		if tagName=='regionBase' then
			evt.command = codes.addRegionBase .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='region' then
			evt.command = codes.addRegion .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. ids.regionBase .. ',' .. ids.region[#ids.region] .. ',' .. tag
			ids.region[#ids.region+1] = node.id or ''
		elseif tagName=='rule' then
			evt.command = codes.addRule .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='ruleBase' then
			evt.command = codes.addRuleBase .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='causalConnector' then
			evt.command = codes.addConnector .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='connectorBase' then
			evt.command = codes.addConnectorBase .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='descriptor' then
			evt.command = codes.addDescriptor .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='descriptorSwitch' then
			evt.command = codes.addDescriptorSwitch .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='descriptorBase' then
			evt.command = codes.addDescriptorBase .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='transition' then
			evt.command = codes.addTransition .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='transitionBase' then
			evt.command = codes.addTransitionBase .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='importBase' then
			evt.command = codes.addImportBase .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='transition' then
			evt.command = codes.addTransition .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. ids.parent[#ids.parent] .. ',' .. tag
		elseif tagName=='importedDocumentBase' then
			evt.command = codes.addImportedDocumentBase .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='importNCL' then
			evt.command = codes.addImportNCL .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='media' or tagName=='context' or tagName=='switch' then
			evt.command = codes.addNode .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. ids.perspective[#ids.perspective] .. ',' .. tag
		elseif tagName=='port' or tagName=='area' or tagName=='property' or tagName=='switchPort' then
			evt.command = codes.addInterface .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. ids.parent[#ids.parent] .. ',' .. tag
		elseif tagName=='link' then
			evt.command = codes.addLink .. ',' .. self.baseId .. ',' .. self.documentId .. ',' .. ids.perspective[#ids.perspective] .. ',' .. tag
		end
]]--
		if tagName=='regionBase' then
			evt.command = codes.addRegionBase .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='region' then
			--evt.command = codes.addRegion .. ',' .. self.documentId .. ',' .. ids.region[#ids.region] .. ',' .. tag
			evt.command = codes.addRegion .. ',' .. self.documentId .. ',' .. ' ' .. ',' .. tag
			ids.region[#ids.region+1] = node.id or ' '
		elseif tagName=='rule' then
			evt.command = codes.addRule .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='ruleBase' then
			evt.command = codes.addRuleBase .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='causalConnector' then
			evt.command = codes.addConnector .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='connectorBase' then
			evt.command = codes.addConnectorBase .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='descriptor' then
			evt.command = codes.addDescriptor .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='descriptorSwitch' then
			evt.command = codes.addDescriptorSwitch .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='descriptorBase' then
			evt.command = codes.addDescriptorBase .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='transition' then
			evt.command = codes.addTransition .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='transitionBase' then
			evt.command = codes.addTransitionBase .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='importBase' then
			evt.command = codes.addImportBase .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='transition' then
			evt.command = codes.addTransition .. ',' .. self.documentId .. ',' .. ids.parent[#ids.parent] .. ',' .. tag
		elseif tagName=='importedDocumentBase' then
			evt.command = codes.addImportedDocumentBase .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='importNCL' then
			evt.command = codes.addImportNCL .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='media' or tagName=='context' or tagName=='switch' then
			evt.command = codes.addNode .. ',' .. self.documentId .. ',' .. ids.perspective[#ids.perspective] .. ',' .. tag
		elseif tagName=='port' or tagName=='area' or tagName=='property' or tagName=='switchPort' then
			--evt.command = codes.addInterface .. ',' .. self.documentId .. ',' .. ids.parent[#ids.parent] .. ',' .. tag
			evt.command = codes.addInterface .. ',' .. self.documentId .. ',' .. self.documentId .. ',' .. tag
		elseif tagName=='link' then
			evt.command = codes.addLink .. ',' .. self.documentId .. ',' .. ids.perspective[#ids.perspective] .. ',' .. tag
		else
			evt.command = ''
		end
		print('lua factored command: [' .. evt.command .. ']')
		if evt.command ~= '' then event.post('out', evt) end
		ids.parent[#ids.parent+1] = node.id or ''
		local changePerspective = false
		if tagName=='ncl' or tagName=='context' or tagName=='switch' then
			ids.perspective[#ids.perspective+1] = node.id or ''
			changePerspective = true
		elseif tagName=='body' and node.id then
			ids.perspective[#ids.perspective+1] = node.id
			changePerspective = true
		end
		for k,v in ipairs(node) do
			if type(v)=='table' then self:visitDocument(v, ids) end
		end

		if tagName=='region' then ids.region[#ids.region] = nil end
		ids.parent[#ids.parent] = nil
		if changePerspective then ids.perspective[#ids.perspective]=nil end
	end,
	stringToTime = function(s)
		if not s then return 0 end
		local h, min = string.match(s, '(%d+):(%d+)')
		if h and min then return 60*h + min
		else return 0 end
	end,
	eventsToNCL = function (self)
--		local wd, hg = canvas:attrSize()
		local wd, hg = 800, 600
		hg = (hg < 100 and hg) or 100
		local minTime, duration, prev
		for k, v in ipairs(self.events) do
			local st, dur = self.stringToTime(v.start_time), self.stringToTime(v.duration)
			if prev and st<prev then st = st + 24*60 end
			prev = st
			v.relativeTime = st
			if not minTime then
				minTime = st
				duration = dur
			else
				duration = st+dur - minTime
			end
		end
		local head = self.doc[1]
		local body = self.doc[2]
		local regionBase, descriptorBase
		for k, v in ipairs(head) do
			if v.tagName=='regionBase' then regionBase = v
			elseif v.tagName=='descriptorBase' then descriptorBase = v end
		end
		for k, v in ipairs(self.events) do
			local id = v.id or k
			local left = tostring( math.floor( ( (v.relativeTime - minTime) / duration ) * wd ) )
			local width = tostring( math.floor( (self.stringToTime(v.duration) / duration) * wd ) )
			regionBase[ #regionBase+1 ] = {tagName='region', id='rg' .. id, left=left, width=width, top='0', height=tostring(hg)}
			descriptorBase[ #descriptorBase+1 ] = {tagName='descriptor', id='ds' .. id, region = 'rg' .. id}
			body[ #body+1 ] = {tagName='media', id=id, src=id .. '.txt', descriptor='ds'..id}
			body[ #body+1 ] = {tagName='port', id='pt'..id, component=id}

			local f = io.open( tostring(id .. '.txt'), "w" )
			if f then
				f:write( v.description or '...' ) 
				f:close()
			end
		end
	end,
	createDocument = function (self)
		event.post('out', {
			class='ncl',
			type='ncledit',
			command='0x6,' .. self.documentId
		})

		event.post('out', {
			class='ncl',
			type='ncledit',
			command='0x5,' .. self.documentId .. ',<?xml version="1.0" encoding="UTF-8"?>\n<ncl id="' .. self.documentId .. '"><head/><body/></ncl>'
		})
		
		self:eventsToNCL()
		self:visitDocument(self.doc)
	end
}

event.register(
	function (evt)
		table.foreach(evt, print)
		if evt.class=='ncl' and evt.type=='epgFactory' then
			if evt.call=='addEvent' then
				evt.class, evt.type = nil, nil
				epg:addEvent(evt)
			elseif evt.call=='startDocument' then
				epg:createDocument()
				print( epg:generate() )
				epg:startDocument()
			elseif evt.call=='clearEvents' then
				epg:stopDocument()
				epg:removeEvents()
			end
		end
	end
)

--[[
epg:addEvent{id='1', start_time='12:00', duration='2:00', even_name_char='Jogo de futebol', text_char='Descricao do jogo'}
epg:addEvent{id='2', start_time='14:10', duration='1:00', even_name_char='Outro evento', text_char='Descricao do outro evento'}
epg:createDocument()

print( epg:generate() )
--]]
