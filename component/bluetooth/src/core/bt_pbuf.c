/******************************************************************************
  * @file           bt_pbuf.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt pbuf source file
******************************************************************************/

#include "bt_pbuf.h"


#define SIZEOF_STRUCT_PBUF        MEM_ALIGN_SIZE(sizeof(struct bt_pbuf_t))
#define PBUF_POOL_BUFSIZE_ALIGNED PBUF_POOL_BUFSIZE


/******************************************************************************
 * func name   : bt_pbuf_alloc
 * para        : layer(IN)	--> pbuf头类型
                 length(IN)	--> 申请的payload长度
                 type(IN)		--> 申请内存方式
 * return      : 返回申请的pbuf
 * description : 根据给定的layer，长度，类型申请一个pbuf
******************************************************************************/
struct bt_pbuf_t *bt_pbuf_alloc(uint8_t layer, uint16_t length, bt_pbuf_type_e type)
{
    struct bt_pbuf_t *p, *q, *r;
    uint16_t offset;
    int32_t rem_len; /* remaining length */

    BT_PBUF_TRACE_DEBUG("bt_pbuf_alloc(length=%d)\n", length);

    /* determine header offset */
    switch (layer)
    {
    case BT_PBUF_TRANSPORT_H4:
		offset = 1;
		break;
    case BT_PBUF_RAW:
        /** 没有字节偏移 */
        offset = 0;
        break;
    default:
        BT_ASSERT("bt_pbuf_alloc: bad bt_pbuf_t layer\n", 0);
        return NULL;
    }

    switch (type)
    {
    case BT_PBUF_POOL:
        /* allocate head of bt_pbuf_t chain into p */
        p = (struct bt_pbuf_t *)bt_memp_malloc(MEMP_BT_PBUF_POOL);

        BT_PBUF_TRACE_DEBUG("bt_pbuf_alloc: allocated bt_pbuf_t %p\n", (void *)p);

        if (p == NULL)
        {
            return NULL;
        }
        p->type = type;
        p->next = NULL;

        /* make the payload pointer point 'offset' bytes into bt_pbuf_t data memory */
        p->payload = MEM_ALIGN((void *)((uint8_t *)p + (SIZEOF_STRUCT_PBUF + offset)));
        BT_ASSERT("bt_pbuf_alloc: bt_pbuf_t p->payload properly aligned\n",
                  ((mem_ptr_t)p->payload % MEM_ALIGNMENT) == 0);
        /* the total length of the bt_pbuf_t chain is the requested size */
        p->tot_len = length;
        /* set the length of the first bt_pbuf_t in the chain */
        p->len = BT_MIN(length, PBUF_POOL_BUFSIZE_ALIGNED - MEM_ALIGN_SIZE(offset));
        BT_ASSERT("check p->payload + p->len does not overflow bt_pbuf_t\n",
                  ((uint8_t*)p->payload + p->len <=
                   (uint8_t*)p + SIZEOF_STRUCT_PBUF + PBUF_POOL_BUFSIZE_ALIGNED));
        BT_ASSERT("PBUF_POOL_BUFSIZE must be bigger than MEM_ALIGNMENT\n",
                  (PBUF_POOL_BUFSIZE_ALIGNED - MEM_ALIGN_SIZE(offset)) > 0 );
        /* set reference count (needed here in case we fail) */
        p->ref = 1;

        /* now allocate the tail of the bt_pbuf_t chain */

        /* remember first bt_pbuf_t for linkage in next iteration */
        r = p;
        /* remaining length to be allocated */
        rem_len = length - p->len;
        /* any remaining pbufs to be allocated? */
        while (rem_len > 0)
        {
            q = (struct bt_pbuf_t *)bt_memp_malloc(MEMP_BT_PBUF_POOL);
            if (q == NULL)
            {
                /* free chain so far allocated */
                bt_pbuf_free(p);
                /* bail out unsuccessfully */
                return NULL;
            }
            q->type = type;
            q->flags = 0;
            q->next = NULL;
            /* make previous bt_pbuf_t point to this bt_pbuf_t */
            r->next = q;
            /* set total length of this bt_pbuf_t and next in chain */
            BT_ASSERT("rem_len < max_uint16_t\n", rem_len < 0xffff);
            q->tot_len = (uint16_t)rem_len;
            /* this bt_pbuf_t length is pool size, unless smaller sized tail */
            q->len = BT_MIN((uint16_t)rem_len, PBUF_POOL_BUFSIZE_ALIGNED);
            q->payload = (void *)((uint8_t *)q + SIZEOF_STRUCT_PBUF);
            BT_ASSERT("bt_pbuf_alloc: bt_pbuf_t q->payload properly aligned\n",
                      ((mem_ptr_t)q->payload % MEM_ALIGNMENT) == 0);
            BT_ASSERT("check p->payload + p->len does not overflow bt_pbuf_t\n",
                      ((uint8_t*)p->payload + p->len <=
                       (uint8_t*)p + SIZEOF_STRUCT_PBUF + PBUF_POOL_BUFSIZE_ALIGNED));
            q->ref = 1;
            /* calculate remaining length to be allocated */
            rem_len -= q->len;
            /* remember this bt_pbuf_t for linkage in next iteration */
            r = q;
        }
        /* end of chain */
        /*r->next = NULL;*/

        break;
    case BT_PBUF_RAM:
    {
        mem_size_t alloc_len = MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF + offset) + MEM_ALIGN_SIZE(length);

        /* bug #50040: Check for integer overflow when calculating alloc_len */
        if (alloc_len < MEM_ALIGN_SIZE(length))
        {
            BT_PBUF_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] alloc_len < MEM_ALIGN_SIZE(length)\n",__FILE__,__FUNCTION__,__LINE__);
            return NULL;
        }

