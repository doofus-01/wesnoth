--! #textdomain wesnoth

-- This file may provide an implementation of Lua functions removed from the engine.

local helper = wesnoth.require "lua/helper.lua"

-- Calling wesnoth.fire isn't the same as calling wesnoth.wml_actions[name] due to the passed vconfig userdata
-- which also provides "constness" of the passed wml object from the point of view of the caller.
-- So please don't remove since it's not deprecated.
function wesnoth.fire(name, cfg)
	wesnoth.wml_actions[name](wesnoth.tovconfig(cfg or {}))
end

-- This is deprecated, as it was an improper port of the [modify_side] WML tag
function wesnoth.modify_side(cfg)
	wesnoth.wml_actions.modify_side(cfg)
end
