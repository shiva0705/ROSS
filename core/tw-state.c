#include <ross.h>
#include <assert.h>
#include "lz4.h"

/**
 * Make a snapshot of the LP state and store it into the delta buffer
 */
void
tw_snapshot(tw_lp *lp, size_t state_sz)
{
    memcpy(lp->pe->delta_buffer[0], lp->cur_state, state_sz);
}

/**
 * Create the delta from the current state and the snapshot.
 * Compress it.
 * @return The size of the compressed data placed in delta_buffer[1].
 */
long
tw_snapshot_delta(tw_lp *lp, size_t state_sz)
{
    long i;
    tw_clock start;
    int ret_size = 0;
    unsigned char *current_state = lp->cur_state;
    unsigned char *snapshot = lp->pe->delta_buffer[0];

    for (i = 0; i < state_sz; i++) {
        snapshot[i] = current_state[i] - snapshot[i];
    }

    start = tw_clock_read();
    ret_size = LZ4_compress((char*)snapshot, (char*)lp->pe->delta_buffer[1], state_sz);
    g_tw_pe[0]->stats.s_lz4 += (tw_clock_read() - start);
    if (ret_size < 0) {
        tw_error(TW_LOC, "LZ4_compress error");
    }

    start = tw_clock_read();
    lp->pe->cur_event->delta_buddy = buddy_alloc(ret_size);
    g_tw_pe[0]->stats.s_buddy += (tw_clock_read() - start);
    assert(lp->pe->cur_event->delta_buddy);
    lp->pe->cur_event->delta_size = ret_size;
    memcpy(lp->pe->cur_event->delta_buddy, lp->pe->delta_buffer[1], ret_size);

    return ret_size;
}

/**
 * Restore the state of lp to the (decompressed) data held in buffer
 */
void
tw_snapshot_restore(tw_lp *lp, size_t state_sz, void *buffer, size_t delta_size)
{
    tw_clock start = tw_clock_read();
    int ret = LZ4_decompress_safe(buffer, lp->cur_state, delta_size, state_sz);
    g_tw_pe[0]->stats.s_lz4 += (tw_clock_read() - start);
    if (ret < 0) {
        tw_error(TW_LOC, "LZ4_decompress_fast error");
    }
}