        /* If bt_pbuf_t is to be allocated in RAM, allocate memory for it. */
        p = (struct bt_pbuf_t*)bt_mem_malloc(alloc_len);
    }

    if (p == NULL)
    {
    	BT_PBUF_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] alloc_len < MEM_ALIGN_SIZE(length)\n",__FILE__,__FUNCTION__,__LINE__);
        return NULL;
    }
        /* Set up internal structure of the bt_pbuf_t. */
    p->payload = MEM_ALIGN((void *)((uint8_t *)p + SIZEOF_STRUCT_PBUF + offset));
    p->len = p->tot_len = length;
    p->next = NULL;
    p->type = type;

    BT_ASSERT("bt_pbuf_alloc: bt_pbuf_t->payload properly aligned\n",
              ((mem_ptr_t)p->payload % MEM_ALIGNMENT) == 0);
    break;

    default:
        BT_ASSERT("bt_pbuf_alloc: erroneous type\n", 0);
        return NULL;
    }
    /* set reference count */
    p->ref = 1;
    /* set flags */
    p->flags = 0;
    BT_PBUF_TRACE_DEBUG("bt_pbuf_alloc(length=%d) == %p\n", length, (void *)p);
    return p;
}



/******************************************************************************
 * func name   : bt_pbuf_realloc
 * para        : p(IN)	--> 传入的pbuf
                 new_len(IN)	--> 重新申请的长度
 * return      : VOID
 * description : 将pbuf缩小到指定new_len的长度
******************************************************************************/
void bt_pbuf_realloc(struct bt_pbuf_t *p, uint16_t new_len)
{
    struct bt_pbuf_t *q;
    uint16_t rem_len; /* remaining length */
    int32_t grow;

    BT_ASSERT("bt_pbuf_realloc: p != NULL\n", p != NULL);
    BT_ASSERT("bt_pbuf_realloc: sane p->type\n", p->type == BT_PBUF_POOL ||
              p->type == BT_PBUF_RAM);

    /* desired length larger than current length? */
    if (new_len >= p->tot_len)
    {
        BT_PBUF_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] new_len >= p->tot_len\n",__FILE__,__FUNCTION__,__LINE__);
        /* enlarging not yet supported */
        return;
    }

    /* the bt_pbuf_t chain grows by (new_len - p->tot_len) bytes
     * (which may be negative in case of shrinking) */
    grow = new_len - p->tot_len;

    /* first, step over any pbufs that should remain in the chain */
    rem_len = new_len;
    q = p;
    /* should this bt_pbuf_t be kept? */
    while (rem_len > q->len)
    {
        /* decrease remaining length by bt_pbuf_t length */
        rem_len -= q->len;
        /* decrease total length indicator */
        BT_ASSERT("grow < max_uint16_t\n", grow < 0xffff);
        q->tot_len += (uint16_t)grow;
        /* proceed to next bt_pbuf_t in chain */
        q = q->next;
        BT_ASSERT("bt_pbuf_realloc: q != NULL\n", q != NULL);
    }
    /* we have now reached the new last bt_pbuf_t (in q) */
    /* rem_len == desired length for bt_pbuf_t q */

    /* shrink allocated memory for BT_PBUF_RAM */
    /* (other types merely adjust their length fields */
    if ((q->type == BT_PBUF_RAM) && (rem_len != q->len)
       )
    {
        /* reallocate and adjust the length of the bt_pbuf_t that will be split */
        q = (struct bt_pbuf_t *)bt_mem_trim(q, (uint16_t)((uint8_t *)q->payload - (uint8_t *)q) + rem_len);
        BT_ASSERT("mem_trim returned q == NULL\n", q != NULL);
    }
    /* adjust length fields for new last bt_pbuf_t */
    q->len = rem_len;
    q->tot_len = q->len;

    /* any remaining pbufs in chain? */
    if (q->next != NULL)
    {
        /* free remaining pbufs in chain */
        bt_pbuf_free(q->next);
    }
    /* q is last packet in chain */
    q->next = NULL;

}


