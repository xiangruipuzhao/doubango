/*
 * This file is part of the Sofia-SIP package
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Pekka Pessi <pekka.pessi@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

/**@ingroup test_memmem
 *
 * @CFILE test_memmem.c
 *
 * Torture tests for memmem() and strcasestr().
 *
 * @author Pekka Pessi <Pekka.Pessi@nokia.com>
 *
 * @date Created: Tue Aug 21 15:18:26 2001 ppessi
 */

#include "config.h"

#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <assert.h>

#if !HAVE_MEMMEM
void *memmem(const void *haystack, size_t haystacklen,
	     const void *needle, size_t needlelen);
#endif
#if !HAVE_STRCASESTR
char *strcasestr(const char *haystack, const char *needle);
#endif

#include <string.h>

static int test_flags = 0;
#define TSTFLAGS test_flags

#include <sofia-sip/tstdef.h>

char const name[] = "test_memmem";

void usage(int exitcode)
{
  fprintf(stderr, "usage: %s [-v] [-a]\n", name);
  exit(exitcode);
}

static int test_notfound(void);
static int test_pattern(void);

char const *null = NULL;

static int test_notfound(void)
{
  char const haystack[] = "abcabcabcabc";
  char const needle[] = "cab";
  char const *a;
  BEGIN();

  TEST_P(memmem(haystack, 12, needle, 3), haystack + 2);
  TEST_P(memmem(needle, 3, haystack, 12), NULL);

#if HAVE_MEMMEM
  if (memmem(haystack, 12, "", 0) == NULL) {
    fprintf(stderr, "test_memmem.c: "
	    "*** WARNING: system memmem() fails with empty needle ***\n");
  }
  else
#endif
  {
    TEST_P(memmem(haystack, 12, "", 0), haystack);
    TEST_P(memmem(haystack, 12, null, 0), haystack);
    TEST_P(memmem(haystack, 0, "", 0), haystack);
    TEST_P(memmem(haystack, 0, null, 0), haystack);
  }

  TEST_P(memmem(haystack + 2, 3, needle, 3), haystack + 2);
  TEST_P(memmem(haystack + 2, 2, needle, 3), NULL);
  TEST_P(memmem(a = "a\0bc", 4, "a\0bc", 4), a);
  TEST_P(memmem(a, 4, "\0bc", 3), a + 1);

  END();
}

static int test_pattern(void)
{
  BEGIN();
  END();
}

