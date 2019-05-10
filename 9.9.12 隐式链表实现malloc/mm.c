//
// Created by ligand on 2019-04-28.
//

/**
 * 基于隐式链表的 malloc 和 free 实现
 */

#include <unistd.h>

#define WSIZE    4          // 字长
#define DSIZE   8          // 双字
#define CHUNKSIZE (1<<12) // 堆初始化大小以及每次扩张的大小

#define MAX(x, y) ((x) > (y) ? (x) : (y))

// size + 是否分配标记打包
#define PACK(size, alloc) (size | (alloc))

// 读写一个字从/到地址 p
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

// 块头中读块大小
#define GET_SIZE(p) (GET(p) & ~0x7)
// 读分配标记
#define GET_ALLOC(p) (GET(p) & 0x1)

// 对块指针bp，给出块头和块尾
#define HDRP(bp)  ((char *)(bp) - WSIZE)
#define FTRP(bp)  ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// 给定块地址 bp，算上一个和下一个块地址
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

// 堆开始指针
static char *heap_listp;
static int init_mark = 0;

static void *coalesce(char *bp) {
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  // case 1，前后都有分配，不合并
  if (prev_alloc && next_alloc) {
    return bp;
  }
    // case 2, 前有分配，后没有分配
  else if (prev_alloc && !next_alloc) {
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    // FTRP 获取块尾使用的是块头中的信息
    PUT(FTRP(bp), PACK(size, 0));
  }
    // case 3，前没有分配，后有分配
  else if (!prev_alloc && next_alloc) {
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }
    // case 4，前后都没有分配
  else {
    size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }
  return bp;
}

// 找到大小大于大于 asize 的空闲块，找不到返回NULL。采用 first fit 策略
static void *find_fit(size_t asize) {
  if (asize == 0)
    return NULL;
  char *bp = NEXT_BLKP(heap_listp);
  // size 为 0 的块表示结尾
  while (GET_SIZE(HDRP(bp)) != 0 && (GET_ALLOC(HDRP(bp)) || GET_SIZE(HDRP(bp)) < asize))
    bp = NEXT_BLKP(bp);
  if (GET_SIZE(HDRP(bp)) == 0)
    return NULL;
  return bp;
}

// 将块 bp 放入一个 asize 的大小的使用。如果 bp 的大小 减去 asize 大于等于最小快，则切割当前块。
static void place(void *bp, size_t asize) {
  size_t block_size;
  size_t really_asize;
  block_size = GET_SIZE(HDRP(bp));
  really_asize = block_size >= ((CHUNKSIZE) + asize) ? asize : block_size;
  PUT(HDRP(bp), PACK(really_asize, 1));
  PUT(FTRP(bp), PACK(really_asize, 1));
  // 有分割情况
  if (really_asize < block_size) {
    PUT(HDRP(NEXT_BLKP(bp)), PACK(block_size - really_asize, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(block_size - really_asize, 0));
  }
}

/* 分配一个新块 */
static void *extend_heap(size_t words) {
  char *bp;
  size_t size;

  // 保证每次分配双字
  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
  if ((long) (bp = sbrk(size)) == -1)
    return NULL;

  // 初始化这个块
  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  // 重置结尾块
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
  // 合并未使用块
  return coalesce(bp);
}

int mm_init(void) {
  if ((heap_listp = sbrk(4 * WSIZE)) == (void *) -1)
    return -1;
  PUT(heap_listp, 0);
  // 序言块
  PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
  PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
  // 结尾块
  PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
  heap_listp += (2 * WSIZE);
  if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
    return -1;
  return 0;
}

void mm_free(void *bp) {
  size_t size = GET_SIZE(HDRP(bp));
  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  coalesce(bp);
}

void *mm_malloc(size_t size) {
  // 扩张之后的大小
  size_t asize;
  char *bp;

  if (size == 0)
    return NULL;

  // 自动初始化
  if (!init_mark) {
    mm_init();
    init_mark = 1;
  }

  // 负载小于双字，则赋值为双字，加头尾，总共四字
  if (size <= DSIZE)
    asize = 2 * DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

  // 找到了合适的
  if ((bp = find_fit(asize)) != NULL) {
    place(bp, asize);
    return bp;
  }

  // 没找到合适的，从新申请新空间
  size_t extendsize = MAX(asize, CHUNKSIZE);
  if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
    return NULL;
  place(bp, asize);
  return bp;
}
