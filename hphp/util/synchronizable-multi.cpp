/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include "hphp/util/synchronizable-multi.h"
#include "hphp/util/lock.h"
#include "hphp/util/rank.h"

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

SynchronizableMulti::SynchronizableMulti(int size) :
  m_mutex(RankLeaf), m_group(0), m_conds(size), m_cond_list_vec(1) {
  assert(size > 0);
}

inline
bool SynchronizableMulti::waitImpl(
    int id, int q, Priority pri,
    std::optional<std::chrono::nanoseconds> timeout) {
  assert(id >= 0 && id < m_conds.size());
  auto& cond = m_conds[id];

  assert(q >= 0);
  const uint32_t num_lists = m_cond_list_vec.size();
  uint32_t list_index = 0;
  if (num_lists == 2) list_index = q & 1;
  else if (num_lists > 2) list_index = q % num_lists;
  auto& cond_list = m_cond_list_vec[list_index];

  cond_list.push(cond, pri);

  if (timeout) {
    auto status = cond.m_cond.wait_for(m_mutex, *timeout);
    if (status == std::cv_status::timeout) {
      cond.unlink();
      return false;
    }
  } else {
    cond.m_cond.wait(m_mutex);
  }

  return true;
}

void SynchronizableMulti::wait(int id, int q, Priority pri) {
  waitImpl(id, q, pri, std::nullopt);
}

bool SynchronizableMulti::wait(int id, int q, Priority pri,
                               std::chrono::nanoseconds timeout) {
  return waitImpl(id, q, pri, timeout);
}

void SynchronizableMulti::setNumGroups(int num_groups) {
  Lock l(this);
  if (num_groups != m_cond_list_vec.size()) {
    assert(num_groups > m_cond_list_vec.size());
    m_cond_list_vec.resize(num_groups);
  }
}

void SynchronizableMulti::notifySpecific(int id) {
  assert(id >= 0 && id < m_conds.size());
  auto& cond = m_conds[id];
  cond.m_cond.notify_one();
  cond.unlink();
}

void SynchronizableMulti::notify() {
  for (int i = 0, s = m_cond_list_vec.size(); i < s; i++) {
    assert(m_group < s);
    auto& cond_list = m_cond_list_vec[m_group++];
    if (m_group == s) m_group = 0;
    if (!cond_list.empty()) {
      cond_list.front().m_cond.notify_one();
      cond_list.pop_front();
      break;
    }
  }
}

void SynchronizableMulti::notifyAll() {
  for (auto& cond_list : m_cond_list_vec) {
    while (!cond_list.empty()) {
      cond_list.front().m_cond.notify_one();
      cond_list.pop_front();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
}