int test_strcasestr(void)
{
  BEGIN();

  {
    char const hs[] =
      "A case-folding string searching test consisting of a Long String";
    char const *s;

    s = strcasestr(hs, "sting");
    TEST_S(s, hs + 42);

    s = strcasestr(hs, "String");
    TEST_S(s, hs + 15);

    s = strcasestr(hs, "S");
    TEST_S(s, hs + 4);

    s = strcasestr(hs, "L");
    TEST_S(s, hs + 9);

    s = strcasestr(hs, "trings");
    TEST_1(s == NULL);

    s = strcasestr(hs, "String");
    TEST_S(s, hs + 15);

    s = strcasestr(hs, "StRiNg");
    TEST_S(s, hs + 15);

    s = strcasestr(hs, "OnG");
    TEST_S(s, hs + 54);

    /* Special cases */
    TEST_1(strcasestr(hs, "") == hs);
    TEST_1(strcasestr("", "ong") == NULL);
    TEST_1(strcasestr("", "OnG") == NULL);
    TEST_1(strcasestr("ong", hs) == NULL);
    TEST_1(strcasestr("OnG", hs) == NULL);
    TEST_1(strcasestr(hs, "Z") == NULL);
    TEST_1(strcasestr(hs, "z") == NULL);
  }

  {
    char const hs[] =
"A case-folding string searching test consisting of a Very Long String\n"

"Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Integer felis. "
"Suspendisse potenti. Morbi malesuada erat eget enim. Sed dui lorem, aliquam "
"eu, lobortis eget, dapibus vitae, velit. Cras non purus. Suspendisse massa. "
"Curabitur gravida condimentum massa. Donec nunc magna, lacinia non, "
"pellentesque ac, laoreet vel, eros. Praesent lectus leo, vestibulum eu, "
"tempus eu, ullamcorper tristique, mi. Duis fringilla ultricies lacus. Ut "
"non pede. Donec id libero. Cum sociis natoque penatibus et magnis dis "
"parturient montes, nascetur ridiculus mus. Phasellus bibendum.\n"

"Vestibulum turpis. Nunc euismod. Maecenas venenatis, purus at pharetra "
"ultrices, orci orci blandit nisl, eget vulputate enim tortor sed nunc. "
"Proin sit amet elit. Donec ut justo. In quis nisi. Praesent posuere. "
"Maecenas porta. Curabitur pharetra. Class aptent taciti sociosqu ad litora "
"torquent per conubia nostra, per inceptos hymenaeos. Donec suscipit ligula. "
"Quisque facilisis ante eget mi. Nunc ac est.\n"

"Quisque in sapien eget justo aliquam laoreet. Nullam ultricies est id dolor. "
"Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Suspendisse ante "
"velit, eleifend at, ullamcorper ut, rutrum et, ipsum. Mauris luctus, tellus "
"non elementum convallis, nunc ipsum hendrerit lectus, ut lacinia elit nulla "
"ac tellus. In sit amet velit. Maecenas non dolor. Sed commodo diam a pede. "
"Ut non pede. Vestibulum condimentum turpis vel lacus consectetuer dictum. "
"Nulla ullamcorper mi eu pede. Donec mauris tortor, facilisis vitae, "
"hendrerit nec, lobortis nec, eros. Nam sit amet mi. Ut pharetra, orci nec "
"porta convallis, lacus velit blandit sapien, luctus nonummy lacus dolor vel "
"sapien. Suspendisse placerat. Donec ante turpis, volutpat eu, hendrerit "
"vel, eleifend ac, arcu. Nulla facilisi. Sed faucibus facilisis lectus. "
"Aliquam congue justo nec dui.\n"

"Donec dapibus dui sed nisl. Proin congue. Curabitur placerat diam id eros. "
"Pellentesque vitae nulla. Quisque at lorem et dolor auctor consequat. Sed "
"sed tellus non nibh imperdiet venenatis. Integer ultrices dapibus nisi. "
"Aenean vehicula malesuada risus. Fusce egestas malesuada leo. In "
"ullamcorper pretium lorem. Vestibulum ante ipsum primis in faucibus orci "
"luctus et ultrices posuere cubilia Curae;\n"

"Phasellus congue. Morbi lectus arcu, mattis non, pulvinar et, condimentum "
"non, mi. Suspendisse vestibulum nunc eu neque. Sed rutrum felis aliquam "
"urna. Ut tincidunt orci vitae ipsum. Nullam eros. Quisque augue. Quisque "
"lacinia. Nunc ligula diam, nonummy a, porta in, tristique quis, leo. "
"Phasellus nunc nulla, fringilla vel, lacinia et, suscipit a, turpis. "
"Integer a est. Curabitur mauris lacus, vehicula sit amet, sodales vel, "
"iaculis vitae, massa. Nam diam est, ultrices vitae, varius et, tempor a, "
"leo. Class aptent taciti sociosqu ad litora torquent per conubia nostra, "
"per inceptos hymenaeos. Fusce felis nibh, ullamcorper non, malesuada eget, "
      "facilisis vel, purus.\n";

char const needle[] =
"Proin congue. Curabitur placerat diam id eros. "
"Pellentesque vitae nulla. Quisque at lorem et dolor auctor consequat. Sed "
"sed tellus non nibh imperdiet venenatis. Integer ultrices dapibus nisi. "
"Aenean vehicula malesuada risus. Fusce egestas malesuada leo. In "
"ullamcorper pretium lorem. Vestibulum ante ipsum primis in faucibus orci "
"luctus et ultrices posuere cubilia Curae;\n";

char const Needle[] =
"PROIN CONGUE. CURABITUR PLACERAT DIAM ID EROS. "
"PELLENTESQUE VITAE NULLA. QUISQUE AT LOREM ET DOLOR AUCTOR CONSEQUAT. SED "
"SED TELLUS NON NIBH IMPERDIET VENENATIS. INTEGER ULTRICES DAPIBUS NISI. "
"AENEAN VEHICULA MALESUADA RISUS. FUSCE EGESTAS MALESUADA LEO. IN "
"ULLAMCORPER PRETIUM LOREM. VESTIBULUM ANTE IPSUM PRIMIS IN FAUCIBUS ORCI "
"LUCTUS ET ULTRICES POSUERE CUBILIA CURAE;\n";

    char const *s;

    s = strcasestr(hs, needle);
    TEST_S(s, hs + 1920);

    s = strcasestr(hs, Needle);
    TEST_S(s, hs + 1920);
  }

  END();
}

int main(int argc, char *argv[])
{
  int retval = 0;
  int i;

  for (i = 1; argv[i]; i++) {
    if (strcmp(argv[i], "-v") == 0)
      test_flags |= tst_verbatim;
    else if (strcmp(argv[i], "-a") == 0)
      test_flags |= tst_abort;
    else
      usage(1);
  }

  retval |= test_notfound(); fflush(stdout);
  retval |= test_pattern(); fflush(stdout);
  retval |= test_strcasestr(); fflush(stdout);


  return retval;
}
