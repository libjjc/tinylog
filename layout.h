#ifndef LOG_LAYOUT_HH
#define LOG_LAYOUT_HH

#include "logstr.h"

struct _adapter;
typedef ls_t(*layout_callback)(void* ,struct _log_msg* );

struct patternLayout {
	ls_t pattern;
	layout_callback layout;
};

struct basicLayout {
    layout_callback layout;
};

layout_callback
createPatternLayout(struct _adapter* ada, ls_t args);

void
freePatternLayout(layout_callback layout);

/**
 * represent the time specified in timeptr. They all begin 
 * with a percentage (%) sign, and are:
 * specifier	Replaced by	                              Example
 * %a	        Abbreviated weekday name *	              Thu
 * %A	        Full weekday name *	                      Thursday
 * %b			Abbreviated month name *	              Aug
 * %B			Full month name *	                      August
 * %c			Date and time representation *	          Thu Aug 23 14:55:02 2001
 * %d			Day of the month (01-31)	              23
 * %H			Hour in 24h format (00-23)	              14
 * %I			Hour in 12h format (01-12)	              02
 * %j			Day of the year (001-366)	              235
 * %m			Month as a decimal number (01-12)	      08
 * %M			Minute (00-59)	                          55
 * %p			AM or PM designation	                  PM
 * %S			Second (00-61)	                          02
 * %U			Week number with the first Sunday as
 *              the first day of week one (00-53)	      33
 * %w			Weekday as a decimal number with 
 *              Sunday as 0 (0-6)	                      4
 * %W			Week number with the first Monday as 
 *              the first day of week one (00-53)	      34
 * %x			Date representation *	                  08/23/01
 * %X			Time representation *	                  14:55:02
 * %y			Year, last two digits (00-99)	          01
 * %Y			Year	                                  2001
 * %Z			Timezone name or abbreviation	          CDT
 * %%			A % sign	                              %
 */
ls_t 
patternLayout(layout_callback layout ,struct _log_msg* msg);

layout_callback
createBasicLayout(struct _adapter* ada);

void
freeBasicLayout(layout_callback layout);

ls_t
basicLayout(layout_callback layout ,struct _log_msg* msg);


#endif//LOG_LAYOUT_HH
