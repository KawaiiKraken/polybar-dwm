# dwm - dynamic window manager
============================
why dwm? its extremely fast, stable and has lots of patches.

![screenshot](https://github.com/KawaiiKraken/polybar-dwm/blob/master/myconfig.png "My Config")

# Included Patches
- anybar-polybar-tray-fix
- fullgaps
- ipc
- notitle (might remove as it doesnt affect polybar)
- restartsig
- restoreafterrestart
- statuscmd (also might be removed)

# Requirements (to add to normal dwm requirements)
------------
- yajl (small C JSON library)

Installation
------------

    make clean install


Running dwm
-----------
- Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

- In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)

OR

use xrandr --output in .xinitrc (might not work in all dispaly configs)

- In order to display status info in the bar, you can do something
like this in your .xinitrc:

    while xsetroot -name "`date` `uptime | sed 's/.*,//'`"
    do
    	sleep 1
    done &
    exec dwm


Configuration
-------------
The configuration of dwm is done by editing config.h and running make install again.