static uint8_t pbuf_header_impl(struct bt_pbuf_t *p, int16_t header_size_increment, uint8_t force)
{
    uint16_t type;
    void *payload;
    uint16_t increment_magnitude;

    BT_ASSERT("p != NULL\n", p != NULL);
    if ((header_size_increment == 0) || (p == NULL))
    {
        return 0;
    }

    if (header_size_increment < 0)
    {
        increment_magnitude = (uint16_t)-header_size_increment;
        /* Check that we aren't going to move off the end of the bt_pbuf_t */
        BT_ERROR("increment_magnitude <= p->len\n", (increment_magnitude <= p->len), return 1;);
    }
    else
    {
        increment_magnitude = (uint16_t)header_size_increment;
    }

    type = p->type;
    /* remember current payload pointer */
    payload = p->payload;

    /* bt_pbuf_t types containing payloads? */
    if (type == BT_PBUF_RAM || type == BT_PBUF_POOL)
    {
        /* set new payload pointer */
        p->payload = (uint8_t *)p->payload - header_size_increment;
        /* boundary check fails? */
        if ((uint8_t *)p->payload < (uint8_t *)p + SIZEOF_STRUCT_PBUF)
        {
            BT_PBUF_TRACE_ERROR("bt_pbuf_header: failed as %p < %p (not enough space for new header size)\n",
                                (void *)p->payload, (void *)((uint8_t *)p + SIZEOF_STRUCT_PBUF));


            /* restore old payload pointer */
            p->payload = payload;
            /* bail out unsuccessfully */
            return 1;
        }
        /* bt_pbuf_t types referring to external payloads? */
    }
    else
    {
        /* Unknown type */
        BT_PBUF_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bad bt_pbuf_t type\n",__FILE__,__FUNCTION__,__LINE__);
        return 1;
    }
    /* modify bt_pbuf_t length fields */
    p->len += header_size_increment;
    p->tot_len += header_size_increment;

    BT_PBUF_TRACE_DEBUG("bt_pbuf_header: old %p new %p (%d)\n",
                        (void *)payload, (void *)p->payload, header_size_increment);


    return 0;
}


/******************************************************************************
 * func name   : bt_pbuf_header
 * para        : p(IN)	--> 传入的pbuf
                 header_size_increment(IN)	--> payload偏移大小
                 buffer[4]={1,2,3,4}，架设payload位置在2,那么便宜-1，那么偏移
                 位置的数据变为3，如果在此基础偏移2,那么payload变为1
 * return      : 0成功，非0失败
 * description : 调整payload指针以隐藏或显示有效负载中的标头。
******************************************************************************/
uint8_t bt_pbuf_header(struct bt_pbuf_t *p, int16_t header_size_increment)
{
    return pbuf_header_impl(p, header_size_increment, 0);
}


