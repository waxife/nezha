#include <unistd.h>
#include <debug.h>
#include <io.h>
#include <stdio.h>
#include <time.h>
#include "tw_widget_sys.h"
#include "drivers/osd2/osd2.h"
#include "drivers/spiosd/spiosd.h"
#include "OSD/hello.h"
#include "OSD/res.h"
 #include "./drivers/spirw/spirw.h"
#include "./drivers/spiosd/spiosd.h"
#include "./drivers/osd2/osd2.h"
#include "./drivers/calibration/calibration.h"
#include "./drivers/sarkey/sarkey.h"

unsigned char hello_process (TW_EVENT* event)
{
	switch(event->type)
	{
		case TW_EVENT_TYPE_ENTER_SYSTEM:
			dbg(2, ">>>>> Enter Menu Page\n\r");		
		break;

		case TW_EVENT_TYPE_QUIT_SYSTEM:
			dbg(2, ">>>>> QUIT Menu Page\n\r");

			/* todo: Write Your Code Here */

		break;

		default:	/* put to nodify if no process this event */
			return TW_RETURN_NO_PROCESS;
	}
	return TW_RETURN_NONE;
}