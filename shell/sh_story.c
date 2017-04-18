/**
 *  @file   sh_story.c
 *  @brief  system init entry
 *  $Id: sh_story.c,v 1.4 2014/06/25 03:27:19 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/01  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <story.h>

command_init (sh_story, "story", "story all | <story> [<chapter>]");
command_init (sh_story_board, "story_board", "story_board");
command_init (sh_story_reset, "story_reset", "story_reset");
command_init (sh_story_test, "story_test", "story_test a1 ...");
command_init (sh_story_test_str, "story_test_str", "story_test_str str");
command_init (sh_story_test_counter, "story_test_counter", "story_test_counter");


static int sh_story (int argc, char **argv, void **result, void *sys_ctx)
{
    int opt = 0;
    int argopt = STOP_STORY;
    int allopt = 0;
    int iarg = 1;

    if (argc < 2) {
        list_story();
        return 0;
    }


    if (strcmp(argv[iarg], "-v") == 0) {
        opt = VERBOSE_STORY;
        iarg++;
        argc--;
    }

    if (strcmp(argv[iarg], "-s") == 0) {
        allopt |= STOP_STORY;
        argopt |= STOP_STORY;
        iarg++;
        argc--;
    }

    if (strcmp(argv[iarg], "-c") == 0) {
        allopt &= ~STOP_STORY;
        argopt &= ~STOP_STORY;
        iarg++;
        argc--;
    }
    
    if (strcmp(argv[iarg], "all") == 0) {
        run_all_story(opt | allopt);
    } else {
        if (argc < 3) {
            run_story(argv[iarg], NULL, opt | argopt);
        } else {
            run_story(argv[iarg], argv[iarg+1], opt | argopt);
        }
    }

    return 0;
}

static int sh_story_board(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    rc = story_board();
    STORY_ASSERT(rc);

    return 0;
}

static int sh_story_reset(int argc, char **argv, void **result, void *sys_ctx)
{
    story_reset();

    return 0;
}

static int sh_story_test(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    int val;
    rc = get_val(argv[1], &val);
    if (rc < 0) val = -1;
    STORY_SET(S_R0, val);
    rc = get_val(argv[2], &val);
    if (rc < 0) val = -1;
    STORY_SET(S_R1, val);
    rc = get_val(argv[3], &val);
    if (rc < 0) val = -1;
    STORY_SET(S_R2, val);
    rc = get_val(argv[4], &val);
    if (rc < 0) val = -1;
    STORY_SET(S_R3, val);

    return 0;
}

static int sh_story_test_str(int argc, char **argv, void **result, void *sys_ctx)
{
    char *str = NULL;

    if (argc >= 2) 
        str = argv[1];

    STORY_SET_STR(str);

    return 0;
}

static int sh_story_test_counter(int argc, char **argv, void **result, void *sys_ctx)
{
    int loop;
    int i;
    int rc;

    if (argc < 2)
        return 0;
    
    rc = get_val(argv[1], &loop);
    if (rc < 0)
        return 0;
    
    if (loop < 0) {
        for (i = 0; i < -loop; i++)
            STORY_DEC_COUNTER();
    } else {
        for (i = 0; i < loop; i++)
            STORY_INC_COUNTER();
    }

    return 0;
}


STORY(astory, story_basic)
{
    story_init();

    item(story_board, );
    item_expect_success(1, "story_board should always success");
    item_end();

    item(story_test, 1 20 -30 99);
    item_check(S_R0, 1, "S_R0");
    item_check(S_R1, 20, "S_R1");
    item_check(S_R2, -30, "S_R2"); /* should be wrong */
    item_check(S_R3, 99, "S_R3");
    item_end();


    item(story_test_str, this_is_a_book);
    item_check_str("this_is_a_book", "test_str");
    item_end();

    item(story_test_str, );
    item_check_str("(null)", "test_str null");
    item_end();

    item(story_test_counter, 100);
    item_check_counter(100, "counter");
    item_end();


    item(story_test_counter, -99);
    item_check_counter(-99, "counter");
    item_end();

    return story_end();
}

STORY(astory, error)
{
    story_init();

    item(story_board, );
    item_expect_success(1, "story_board should always success");
    item_end();

    item(story_test, 1 20 -30 99);
    item_check(S_R0, 1, "S_R0");
    item_check(S_R1, 20, "S_R1");
    item_check(S_R2, 30, "S_R2"); /* should be wrong */
    item_check(S_R3, 99, "S_R3");
    item_end();

    item(story_test_counter, -99);
    item_check_counter(-99, "counter");
    item_end();


    return story_end();
}


IGNORE_STORY(astory, ignore)
{
    story_init();

    item(story_board, );
    item_expect_success(1, "story_board should always success");
    item_end();

    item(story_test, 1 20 -30 99);
    item_check(S_R0, 1, "S_R0");
    item_check(S_R1, 20, "S_R1");
    item_check(S_R2, 30, "S_R2"); /* should be wrong */
    item_check(S_R3, 99, "S_R3");
    item_end();

    item(story_test_counter, -99);
    item_check_counter(-99, "counter");
    item_end();


    return story_end();
}

STORY(another, 1)
{
    story_init();

    item(story_test_counter, 1);
    item_check_counter(1, "counter");
    item_end();

    return story_end();
}
