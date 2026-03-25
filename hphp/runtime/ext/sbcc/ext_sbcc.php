<?hh
// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

namespace HH\SBCC {

/**
 * Get per-request SBCC (Sandbox ByteCode Cache) counters.
 * Returns a dict with keys: hits, misses, corrupt.
 * Counters reset at the start of each request (or via reset_stats()).
 */
<<__Native>>
function get_stats()[]: dict<string, int>;

/**
 * Reset per-request SBCC counters to zero.
 */
<<__Native>>
function reset_stats()[]: void;

/**
 * Get process-wide (global) SBCC counters from ServiceData.
 * Returns a dict with keys: hits, misses, corrupt, init_errors.
 * These accumulate across all requests for the lifetime of the process.
 */
<<__Native>>
function get_global_stats()[]: dict<string, int>;

} // namespace HH\SBCC
