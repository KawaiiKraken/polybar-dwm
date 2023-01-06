if pgrep '^polybar' > /dev/null; then
  exit 0
fi
if type "xrandr"; then
  for m in $(xrandr --query | grep " connected" | cut -d" " -f1); do
    MONITOR=$m polybar --reload mainbar & MONITOR=$m polybar --reload secondarybar & 
  done
else
  polybar --reload mainbar & polybar --reload secondarybar &
fi
