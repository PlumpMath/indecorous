#include "rpc/target.hpp"

#include "coro/shutdown.hpp"
#include "coro/thread.hpp"
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

void target_t::note_send() const {
    if (is_local()) {
        thread_t *t = thread_t::self();
        if (t != nullptr) {
            t->note_local_rpc();
        }
    }
}

void target_t::send_reply(write_message_t &&msg) {
    // Replies do not have to note a send - there should already be a coroutine waiting
    // to receive the reply - if not it gets discarded.
    // TODO: the source IDs in this message are the source of the original message, not the reply
    stream()->write(std::move(msg));
}

target_t::request_params_t target_t::new_request() const {
    return thread_t::self()->hub()->new_request();
}

template <> void target_t::parse_result(read_message_t) { }

remote_target_t::remote_target_t() :
    target_t(), m_stream(-1) { }

stream_t *remote_target_t::stream() {
    return &m_stream;
}

bool remote_target_t::is_local() const {
    return false;
}

} // namespace indecorous