/******************************************************************************
* func name   : bt_pbuf_free
* para        : p(IN)	--> 传入的pbuf
* return      : 返回释放pbuf节点的个数
* description : 调整payload指针以隐藏或显示有效负载中的标头。
					 * Assuming existing chains a->b->c with the following reference
					 * counts, calling bt_pbuf_free(a) results in:
					 *
					 * 1->2->3 becomes ...1->3
					 * 3->3->3 becomes 2->3->3
					 * 1->1->2 becomes ......1
					 * 2->1->1 becomes 1->1->1
					 * 1->1->1 becomes .......
******************************************************************************/
uint8_t bt_pbuf_free(struct bt_pbuf_t *p)
{
    uint16_t type;
    struct bt_pbuf_t *q;
    uint8_t count;

    if (p == NULL)
    {
        /* if assertions are disabled, proceed with debug output */
        BT_PBUF_TRACE_DEBUG("bt_pbuf_free(p == NULL) was called.\n");

        return 0;
    }
    BT_PBUF_TRACE_DEBUG("bt_pbuf_free(%p)\n", (void *)p);


    BT_ASSERT("bt_pbuf_free: sane type\n",
              p->type == BT_PBUF_RAM || p->type == BT_PBUF_POOL);

    count = 0;
    /* de-allocate all consecutive pbufs from the head of the chain that
     * obtain a zero reference count after decrementing*/
    while (p != NULL)
    {
        uint16_t ref;

        /* Since decrementing ref cannot be guaranteed to be a single machine operation
         * we must protect it. We put the new ref into a local variable to prevent
         * further protection. */
        /* all pbufs in a chain are referenced at least once */
        BT_ASSERT("bt_pbuf_free: p->ref > 0\n", p->ref > 0);
        /* decrease reference count (number of pointers to bt_pbuf_t) */
        ref = --(p->ref);
        /* this bt_pbuf_t is no longer referenced to? */
        if (ref == 0)
        {
            /* remember next bt_pbuf_t in chain for next iteration */
            q = p->next;
            BT_PBUF_TRACE_DEBUG("bt_pbuf_free: deallocating %p\n", (void *)p);
            type = p->type;

            {
                /* is this a bt_pbuf_t from the pool? */
                if (type == BT_PBUF_POOL)
                {
                    bt_memp_free(MEMP_BT_PBUF_POOL, p);
                    /* is this a ROM or RAM referencing bt_pbuf_t? */
                }
                else
                {
                    bt_mem_free(p);
                }
            }
            count++;
            /* proceed to next bt_pbuf_t */
            p = q;
            /* p->ref > 0, this bt_pbuf_t is still referenced to */
            /* (and so the remaining pbufs in chain as well) */
        }
        else
        {
            BT_PBUF_TRACE_DEBUG("bt_pbuf_free: %p has ref %d, ending here.\n", (void *)p, ref);
            /* stop walking through the chain */
            p = NULL;
        }
    }
    /* return number of de-allocated pbufs */
    return count;
}


/******************************************************************************
 * func name   : bt_pbuf_ref
 * para        : p(IN)	--> 传入的pbuf
 * return      : VOID
 * description : 把pbuf的引用节点+1
******************************************************************************/
void bt_pbuf_ref(struct bt_pbuf_t *p)
{
    /* bt_pbuf_t given? */
    if (p != NULL)
    {
        SYS_ARCH_INC(p->ref, 1);
        BT_ASSERT("bt_pbuf_t ref overflow\n", p->ref > 0);
    }
}


 /******************************************************************************
 * func name   : bt_pbuf_cat
 * para        : head(IN)	--> 传入的头部pbuf
 					 tail(IN)	--> 传入的尾部pbuf
 * return      : VOID
 * description : 把两个pbuf链表组合在一起
******************************************************************************/
void bt_pbuf_cat(struct bt_pbuf_t *head, struct bt_pbuf_t *tail)
{
    struct bt_pbuf_t *p;

    BT_ERROR("(h != NULL) && (t != NULL) (programmer violates API)\n",
             ((head != NULL) && (tail != NULL)), return;);

    /* proceed to last bt_pbuf_t of chain */
    for (p = head; p->next != NULL; p = p->next)
    {
        /* add total length of second chain to all totals of first chain */
        p->tot_len += tail->tot_len;
    }
    /* { p is last bt_pbuf_t of first h chain, p->next == NULL } */
    BT_ASSERT("p->tot_len == p->len (of last bt_pbuf_t in chain)\n", p->tot_len == p->len);
    BT_ASSERT("p->next == NULL\n", p->next == NULL);
    /* add total length of second chain to last bt_pbuf_t total of first chain */
    p->tot_len += tail->tot_len;
    /* chain last bt_pbuf_t of head (p) with first of tail (t) */
    p->next = tail;
    /* p->next now references t, but the caller will drop its reference to t,
     * so netto there is no change to the reference count of t.
     */
}


