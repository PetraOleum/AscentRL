#Ascent RogueLike

##Idea

A dungeon of rooms and corridoors; however, they are generated as-needed by the engine to reflect where you are/your progress/etc. As such, the dungeon makes no topological sense, however you can traverse backwards if you want and you have multiple paths avaliable to you. The game prohibits grinding in that once you clear a room it's cleared, and when you enter a new room you get what you're given - i.e. the next challenge. The dungeon can go "easy" on you, but that means things will take longer. The dungeon will try to get you to do what it wants; your goal is to get it to want you to leave (or let you, because you've been such a nice girl).

## Controls

Currently implemented:

* hjkluinm: Vi/Nethack style 8-way movement
* Numpad 12346789: Numpad style 8-way movement
* &larr;&uarr;&darr;&rarr;: Arrow key 4-way movement
* . (full stop/period): Wait 1 action
* Numpad 5: Wait 1 action
* g: Pickup item
* d: Drop item (takes you to invetory screen; select item with a-zA-Z as applicable, &lt;Esc&gt; to cancel)
* &lt;F11&gt;: toggle fullscreen
* &lt;Esc&gt;: get out of fullscreen (when not in inventory mode)
* Mouse click: Attempt to path to

Moving into an enemy attacks
