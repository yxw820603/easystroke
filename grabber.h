#ifndef __GRABBER_H__
#define __GRABBER_H__
#include "prefdb.h"
#include <string>
#include <map>
#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <X11/cursorfont.h>

class Grabber;
extern Grabber *grabber;

class Grabber {
public:
	enum State { NONE, BUTTON, ALL_SYNC, ALL_ASYNC, POINTER };
	enum EventType { DOWN = 0, UP = 1, MOTION = 2, PROX_IN = 3, PROX_OUT = 4 };
	static const char *state_name[5];
	bool xinput;
	bool proximity_selected;
	bool is_event(int, EventType);
	unsigned int get_device_button_state();
	bool supports_pressure();
	bool supports_proximity();
	void select_proximity();

	struct XiDevice {
		int event_type[5];
		bool supports_proximity;
		bool supports_pressure;
		int pressure_min;
		int pressure_max;
		XDevice *dev;
		XEventClass events[6];
		int normalize_pressure(int pressure) {
			return 255 * (pressure - pressure_min) / (pressure_max - pressure_min);
		}
	};
	XiDevice *get_xi_dev(XID id);
private:
	int button_events_n;
	int all_events_n;
	int proximity_events_n; // starts after all_events_n
	Window proximity_win;
	XiDevice **xi_devs;
	int xi_devs_n;
	bool init_xi();

	State current, grabbed;
	bool xi_grabbed;
	bool suspended;
	bool active;
	Cursor cursor;
	std::map<guint, guint> buttons;
	bool timing_workaround;

	Atom wm_state;

	void set();
	std::string get_wm_state(Window w);
	void grab_xi(bool);
public:
	Grabber();
	~Grabber();
	void init(Window w, int depth);
	bool has_wm_state(Window w);
	void update(Window w) { active = !RPrefEx(prefs().exceptions)->count(get_wm_state(w)); set(); }
	void create(Window w);
	void get_button();
	void fake_button(int b);
	void grab(State s) { current = s; set(); }
	void suspend() { suspended = true; set(); }
	void resume() { suspended = false; set(); }
	void regrab() { suspend(); grab_xi(false); get_button(); resume(); grab_xi(true); }
	void grab_xi_devs(bool);
	bool is_grabbed(guint b) { return buttons.find(b) != buttons.end(); }
};

class GrabFailedException : public std::exception {
	virtual const char* what() const throw() { return "Grab Failed"; }
};

#endif
