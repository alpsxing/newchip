/* tools/mkbootimg/mkbootimg.c
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "common.h"



typedef struct {
	unsigned int entry; 	//where to place 
	unsigned int size;	 //size of binary 
	unsigned int loader_cksum; //chsum of binary
}uart_header;


static void *load_file(const char *fn, unsigned *_sz)
{
    char *data;
    int sz;
    int fd;

    data = 0;
    fd = open(fn, O_RDONLY);
    if(fd < 0) return 0;

    sz = lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;

    data = (char*) malloc(sz);
    if(data == 0) goto oops;

    if(read(fd, data, sz) != sz) goto oops;
    close(fd);

    if(_sz) *_sz = sz;
    return data;

oops:
    close(fd);
    if(data != 0) free(data);
    return 0;
}

int usage(void)
{
    fprintf(stderr,"usage: mk_crc\n"
            "       <filename>\n"
            "       [ --entry <address> ]\n"
            "       -o|--output <filename>\n"
            );
    return 1;
}


/* XXX: maximum page size is 16KB */
static unsigned char padding[16384] = { 0, };

int write_padding(int fd, unsigned pagesize, unsigned itemsize)
{
    unsigned pagemask = pagesize - 1;
    unsigned count;

    if((itemsize & pagemask) == 0) {
        return 0;
    }

    count = pagesize - (itemsize & pagemask);

    if(write(fd, padding, count) != count) {
        return -1;
    } else {
        return 0;
    }
}

int main(int argc, char **argv)
{
    uart_header hdr;

    unsigned int base           = 0x80000800;
    char *inf;
    char *of;
    void *inf_data;
    int fd;
    char cmd_dl[2], cmd_bl[2];

    argc--;
    argv++;

    memset(&hdr, 0, sizeof(hdr));

    if(argc < 1)
	return usage();
    inf = argv[0];

    argc--;
    argv++;
    while(argc > 0){
        char *arg = argv[0];
        char *val = argv[1];
        if(argc < 2) {
            return usage();
        }
        argc -= 2;
        argv += 2;
        if(!strcmp(arg, "--output") || !strcmp(arg, "-o")) {
            of = val;
        } else if(!strcmp(arg, "--entry")) {
            base = strtoul(val, 0, 16);
        } else {
            return usage();
        }
    }
    cmd_dl[0] = 'D';
    cmd_dl[1] = 'L';

    cmd_bl[0] = 'B';
    cmd_bl[1] = 'L';

    hdr.entry = base;

    if(of == 0) {
        fprintf(stderr,"error: no output filename specified\n");
        return usage();
    }

    if(inf == 0) {
        fprintf(stderr,"error: no input filename specified\n");
        return usage();
    }

    inf_data = load_file(inf, &hdr.size);
    if(inf_data == 0) {
        fprintf(stderr,"error: could not load file '%s'\n", inf);
        return 1;
    }

    hdr.loader_cksum = checksum32(inf_data, hdr.size);

    check_header(&hdr);
 
    fd = open(of, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(fd < 0) {
        fprintf(stderr,"error: could not create '%s'\n", of);
        return 1;
    }

    if(write(fd, cmd_dl, sizeof(cmd_dl)) != sizeof(cmd_dl)) goto fail;

    if(write(fd, &hdr, sizeof(hdr)) != sizeof(hdr)) goto fail;

    if(write(fd, inf_data, hdr.size) != hdr.size) goto fail;

    if(write(fd, cmd_bl, sizeof(cmd_bl)) != sizeof(cmd_bl)) goto fail;

    if(write(fd, &base, sizeof(base)) != sizeof(base)) goto fail;

    close(fd);
    return 0;

fail:
    unlink(of);
    close(fd);
    fprintf(stderr,"error: failed writing '%s': %s\n", of, strerror(errno));
    return 1;
}
