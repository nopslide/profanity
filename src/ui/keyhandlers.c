/*
 * keyhandlers.c
 *
 * Copyright (C) 2012 - 2014 James Booth <boothj5@gmail.com>
 *
 * This file is part of Profanity.
 *
 * Profanity is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Profanity is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Profanity.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link the code of portions of this program with the OpenSSL library under
 * certain conditions as described in each individual source file, and
 * distribute linked combinations including the two.
 *
 * You must obey the GNU General Public License in all respects for all of the
 * code used other than OpenSSL. If you modify file(s) with this exception, you
 * may extend this exception to your version of the file(s), but you are not
 * obligated to do so. If you do not wish to do so, delete this exception
 * statement from your version. If you delete this exception statement from all
 * source files in the program, then also delete it here.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <glib.h>

#include "ui/inputwin.h"
#include "common.h"

void
key_printable(char * const line, int * const line_utf8_pos, int * const col, int * const pad_start, const wint_t ch, const int wcols)
{
    int utf8_len = g_utf8_strlen(line, -1);

    // handle insert if not at end of input
    if (*line_utf8_pos < utf8_len) {
        // create new line
        char bytes[MB_CUR_MAX];
        size_t utf8_ch_len = wcrtomb(bytes, ch, NULL);
        bytes[utf8_ch_len] = '\0';
        gchar *start = g_utf8_substring(line, 0, *line_utf8_pos);
        gchar *end = g_utf8_substring(line, *line_utf8_pos, utf8_len);
        GString *new_line_str = g_string_new(start);
        g_string_append(new_line_str, bytes);
        g_string_append(new_line_str, end);
        char *new_line = new_line_str->str;
        g_free(start);
        g_free(end);
        g_string_free(new_line_str, FALSE);

        // replace old line
        strncpy(line, new_line, INP_WIN_MAX);
        free(new_line);

        // set utf8 position
        (*line_utf8_pos)++;

        // set col position
        (*col)++;
        gunichar uni = g_utf8_get_char(bytes);
        if (g_unichar_iswide(uni)) {
            (*col)++;
        }

        // set pad_start
        int display_len = utf8_display_len(line);
        (*pad_start) = 0;
        if (display_len > wcols-2) {
            (*pad_start) = display_len - wcols + 1;
        }

    // otherwise just append
    } else {
        char bytes[MB_CUR_MAX+1];
        size_t utf8_ch_len = wcrtomb(bytes, ch, NULL);

        // wcrtomb can return (size_t) -1
        if (utf8_ch_len < MB_CUR_MAX) {
            // update old line
            int i;
            int bytes_len = strlen(line);

            for (i = 0 ; i < utf8_ch_len; i++) {
                line[bytes_len++] = bytes[i];
            }
            line[bytes_len] = '\0';

            // set utf8 position
            (*line_utf8_pos)++;

            // set col position
            (*col)++;
            bytes[utf8_ch_len] = '\0';
            gunichar uni = g_utf8_get_char(bytes);
            if (g_unichar_iswide(uni)) {
                (*col)++;
            }

            // set pad_start
            // if gone over screen size follow input
            if (*col - *pad_start > wcols-2) {
                (*pad_start)++;
                if (g_unichar_iswide(uni)) {
                    (*pad_start)++;
                }
            }
        }
    }
}