/******************************************************************************
 * func name   : bt_pbuf_chain
 * para        : head(IN)	--> 传入的头部pbuf
 					 tail(IN)	--> 传入的尾部pbuf
 * return      : VOID
 * description : 把两个pbuf链表组合在一起
******************************************************************************/
void bt_pbuf_chain(struct bt_pbuf_t *head, struct bt_pbuf_t *tail)
{
    bt_pbuf_cat(head, tail);
    /* t is now referenced by h */
    bt_pbuf_ref(tail);
    BT_PBUF_TRACE_DEBUG("bt_pbuf_chain: %p references %p\n", (void *)head, (void *)tail);
}


 /******************************************************************************
 * func name   : bt_pbuf_dechain
 * para        : p(IN)	--> 传入的pbuf链表
 * return      : 返回dechain的链表
 * description : 把链表的第一个pbuf从链表中剔除
******************************************************************************/
struct bt_pbuf_t *bt_pbuf_dechain(struct bt_pbuf_t *p)
{
    struct bt_pbuf_t *q;
    uint8_t tail_gone = 1;
    /* tail */
    q = p->next;
    /* bt_pbuf_t has successor in chain? */
    if (q != NULL)
    {
        /* assert tot_len invariant: (p->tot_len == p->len + (p->next? p->next->tot_len: 0) */
        BT_ASSERT("p->tot_len == p->len + q->tot_len\n", q->tot_len == p->tot_len - p->len);
        /* enforce invariant if assertion is disabled */
        q->tot_len = p->tot_len - p->len;
        /* decouple bt_pbuf_t from remainder */
        p->next = NULL;
        /* total length of bt_pbuf_t p is its own length only */
        p->tot_len = p->len;
        /* q is no longer referenced by p, free it */
        BT_PBUF_TRACE_DEBUG("bt_pbuf_dechain: unreferencing %p\n", (void *)q);

        tail_gone = bt_pbuf_free(q);
        if (tail_gone > 0)
        {
            BT_PBUF_TRACE_DEBUG("bt_pbuf_dechain: deallocated %p (as it is no longer referenced)\n", (void *)q);

        }
        /* return remaining tail or NULL if deallocated */
    }
    /* assert tot_len invariant: (p->tot_len == p->len + (p->next? p->next->tot_len: 0) */
    BT_ASSERT("p->tot_len == p->len\n", p->tot_len == p->len);
    return ((tail_gone > 0) ? NULL : q);
}


