
m f58c7e4fd05ec13383518ccd51663167d45e92d0 Mon Sep 17 00:00:00 2001
From: Daniel Bylinka <daniel.bylinka@gmail.com>
Date: Fri, 2 Apr 2021 19:02:58 +0200
Subject: [PATCH] [statuscmd] Signal mouse button and click location to status
 monitor

---
 config.def.h |   6 +++-
 dwm.c        | 100 ++++++++++++++++++++++++++++++++++++++++++++++++---
 2 files changed, 100 insertions(+), 6 deletions(-)

diff --git a/config.def.h b/config.def.h
index 1c0b587..154a59b 100644
--- a/config.def.h
+++ b/config.def.h
@@ -54,6 +54,8 @@ static const Layout layouts[] = {
 /* helper for spawning shell commands in the pre dwm-5.0 fashion */
 #define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
 
+#define STATUSBAR "dwmblocks"
+
 /* commands */
 static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
 static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
@@ -103,7 +105,9 @@ static Button buttons[] = {
 	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
 	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
 	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
-	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
+	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
+	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
+	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
 	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
 	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
 	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
diff --git a/dwm.c b/dwm.c
index b0b3466..d871457 100644
--- a/dwm.c
+++ b/dwm.c
@@ -172,6 +172,7 @@ static void focusstack(const Arg *arg);
 static Atom getatomprop(Client *c, Atom prop);
 static int getrootptr(int *x, int *y);
 static long getstate(Window w);
+static pid_t getstatusbarpid();
 static int gettextprop(Window w, Atom atom, char *text, unsigned int size);
 static void grabbuttons(Client *c, int focused);
 static void grabkeys(void);
@@ -206,6 +207,7 @@ static void setup(void);
 static void seturgent(Client *c, int urg);
 static void showhide(Client *c);
 static void sigchld(int unused);
+static void sigstatusbar(const Arg *arg);
 static void spawn(const Arg *arg);
 static void tag(const Arg *arg);
 static void tagmon(const Arg *arg);
@@ -238,6 +240,9 @@ static void zoom(const Arg *arg);
 /* variables */
 static const char broken[] = "broken";
 static char stext[256];
+static int statusw;
+static int statussig;
+static pid_t statuspid = -1;
 static int screen;
 static int sw, sh;           /* X display screen geometry width, height */
 static int bh, blw = 0;      /* bar geometry */
@@ -422,6 +427,7 @@ buttonpress(XEvent *e)
 	Client *c;
 	Monitor *m;
 	XButtonPressedEvent *ev = &e->xbutton;
+	char *text, *s, ch;
 
 	click = ClkRootWin;
 	/* focus monitor if necessary */
@@ -440,9 +446,23 @@ buttonpress(XEvent *e)
 			arg.ui = 1 << i;
 		} else if (ev->x < x + blw)
 			click = ClkLtSymbol;
-		else if (ev->x > selmon->ww - (int)TEXTW(stext))
+		else if (ev->x > selmon->ww - statusw) {
+			x = selmon->ww - statusw;
 			click = ClkStatusText;
-		else
+			statussig = 0;
+			for (text = s = stext; *s && x <= ev->x; s++) {
+				if ((unsigned char)(*s) < ' ') {
+					ch = *s;
+					*s = '\0';
+					x += TEXTW(text) - lrpad;
+					*s = ch;
+					text = s + 1;
+					if (x >= ev->x)
+						break;
+					statussig = ch;
+				}
+			}
+		} else
 			click = ClkWinTitle;
 	} else if ((c = wintoclient(ev->window))) {
 		focus(c);
@@ -704,9 +724,24 @@ drawbar(Monitor *m)
 
 	/* draw status first so it can be overdrawn by tags later */
 	if (m == selmon) { /* status is only drawn on selected monitor */
+		char *text, *s, ch;
 		drw_setscheme(drw, scheme[SchemeNorm]);
-		tw = TEXTW(stext) - lrpad + 2; /* 2px right padding */
-		drw_text(drw, m->ww - tw, 0, tw, bh, 0, stext, 0);
+
+		x = 0;
+		for (text = s = stext; *s; s++) {
+			if ((unsigned char)(*s) < ' ') {
+				ch = *s;
+				*s = '\0';
+				tw = TEXTW(text) - lrpad;
+				drw_text(drw, m->ww - statusw + x, 0, tw, bh, 0, text, 0);
+				x += tw;
+				*s = ch;
+				text = s + 1;
+			}
+		}
+		tw = TEXTW(text) - lrpad + 2;
+		drw_text(drw, m->ww - statusw + x, 0, tw, bh, 0, text, 0);
+		tw = statusw;
 	}
 
 	for (c = m->clients; c; c = c->next) {
@@ -872,6 +907,30 @@ getatomprop(Client *c, Atom prop)
 	return atom;
 }
 
+pid_t
+getstatusbarpid()
+{
+	char buf[32], *str = buf, *c;
+	FILE *fp;
+
+	if (statuspid > 0) {
+		snprintf(buf, sizeof(buf), "/proc/%u/cmdline", statuspid);
+		if ((fp = fopen(buf, "r"))) {
+			fgets(buf, sizeof(buf), fp);
+			while ((c = strchr(str, '/')))
+				str = c + 1;
+			fclose(fp);
+			if (!strcmp(str, STATUSBAR))
+				return statuspid;
+		}
+	}
+	if (!(fp = popen("pidof -s "STATUSBAR, "r")))
+		return -1;
+	fgets(buf, sizeof(buf), fp);
+	pclose(fp);
+	return strtol(buf, NULL, 10);
+}
+
 int
 getrootptr(int *x, int *y)
 {
@@ -1637,6 +1696,20 @@ sigchld(int unused)
 	while (0 < waitpid(-1, NULL, WNOHANG));
 }
 
+void
+sigstatusbar(const Arg *arg)
+{
+	union sigval sv;
+
+	if (!statussig)
+		return;
+	sv.sival_int = arg->i;
+	if ((statuspid = getstatusbarpid()) <= 0)
+		return;
+
+	sigqueue(statuspid, SIGRTMIN+statussig, sv);
+}
+
 void
 spawn(const Arg *arg)
 {
@@ -1990,8 +2063,25 @@ updatesizehints(Client *c)
 void
 updatestatus(void)
 {
-	if (!gettextprop(root, XA_WM_NAME, stext, sizeof(stext)))
+	if (!gettextprop(root, XA_WM_NAME, stext, sizeof(stext))) {
 		strcpy(stext, "dwm-"VERSION);
+		statusw = TEXTW(stext) - lrpad + 2;
+	} else {
+		char *text, *s, ch;
+
+		statusw  = 0;
+		for (text = s = stext; *s; s++) {
+			if ((unsigned char)(*s) < ' ') {
+				ch = *s;
+				*s = '\0';
+				statusw += TEXTW(text) - lrpad;
+				*s = ch;
+				text = s + 1;
+			}
+		}
+		statusw += TEXTW(text) - lrpad + 2;
+
+	}
 	drawbar(selmon);
 }
 
-- 
2.31.0

