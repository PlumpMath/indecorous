#include "rpc/target.hpp"

#include "rpc/hub.hpp"

namespace indecorous {

target_t::target_t() :
    target_id(target_id_t::assign()) { }

target_t::~target_t() { }

target_id_t target_t::id() const {
    return target_id;
}

void target_t::wait() {
    stream()->wait();
}

local_target_t::local_target_t(thread_t *thread) :
    target_t(), m_stream(thread) { }

bool local_target_t::handle() {
    read_message_t msg(m_stream.read());
    if (!msg.buffer.has()) { return false; }
    // membership.hub->spawn(std::move(msg)); // TODO
    return true;
}

stream_t *local_target_t::stream() {
    return &m_stream;
}

remote_target_t::remote_target_t() :
    target_t(), m_stream(-1) { }

stream_t *remote_target_t::stream() {
    return &m_stream;
}

} // namespace indecorous