/******************************************************************************
 * func name   : bt_pbuf_copy
 * para        : p_to(IN)	--> destination of the copy
 					 p_from(IN)	--> source of the copy
 * return      : BT_ERR_OK if bt_pbuf_t was copied
 *         		 BT_ERR_ARG if one of the pbufs is NULL or p_to is not big
 *               enough to hold p_from
 * description : 复制pbuf
******************************************************************************/
err_t bt_pbuf_copy(struct bt_pbuf_t *p_to, const struct bt_pbuf_t *p_from)
{
    uint16_t offset_to=0, offset_from=0, len;

    BT_PBUF_TRACE_DEBUG("bt_pbuf_copy(%p, %p)\n",
                        (const void*)p_to, (const void*)p_from);


    /* is the target big enough to hold the source? */
    BT_ERROR("bt_pbuf_copy: target not big enough to hold source\n", ((p_to != NULL) &&
             (p_from != NULL) && (p_to->tot_len >= p_from->tot_len)), return BT_ERR_ARG;);

    /* iterate through bt_pbuf_t chain */
    do
    {
        /* copy one part of the original chain */
        if ((p_to->len - offset_to) >= (p_from->len - offset_from))
        {
            /* complete current p_from fits into current p_to */
            len = p_from->len - offset_from;
        }
        else
        {
            /* current p_from does not fit into current p_to */
            len = p_to->len - offset_to;
        }
        memcpy((uint8_t*)p_to->payload + offset_to, (uint8_t*)p_from->payload + offset_from, len);
        offset_to += len;
        offset_from += len;
        BT_ASSERT("offset_to <= p_to->len\n", offset_to <= p_to->len);
        BT_ASSERT("offset_from <= p_from->len\n", offset_from <= p_from->len);
        if (offset_from >= p_from->len)
        {
            /* on to next p_from (if any) */
            offset_from = 0;
            p_from = p_from->next;
        }
        if (offset_to == p_to->len)
        {
            /* on to next p_to (if any) */
            offset_to = 0;
            p_to = p_to->next;
            BT_ERROR("p_to != NULL\n", (p_to != NULL) || (p_from == NULL), return BT_ERR_ARG;);
        }

        if ((p_from != NULL) && (p_from->len == p_from->tot_len))
        {
            /* don't copy more than one packet! */
            BT_ERROR("bt_pbuf_copy() does not allow packet queues!\n",
                     (p_from->next == NULL), return BT_ERR_VAL;);
        }
        if ((p_to != NULL) && (p_to->len == p_to->tot_len))
        {
            /* don't copy more than one packet! */
            BT_ERROR("bt_pbuf_copy() does not allow packet queues!\n",
                     (p_to->next == NULL), return BT_ERR_VAL;);
        }
    }
    while (p_from);

    BT_PBUF_TRACE_DEBUG("bt_pbuf_copy: end of chain reached.\n");
    return BT_ERR_OK;
}

/******************************************************************************
 * func name   : bt_pbuf_copy
 * para        : buf(IN)	--> 复制pbuf source
 					 dataptr(OUT)	--> 准备要把pbuf内容复制到哪里
 					 len(IN)		-->		复制的长度
 					 offset(IN)	-->		复制pbuf payload的偏移
 * return      : the number of bytes copied, or 0 on failure
 * description : 复制pbuf内容到给的dataptr
******************************************************************************/
uint16_t bt_pbuf_copy_partial(const struct bt_pbuf_t *buf, void *dataptr, uint16_t len, uint16_t offset)
{
    const struct bt_pbuf_t *p;
    uint16_t left;
    uint16_t buf_copy_len;
    uint16_t copied_total = 0;

    BT_ERROR("bt_pbuf_copy_partial: invalid buf\n", (buf != NULL), return 0;);
    BT_ERROR("bt_pbuf_copy_partial: invalid dataptr\n", (dataptr != NULL), return 0;);

    left = 0;

    if ((buf == NULL) || (dataptr == NULL))
    {
        return 0;
    }

    /* Note some systems use byte copy if dataptr or one of the bt_pbuf_t payload pointers are unaligned. */
    for (p = buf; len != 0 && p != NULL; p = p->next)
    {
        if ((offset != 0) && (offset >= p->len))
        {
            /* don't copy from this buffer -> on to the next */
            offset -= p->len;
        }
        else
        {
            /* copy from this buffer. maybe only partially. */
            buf_copy_len = p->len - offset;
            if (buf_copy_len > len)
            {
                buf_copy_len = len;
            }
            /* copy the necessary parts of the buffer */
            memcpy(&((char*)dataptr)[left], &((char*)p->payload)[offset], buf_copy_len);
            copied_total += buf_copy_len;
            left += buf_copy_len;
            len -= buf_copy_len;
            offset = 0;
        }
    }
    return copied_total;
}


/* Actual implementation of bt_pbuf_skip() but returning const pointer... */
static const struct bt_pbuf_t*pbuf_skip_const(const struct bt_pbuf_t* in, uint16_t in_offset, uint16_t* out_offset)
{
    uint16_t offset_left = in_offset;
    const struct bt_pbuf_t* q = in;

    /* get the correct bt_pbuf_t */
    while ((q != NULL) && (q->len <= offset_left))
    {
        offset_left -= q->len;
        q = q->next;
    }
    if (out_offset != NULL)
    {
        *out_offset = offset_left;
    }
    return q;
}

