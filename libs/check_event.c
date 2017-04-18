#include "tw_widget_sys.h"
#include <sys/include/keypad.h>
void check_event(void)
{
		TW_EVENT s_event;
		int sar=0x00;
		sar = keypad_get_keyvalue();
		if(sar != -1)
		{
			s_event.type = TW_EVENT_TYPE_KEY_DOWN;
			s_event.keystroke.ch = (unsigned char)sar;
			put_event(&s_event);
		}
}
