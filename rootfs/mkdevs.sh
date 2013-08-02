#!/bin/sh
#
# makedev.sh - creates device files for a busybox boot floppy image
mkdir -p dev

cd dev 

# miscellaneous one-of-a-kind stuff
mknod console c 5 1
mknod full c 1 7
mknod kmem c 1 2
mknod mem c 1 1
mknod null c 1 3
mknod port c 1 4
mknod random c 1 8
mknod urandom c 1 9
mknod zero c 1 5
mknod ttyS0 c 4 64
mknod ttyS1 c 4 65
ln -s /proc/kcore core

# IDE HD devs
# note: not going to bother creating all concievable partitions; you can do
# that yourself as you need 'em.
mknod hda b 3 0
mknod hdb b 3 64
mknod hdc b 22 0
mknod hdd b 22 64

# loop devs
for i in `seq 0 7`; do
	mknod loop$i b 7 $i
done

# ram devs
for i in `seq 0 9`; do
	mknod ram$i b 1 $i
done
ln -s ram1 ram

# ttys
mknod tty c 5 0
for i in `seq 0 9`; do
	mknod tty$i c 4 $i
done

# virtual console screen devs
for i in `seq 0 9`; do
	mknod vcs$i b 7 $i
done
ln -s vcs0 vcs

# virtual console screen w/ attributes devs
for i in `seq 0 9`; do
	mknod vcsa$i b 7 $((128 + i))
done
ln -s vcsa0 vcsa

mknod mtdblock0 b 31 0
mknod mtdblock1 b 31 1
mknod mtdblock2 b 31 2
mknod mtdblock3 b 31 3
mknod mtd0 c 90 0
mknod mtd0ro c 90 1
mknod mtd1 c 90 2
mknod mtd1ro c 90 3
mknod mtd2 c 90 4
mknod mtd2ro c 90 5
mknod mtd3 c 90 6
mknod mtd3ro c 90 7

mknod i2c-0 c 89 0
mknod watchdog c 10 130

