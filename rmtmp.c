/*
 * Copyright (c) 2013 Kyle Isom <kyle@tyrfingr.is>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * ---------------------------------------------------------------------
 */

#include <sys/types.h>
#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>


#define TMP_DIR "/tmp"
#ifndef PATHMAX
#define PATHMAX 1024
#endif


static int      verbose = 0;

/*
 * remove_tmp contains all the actual search-and-destroy code.
 */
static int
remove_tmp(const char *tmpdir, const char *prefix)
{
	char            target[PATHMAX];
	size_t          matched, prefixlen, removed;
	int             status = -1;
	DIR            *tmp = NULL;
	struct dirent  *entry = NULL;

	prefixlen = strnlen(prefix, PATHMAX);
	tmp = opendir(tmpdir);
	matched = removed = 0;

	if (NULL == tmp) {
		err(EX_OSERR, "couldn't open %s", tmpdir);
	}
	while (NULL != (entry = readdir(tmp))) {
		if (entry->d_type != DT_REG)
			continue;
		else if (strncmp(entry->d_name, prefix, prefixlen) != 0)
			continue;
		snprintf(target, PATHMAX, "%s/%s", TMP_DIR, entry->d_name);
		matched++;
		status = unlink(target);
		if (status != 0)
			warn("unlink %s", target);
		else
			removed++;
		if (verbose && removed > 0 && (removed % 1000) == 0)
			printf("removed %lu files\n",
			       (long unsigned) removed);
	}
	printf("removed %lu files\n", (long unsigned) removed);
	if (matched == removed)
		status = 0;
	return status;
}

/*
 * rmtmp searches /tmp for a temporary files created with a certain prefix,
 * for example via mkstemp(3) and removes them. It is designed to clean up
 * the temporary file directory in the event a large number of files are
 * dumped there.
 */
int
main(int argc, char *argv[])
{
	char           *tmpdir = NULL;
	int             opt;

	if (argc == 1) {
		fprintf(stderr, "no prefix given\n");
		return EXIT_FAILURE;
	}
	while ((opt = getopt(argc, (char *const *) argv, "d:v")) != -1) {
		switch (opt) {
		case 'd':
			tmpdir = optarg;
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			/* NOT REACHED */
			fprintf(stderr, "invalid argument!\n");
			return EXIT_FAILURE;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 1)
		fprintf(stderr, "warning: multiple prefixes selected, "
			"but only the first will be used.\n");

	if (NULL == tmpdir)
		tmpdir = (char *) TMP_DIR;

	if (remove_tmp(tmpdir, argv[0]) != 0) {
		fprintf(stderr, "failed to remove some files.\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
