/**
 *  @file   story.h
 *  @brief  story framework for integration 
 *  $Id: story.h,v 1.3 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */

#ifndef __STORY_H__
#define __STORY_H__

#include <string.h>

typedef struct story {
	int	    (*story) (int opt);
    const char      *group;
	const char	    *name;
} story_t;

extern story_t __story_start, __story_end;

#define STORY(xgroup, xname) \
    static int xgroup##_##xname (int opt) __attribute__ ((unused, section (".story"))); \
    static story_t __story_##xgroup##_##xname \
        __attribute__ ((unused, section (".story_table"))) \
        = {xgroup##_##xname, #xgroup, #xname }; \
    static int xgroup##_##xname(int opt)

#define IGNORE_STORY(xgroup, xname) \
    static int xgroup##_##xname (int opt) __attribute__ ((unused, section (".story"))); \
    static story_t __story_##xgroup##_##xname \
        __attribute__ ((unused, section (".story_table"))) \
        = {0 , #xgroup, #xname }; \
    static int xgroup##_##xname(int opt)


#define VERBOSE_STORY       0x0001
#define STOP_STORY          0x0002

typedef struct story_board {
    int     regs[4];
    int     success;
    int     fail;
    int     counter;
    char    str[32];    
} story_board_t;

extern story_board_t    _sb;

#define S_R0    0
#define S_R1    1
#define S_R2    2
#define S_R3    3

#define STORY_ASSERT(rc)            ({ if (rc >= 0) { _sb.success++; } else { _sb.fail++; } })
#define STORY_SET(xreg, xval)       ({ _sb.regs[xreg] = xval; })
#define STORY_SET_STR(xstr)         ({ if(xstr == NULL) strncpy(_sb.str, "(null)", 32); \
                                            else strncpy(_sb.str, xstr, 32); })
#define STORY_INC_COUNTER()         ({ _sb.counter++; })
#define STORY_DEC_COUNTER()         ({ _sb.counter--; })

//extern int story_printf(const char *fmt, ...) __attribute__((format (printf, 1, 2)));
#define story_printf    __printf

extern int __printf(const char *fmt, ...) __attribute__((format (printf, 1, 2)));

extern int story_board(void);
extern int story_reset(void);
extern int __item(const char *shell, const char *args, const char *fname, int lno);
extern int __item_check(int xreg, int val, const char *message, 
            const char *item, const char *fname, int lno);
extern int __item_check_hex(int xreg, int val, const char *message, 
            const char *item, const char *fname, int lno);
extern int __item_check_str(const char *xstr, const char *message, 
            const char *item, const char *fname, int lno);
extern int __item_expect(int success, int fail,const char *message, 
            const char *item, const char *fname, int lno); 
extern int __item_check_counter(int counter, const char *message, 
            const char *item, const char *fname, int lno);

extern int item_get(int xreg);
extern int item_get_counter(void);
extern char *item_get_str(void); 

extern int list_story(void);
extern int run_all_story(int opt);
extern int run_story(const char *story, const char *chapter, int opt);

#define story_init() \
    char *jump __attribute__((unused)); int check_rc __attribute__((unused)); \
        const char *item_str __attribute__((unused)); int item_errs = 0

#define story_end()  item_errs

#define ignore_item(shell, args) \
    do { \
        __label__ _end_story;

#define item(shell, args) \
    do { \
        __label__ _end_story; \
        if ((opt & STOP_STORY) == STOP_STORY && item_errs > 0) return item_errs; \
        jump = &&_end_story; \
        item_str = #shell" "#args; \
        story_reset(); \
        check_rc = __item(#shell, #args, __FILE__, __LINE__); \
            if (check_rc < 0) goto *jump
        
#define item_end(shell) \
    _end_story: \
        if (check_rc >= 0) \
            if ((opt & VERBOSE_STORY) == VERBOSE_STORY) \
                story_printf("\n  >>item(%s) Pass", item_str); \
            else \
                story_printf("."); \
        else item_errs++; \
    } while(0)            

#define item_check(xreg, xval, msg) \
    ({ check_rc = __item_check(xreg, xval, msg, item_str,  __FILE__, __LINE__); \
           if (check_rc < 0) goto *jump; })


#define item_check_hex(xreg, xval, msg) \
    ({ check_rc = __item_check_hex(xreg, xval, msg, item_str,  __FILE__, __LINE__); \
           if (check_rc < 0) goto *jump; })

#define item_check_str(xstr, msg) \
    ({ check_rc = __item_check_str(xstr, msg, item_str,  __FILE__, __LINE__); \
           if (check_rc < 0) goto *jump; })

#define item_check_counter(xcounter, msg) \
    ({ check_rc = __item_check_counter(xcounter, msg, item_str,  __FILE__, __LINE__); \
           if (check_rc < 0) goto *jump; })

#define item_expect(xsuccess,xfail, msg) \
    ({ check_rc = __item_expect(xsuccess, xfail, msg, item_str, __FILE__, __LINE__); \
           if (check_rc < 0) goto *jump; })

#define item_expect_success(xsuccess, msg) \
    ({ check_rc = __item_expect(xsuccess, _sb.fail, msg, item_str,  __FILE__, __LINE__); \
           if (check_rc < 0) goto *jump; })

#define item_expect_fail(xfail, msg) \
    ({ check_rc = __item_expect(_sb.success, xfail, msg, item_str, __FILE__, __LINE__); \
           if (check_rc < 0) goto *jump; })

#endif /* __STORY_H__ */
