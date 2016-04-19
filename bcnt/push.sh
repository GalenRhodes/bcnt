#!/bin/bash
#     PROJECT: bcnt
#    FILENAME: push.sh
#         IDE: AppCode
#      AUTHOR: Galen Rhodes
#        DATE: 4/15/16 9:45 AM
# DESCRIPTION: Pushes the source code for this project to the remote clients and then
#              compiles it.
#
# Copyright (c) 2016 Galen Rhodes. All rights reserved.
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

_hosts="odroid@odroid1 odroid@odroid2 odroid@odroid3 grhodes@rpi1"

# Copy the source files to the remote hosts.
for h in ${_hosts}; do
	rsync -avz bcnt.c "${h}:bin/"
done

# Now compile the source files.
_CMD="-Ofast -g0 -Wl,--strip-all -o bin/bcnt bin/bcnt.c"

for h in ${_hosts}; do
	ssh "${h}" "if [ -e /usr/bin/clang ]; then /usr/bin/clang \${CFLAGS} \${LDFLAGS} ${_CMD}; else /usr/bin/gcc \${CFLAGS} \${LDFLAGS} ${_CMD}; fi"
done

# Now test the builds
for h in ${_hosts}; do
	ssh "${h}" "dd if=/dev/zero bs=4M count=512 | bin/bcnt -c6 >/dev/null"
done

exit 0
