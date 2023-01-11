# polybar-dwm - dwm with polybar support*
 this is patched to work with polybar-dwm-module which you can find here
 https://github.com/mihirlad55/polybar-dwm-module

![screenshot](https://github.com/KawaiiKraken/polybar-dwm/blob/master/myconfig.png "My Config")

# Included Patches
- anybar-polybar-tray-fix
- fullgaps (can be disabled in config.h)
- ipc
- restartsig
- restoreafterrestart
- anybar-togglebar (custom patch)
- minor changes to stop weird behaviour with polybar

# Dependencies
------------
- yajl
- polybar-dwm-module
- freetype2
- libxft
- libxinerama
- libx11

Installation
------------

    make clean install`


Running dwm
-----------
Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)

OR

use xrandr --output in .xinitrc (might not work in all dispaly configs)


Running polybar
-----------
Add the following line to the top of your .xinitrc:
      
    $(sleep 5 && $HOME/polybar-dwm/bar.sh) &

Changing the sleep time is not recommended for consistent results.



Configuration
-------------
- The configuration of dwm is done by editing config.h and running make install again.
- polybar-dwm-module is configured in .config/polybar/config
