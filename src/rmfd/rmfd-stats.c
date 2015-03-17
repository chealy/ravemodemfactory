/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 * rmfd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2015 Zodiac Inflight Innovations
 *
 * Author: Aleksander Morgado <aleksander@aleksander.es>
 */

#include <glib.h>
#include <stdio.h>
#include <errno.h>

#include "rmfd-stats.h"
#include "rmfd-syslog.h"

static FILE      *stats_file;
static gchar     *stats_file_path;
static GDateTime *start_system_time;
static time_t     start_time;

#define MAX_LINE_LENGTH 255

/******************************************************************************/
/* Write to syslog */

static void
write_syslog_record (const gchar *from_timestamp,
                     const gchar *to_timestamp,
                     gulong       duration,
                     guint64      rx_bytes,
                     guint64      tx_bytes)
{
    rmfd_syslog (LOG_INFO,
                 "Connection stats [From: %s] [To: %s] [Duration: %lu] "
                 "[RX: %" G_GUINT64_FORMAT "] [TX: %" G_GUINT64_FORMAT "]",
                 from_timestamp,
                 to_timestamp,
                 duration,
                 rx_bytes,
                 tx_bytes);
}

/******************************************************************************/
/* Write to tmp stats file */

static void
write_record (gchar      record_type,
              GDateTime *first_system_time,
              time_t     first_time,
              GDateTime *second_system_time,
              time_t     second_time,
              guint64    rx_bytes,
              guint64    tx_bytes)
{

    gchar line[MAX_LINE_LENGTH + 1];
    gchar *first_system_time_str;
    gchar *second_system_time_str;

    g_assert (record_type == 'S' || record_type == 'P' || record_type == 'F');

    /* Bail out if stats not enabled */
    if (!stats_file)
        return;

    first_system_time_str  = first_system_time  ? g_date_time_format (first_system_time, "%F %T")  : g_strdup ("N/A");
    second_system_time_str = second_system_time ? g_date_time_format (second_system_time, "%F %T") : g_strdup ("N/A");

    /* We'll cap the max line length to a known value by default, just in case */
    g_snprintf (line, MAX_LINE_LENGTH, "%c\t%s\t%s\t%lu\t%" G_GUINT64_FORMAT "\t%" G_GUINT64_FORMAT "\n",
                record_type,
                first_system_time_str,
                second_system_time_str,
                (gulong) (second_time > first_time ? (second_time - first_time) : 0),
                rx_bytes,
                tx_bytes);

    if (fprintf (stats_file, "%s", line) < 0)
        g_warning ("error: cannot write to stats file: %s", g_strerror (ferror (stats_file)));
    else
        fflush (stats_file);

    g_free (first_system_time_str);
    g_free (second_system_time_str);
}

/******************************************************************************/

void
rmfd_stats_start (GDateTime *system_time)
{
    /* Open the file only when started */
    errno = 0;
    if (!(stats_file = fopen (stats_file_path, "w"))) {
        g_warning ("error: cannot open stats file: %s", g_strerror (errno));
        return;
    }

    /* Keep track of when this was started */
    if (start_system_time)
        g_date_time_unref (start_system_time);
    start_system_time = system_time ? g_date_time_ref (system_time) : NULL;
    start_time = time (NULL);

    write_record ('S', start_system_time, start_time, start_system_time, start_time, 0, 0);
}

void
rmfd_stats_tmp (GDateTime *tmp_system_time,
                guint64    rx_bytes,
                guint64    tx_bytes)
{
    write_record ('P', start_system_time, start_time, tmp_system_time, time (NULL), rx_bytes, tx_bytes);
}

void
rmfd_stats_stop (GDateTime *stop_system_time,
                 guint64    rx_bytes,
                 guint64    tx_bytes)
{
    /* If for any reason stop is called multiple times, don't write multiple final records */
    if (!start_system_time)
        return;

    write_record ('F', start_system_time, start_time, stop_system_time, time (NULL), rx_bytes, tx_bytes);

    /* Syslog writing */
    {
        gchar *from_str;
        gchar *to_str;
        time_t stop_time;

        from_str  = start_system_time  ? g_date_time_format (start_system_time, "%F %T") : g_strdup ("N/A");
        to_str    = stop_system_time   ? g_date_time_format (stop_system_time, "%F %T")  : g_strdup ("N/A");
        stop_time = time (NULL);

        g_debug ("writing stats to syslog...");
        write_syslog_record (from_str,
                             to_str,
                             (stop_time > start_time ? (stop_time - start_time) : 0),
                             rx_bytes,
                             tx_bytes);

        g_free (from_str);
        g_free (to_str);
    }

    /* Cleanup start time */
    if (start_system_time)
        g_date_time_unref (start_system_time);
    start_system_time = NULL;
    start_time = 0;

    if (stats_file) {
        fclose (stats_file);
        stats_file = NULL;
    }

    /* Once written to syslog, remove the file */
    g_debug ("removing stats file...");
    g_unlink (stats_file_path);
}

void
rmfd_stats_setup (const gchar *path)
{
    g_assert (!stats_file);
    g_assert (!stats_file_path);

    stats_file_path = g_strdup (path);
}

void
rmfd_stats_teardown (void)
{
    if (start_system_time) {
        g_date_time_unref (start_system_time);
        start_system_time = NULL;
    }

    if (stats_file) {
        fclose (stats_file);
        stats_file = NULL;
    }

    g_free (stats_file_path);
    stats_file_path = NULL;
}