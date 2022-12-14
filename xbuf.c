#include "less.h"
#include "xbuf.h"

/*
 * Initialize an expandable text buffer.
 */
	public void
xbuf_init(xbuf)
	struct xbuffer *xbuf;
{
	xbuf->data = NULL;
	xbuf->size = xbuf->end = 0;
}

	public void
xbuf_deinit(xbuf)
	struct xbuffer *xbuf;
{
	if (xbuf->data != NULL)
		free(xbuf->data);
	xbuf_init(xbuf);
}

	public void
xbuf_reset(xbuf)
	struct xbuffer *xbuf;
{
	xbuf->end = 0;
}

/*
 * Add a byte to an expandable text buffer.
 */
	public void
xbuf_add_byte(xbuf, b)
	struct xbuffer *xbuf;
	unsigned char b;
{
	if (xbuf->end >= xbuf->size)
	{
		unsigned char *data;
		xbuf->size = (xbuf->size == 0) ? 16 : xbuf->size * 2;
		data = (unsigned char *) ecalloc(xbuf->size, sizeof(unsigned char));
		if (xbuf->data != NULL)
		{
			memcpy(data, xbuf->data, xbuf->end);
			free(xbuf->data);
		}
		xbuf->data = data;
	}
	xbuf->data[xbuf->end++] = b;
}

	public void 
xbuf_add_data(xbuf, data, len)
	struct xbuffer *xbuf;
	unsigned char *data;
	int len;
{
	int i;
	for (i = 0;  i < len;  i++)
		xbuf_add_byte(xbuf, data[i]);
}

	public int
xbuf_pop(buf)
	struct xbuffer *buf;
{
	if (buf->end == 0)
		return -1;
	return (int) buf->data[--(buf->end)];
}

	public void
xbuf_set(dst, src)
	struct xbuffer *dst;
	struct xbuffer *src;
{
	xbuf_reset(dst);
	xbuf_add_data(dst, src->data, src->end);
}

	public char *
xbuf_char_data(xbuf)
	struct xbuffer *xbuf;
{
	return (char *)(xbuf->data);
}
