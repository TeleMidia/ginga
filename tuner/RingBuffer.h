#ifndef HAVE_RING_BUFFER__
#define HAVE_RING_BUFFER__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

// Posix Ring Buffer implementation
//

#define report_exceptional_condition() abort ()

struct ring_buffer
{
  void *address;

  unsigned long count_bytes;
  unsigned long write_offset_bytes;
  unsigned long read_offset_bytes;
};

void ring_buffer_create (struct ring_buffer *buffer, unsigned long order);

void ring_buffer_free (struct ring_buffer *buffer);

void *ring_buffer_write_address (struct ring_buffer *buffer);

void ring_buffer_write_advance (struct ring_buffer *buffer, unsigned long count_bytes);

void *ring_buffer_read_address (struct ring_buffer *buffer);

void ring_buffer_read_advance (struct ring_buffer *buffer, unsigned long count_bytes);

unsigned long ring_buffer_count_bytes (struct ring_buffer *buffer);

unsigned long ring_buffer_count_free_bytes (struct ring_buffer *buffer);

void ring_buffer_clear (struct ring_buffer *buffer);


#ifdef __cplusplus
};
#endif

#endif /* HAVE_RING_BUFFER__ */
