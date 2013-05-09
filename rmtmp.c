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


#define RMTMP_VERSION	"1.0.2"
#define TMP_DIR "/tmp"
#ifndef PATHMAX
#define PATHMAX 1024
#endif


extern char	*__progname;
static int	 verbose = 0;


/*
 * Print the program's version.
 */
static void
version(void)
{
    printf("%s version %s\n", __progname, RMTMP_VERSION);
}


/*
 * Print a short usage message.
 */
static void
usage(void)
{
	version();
	printf("\nusage: %s [-d tmpdir] [-hv] prefix\n", __progname);
	printf("\t-d tmpdir\tdirectory containing temporary files\n");
	printf("\t-h\t\tprint this usage message and exit\n");
	printf("\t-v\t\tverbose mode\n");
	printf("\t-V\t\tprint the program's version\n\n");
	exit(EX_USAGE);
}


/*
 * remove_tmp contains all the actual search-and-destroy code.
 */
static int
remove_tmp(const char *tmpdir, const char *prefix)
{
	char		 target[PATHMAX];
	size_t		 prefixlen;
	long unsigned	 matched, removed;
	int		 status = -1;
	DIR		*tmp = NULL;
	struct dirent	*entry = NULL;

	prefixlen = strnlen(prefix, PATHMAX);
	tmp = opendir(tmpdir);
	matched = removed = 0;

	if (NULL == tmp)
	    err(EX_OSERR, "couldn't open %s", tmpdir);

	while (NULL != (entry = readdir(tmp))) {
		if (entry->d_type != DT_REG)
			continue;
		if (strncmp(entry->d_name, prefix, prefixlen) != 0)
			continue;
		snprintf(target, sizeof(target), "%s/%s", tmpdir,
		    entry->d_name);
		matched++;

		if (unlink(target) != 0) {
			warn("unlink %s", target);
			continue;
		}

		removed++;
		if (verbose && removed > 0 && (removed % 1000) == 0)
			printf("removed %lu files\n", removed);
	}

	if (closedir(tmp) == -1)
		warn("couldn't close %s", tmpdir);

	printf("removed %lu files\n", removed);
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
	char	*tmpdir = NULL;
	int	 opt;

	if (argc == 1) {
		fprintf(stderr, "no prefix given\n");
		return EXIT_FAILURE;
	}

	while ((opt = getopt(argc, (char *const *) argv, "d:hvV")) != -1) {
		switch (opt) {
		case 'd':
			tmpdir = optarg;
			break;
		case 'h':
			usage();
			/* NOT REACHED */
			break;
		case 'v':
			verbose = 1;
			break;
		case 'V':
			version();
			exit(EX_USAGE);
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

	if (NULL == tmpdir) {
		if ((NULL == (tmpdir = getenv("TMPDIR"))) || (*tmpdir == '\0'))
			tmpdir = (char *)TMP_DIR;
	}

	if (remove_tmp(tmpdir, argv[0]) != 0) {
		fprintf(stderr, "failed to remove some files.\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
