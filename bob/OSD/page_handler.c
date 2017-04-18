#include "tw_widget_sys.h"
#include "res.h"
#include "page_ST_LOGO.h"
#include "ignore_page.h"

unsigned char tw_page_handler(unsigned char id, TW_EVENT* event)
{
	unsigned char rc=0;
	switch(id)
	{
		case ID_PAGE_ST_LOGO:
			rc = page_ST_LOGO_process(event);
		break;
		default:	/* put to nodify if no process this event */
			rc = TW_RETURN_NO_PAGE;
		break;
	}

	return rc;
}