/**
 * @ingroup bt_pbuf_t
 * Skip a number of bytes at the start of a bt_pbuf_t
 *
 * @param in input bt_pbuf_t
 * @param in_offset offset to skip
 * @param out_offset resulting offset in the returned bt_pbuf_t
 * @return the bt_pbuf_t in the queue where the offset is
 */
struct bt_pbuf_t* bt_pbuf_skip(struct bt_pbuf_t* in, uint16_t in_offset, uint16_t* out_offset)
{
    const struct bt_pbuf_t* out = pbuf_skip_const(in, in_offset, out_offset);
    return BT_CONST_CAST(struct bt_pbuf_t*, out);
}


/******************************************************************************
 * func name   : bt_pbuf_copy
 * para        : buf(OUT)	--> 复制到pbuf
 					 dataptr(IN)	--> 准备要复制的内容
 					 len(IN)		-->		复制的长度
 * return      : BT_ERR_OK if successful, BT_ERR_MEM if the bt_pbuf_t is not big enough
 * description : 复制dataptr内容到给的buf
******************************************************************************/
err_t bt_pbuf_take(struct bt_pbuf_t *buf, const void *dataptr, uint16_t len)
{
    struct bt_pbuf_t *p;
    uint16_t buf_copy_len;
    uint16_t total_copy_len = len;
    uint16_t copied_total = 0;

    BT_ERROR("bt_pbuf_take: invalid buf\n", (buf != NULL), return BT_ERR_ARG;);
    BT_ERROR("bt_pbuf_take: invalid dataptr\n", (dataptr != NULL), return BT_ERR_ARG;);
    BT_ERROR("bt_pbuf_take: buf not large enough\n", (buf->tot_len >= len), return BT_ERR_MEM;);

    if ((buf == NULL) || (dataptr == NULL) || (buf->tot_len < len))
    {
        return BT_ERR_ARG;
    }

    /* Note some systems use byte copy if dataptr or one of the bt_pbuf_t payload pointers are unaligned. */
    for (p = buf; total_copy_len != 0; p = p->next)
    {
        BT_ASSERT("bt_pbuf_take: invalid bt_pbuf_t\n", p != NULL);
        buf_copy_len = total_copy_len;
        if (buf_copy_len > p->len)
        {
            /* this bt_pbuf_t cannot hold all remaining data */
            buf_copy_len = p->len;
        }
        /* copy the necessary parts of the buffer */
        memcpy(p->payload, &((const char*)dataptr)[copied_total], buf_copy_len);
        total_copy_len -= buf_copy_len;
        copied_total += buf_copy_len;
    }
    BT_ASSERT("did not copy all data\n", total_copy_len == 0 && copied_total == len);
    return BT_ERR_OK;
}

/**
 * @ingroup bt_pbuf_t
 * Same as bt_pbuf_take() but puts data at an offset
 *
 * @param buf bt_pbuf_t to fill with data
 * @param dataptr application supplied data buffer
 * @param len length of the application supplied data buffer
 * @param offset offset in bt_pbuf_t where to copy dataptr to
 *
 * @return BT_ERR_OK if successful, BT_ERR_MEM if the bt_pbuf_t is not big enough
 */
err_t bt_pbuf_take_at(struct bt_pbuf_t *buf, const void *dataptr, uint16_t len, uint16_t offset)
{
    uint16_t target_offset;
    struct bt_pbuf_t* q = bt_pbuf_skip(buf, offset, &target_offset);

    /* return requested data if bt_pbuf_t is OK */
    if ((q != NULL) && (q->tot_len >= target_offset + len))
    {
        uint16_t remaining_len = len;
        const uint8_t* src_ptr = (const uint8_t*)dataptr;
        /* copy the part that goes into the first bt_pbuf_t */
        uint16_t first_copy_len = BT_MIN(q->len - target_offset, len);
        memcpy(((uint8_t*)q->payload) + target_offset, dataptr, first_copy_len);
        remaining_len -= first_copy_len;
        src_ptr += first_copy_len;
        if (remaining_len > 0)
        {
            return bt_pbuf_take(q->next, src_ptr, remaining_len);
        }
        return BT_ERR_OK;
    }
    return BT_ERR_MEM;
}



