/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved	*/

/*
 * Copyright (c) 1980, 1986, 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * Copyright (c) 1996, 1998-1999 by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef	_FSCK_H
#define	_FSCK_H

#ifdef __cplusplus
extern "C" {
#endif

#define		MAXDUP		10	/* limit on dup blks (per inode) */
#define		MAXBAD		10	/* limit on bad blks (per inode) */
#define		MAXBUFSPACE	256*1024	/* maximum space to allocate */
						/* to buffers */
#define		INOBUFSIZE	256*1024	/* size of buffer to read */
						/* inodes in pass1 */
#define	MAXBSIZE	8192	/* maximum allowed block size */
#define	FIRSTAVDP	256

#ifndef BUFSIZ
#define		BUFSIZ 1024
#endif

#ifdef sparc
#define	SWAP16(x) (((x) & 0xff) << 8 | ((x) >> 8) & 0xff)
#define	SWAP32(x) (((x) & 0xff) << 24 | ((x) & 0xff00) << 8 | \
	((x) & 0xff0000) >> 8 | ((x) >> 24) & 0xff)
#define	SWAP64(x) (SWAP32((x) >> 32) & 0xffffffff | SWAP32(x) << 32)
#else
#define	SWAP16(x) (x)
#define	SWAP32(x) (x)
#define	SWAP64(x) (x)
#endif

#define	NOTBUSY 00		/* Not busy when busymarked is set */
#define		USTATE	01		/* inode not allocated */
#define		FSTATE	02		/* inode is file */
#define		DSTATE	03		/* inode is directory */
#define		DFOUND	04		/* directory found during descent */
#define		DCLEAR	05		/* directory is to be cleared */
#define		FCLEAR	06		/* file is to be cleared */
#define		SSTATE	07		/* inode is a shadow */
#define		SCLEAR	010		/* shadow is to be cleared */
#define	ESTATE	011		/* Inode extension */
#define	ECLEAR	012		/* inode extension is to be cleared */
#define	IBUSY	013		/* inode is marked busy by first pass */
#define	LSTATE	014		/* Link tags */

struct dinode {
	int dummy;
};

/*
 * buffer cache structure.
 */
struct bufarea {
	struct bufarea	*b_next;		/* free list queue */
	struct bufarea	*b_prev;		/* free list queue */
	daddr_t	b_bno;
	int	b_size;
	int	b_errs;
	int	b_flags;
	union {
		char	*b_buf;			/* buffer space */
		daddr_t	*b_indir;		/* indirect block */
		struct	fs *b_fs;		/* super block */
		struct	cg *b_cg;		/* cylinder group */
		struct	dinode *b_dinode;	/* inode block */
	} b_un;
	char	b_dirty;
};

#define		B_INUSE 1

#define		MINBUFS		5	/* minimum number of buffers required */

extern struct log_vol_int_desc *lvintp;
extern struct lvid_iu *lviup;
extern struct space_bmap_desc *spacep;

#define		dirty(bp)	(bp)->b_dirty = isdirty = 1
#define		initbarea(bp) \
	(bp)->b_dirty = 0; \
	(bp)->b_bno = (daddr_t)-1; \
	(bp)->b_flags = 0;

enum fixstate {DONTKNOW, NOFIX, FIX};

struct inodesc {
	enum fixstate id_fix;	/* policy on fixing errors */
	int (*id_func)();	/* function to be applied to blocks of inode */
	ino_t id_number;	/* inode number described */
	ino_t id_parent;	/* for DATA nodes, their parent */
	daddr_t id_blkno;	/* current block number being examined */
	int id_numfrags;	/* number of frags contained in block */
	offset_t id_filesize;	/* for DATA nodes, the size of the directory */
	int id_loc;		/* for DATA nodes, current location in dir */
	int id_entryno;		/* for DATA nodes, current entry number */
	struct direct *id_dirp;	/* for DATA nodes, ptr to current entry */
	char *id_name;		/* for DATA nodes, name to find or enter */
	char id_type;		/* type of descriptor, DATA or ADDR */
};
/* file types */
#define		DATA	1
#define		ADDR	2

/*
 * File entry cache structures.
 */
typedef struct fileinfo {
	struct	fileinfo *fe_nexthash;	/* next entry in hash chain */
	uint32_t fe_block;		/* location of this file entry */
	uint16_t fe_len;		/* size of file entry */
	uint16_t fe_lseen;		/* number of links seen */
	uint16_t fe_lcount;		/* count from the file entry */
	uint8_t	 fe_type;		/* type of file entry */
	uint8_t	 fe_state;		/* flag bits */
} fileinfo_t;
extern fileinfo_t *inphead, **inphash, *inpnext, *inplast;
extern long listmax;

#define	FEGROW 512

extern char	*devname;	/* name of device being checked */
extern long	secsize;	/* actual disk sector size */
extern long	fsbsize;	/* file system block size (same as secsize) */
extern char	nflag;		/* assume a no response */
extern char	yflag;		/* assume a yes response */
extern int	debug;		/* output debugging info */
extern int	rflag;		/* check raw file systems */
extern int	fflag;		/* check regardless of clean flag (force) */
extern char	preen;		/* just fix normal inconsistencies */
extern char	mountedfs;	/* checking mounted device */
extern int	exitstat;	/* exit status (set to 8 if 'No' response) */
extern int	fsmodified;	/* 1 => write done to file system */
extern int	fsreadfd;	/* file descriptor for reading file system */
extern int	fswritefd;	/* file descriptor for writing file system */

extern int	iscorrupt;	/* known to be corrupt/inconsistent */
extern int	isdirty;	/* 1 => write pending to file system */

extern char	mountpoint[100]; /* string set to contain mount point */

extern char	*busymap;	/* ptr to primary blk busy map */
extern char	*freemap;	/* ptr to copy of disk map */

extern uint32_t part_start;
extern uint32_t part_len;
extern uint32_t part_bmp_bytes;
extern uint32_t part_bmp_sectors;
extern uint32_t part_bmp_loc;
extern uint32_t rootblock;
extern uint32_t rootlen;
extern uint32_t lvintblock;
extern uint32_t lvintlen;

extern daddr_t	n_blks;		/* number of blocks in use */
extern daddr_t	n_files;	/* number of files in use */
extern daddr_t	n_dirs;		/* number of dirs in use */

/*
 * bit map related macros
 */
#define		bitloc(a, i)	((a)[(i)/NBBY])
#define		setbit(a, i)	((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define		clrbit(a, i)	((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define		isset(a, i)	((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define		isclr(a, i)	(((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

#define		setbmap(blkno)	setbit(blockmap, blkno)
#define		testbmap(blkno)	isset(blockmap, blkno)
#define		clrbmap(blkno)	clrbit(blockmap, blkno)

#define		setbusy(blkno)	setbit(busymap, blkno)
#define		testbusy(blkno)	isset(busymap, blkno)
#define		clrbusy(blkno)	clrbit(busymap, blkno)

#define	fsbtodb(blkno) ((blkno) * (fsbsize / DEV_BSIZE))
#define	dbtofsb(blkno) ((blkno) / (fsbsize / DEV_BSIZE))

#define		STOP	0x01
#define		SKIP	0x02
#define		KEEPON	0x04
#define		ALTERED	0x08
#define		FOUND	0x10

time_t time();
struct dinode *ginode();
struct inoinfo *getinoinfo();
struct fileinfo *cachefile();
ino_t allocino();
int findino();
char *setup();
void markbusy(daddr_t, long);

#ifndef MNTTYPE_UDFS
#define	MNTTYPE_UDFS		"udfs"
#endif

#define	SPACEMAP_OFF 24

#define	FID_LENGTH(fid)    (((sizeof (struct file_id) + \
		(fid)->fid_iulen + (fid)->fid_idlen - 2) + 3) & ~3)

#define	EXTYPE(len) (((len) >> 30) & 3)
#define	EXTLEN(len) ((len) & 0x3fffffff)

/* Integrity descriptor types */
#define	LVI_OPEN 0
#define	LVI_CLOSE 1

#ifdef __cplusplus
}
#endif

#endif	/* _FSCK_H */
