#include "rpc/hub.hpp"

#include <cassert>

#include "rpc/handler.hpp"
#include "rpc/hub_data.hpp"
#include "rpc/target.hpp"

#include "coro/thread.hpp"

namespace indecorous {

message_hub_t::message_hub_t() { }
message_hub_t::~message_hub_t() { }

uint64_t message_hub_t::local_sends_delta() {
    uint64_t res = local_sends;
    local_sends = 0;
    return res;
}

bool message_hub_t::spawn(read_message_t msg) {
    assert(msg.buffer.has());

    auto cb_it = callbacks.find(msg.handler_id);
    if (msg.request_id == request_id_t::noreply()) {
        debugf("handling noreply rpc\n");
        coro_t::spawn(&handler_callback_t::handle_noreply, cb_it->second, std::move(msg));
    } else if (cb_it != callbacks.end()) {
        debugf("handling reply rpc\n");
        coro_t::spawn(&handler_callback_t::handle, cb_it->second, this, std::move(msg));
    } else {
        printf("No extant handler for handler_id (%lu).\n", msg.handler_id.value());
    }
    return true;
}

target_t *message_hub_t::target(target_id_t id) const {
    auto it = targets.find(id);
    return (it == targets.end()) ? nullptr : it->second;
}

void message_hub_t::send_reply(target_id_t target_id, write_message_t &&msg) {
    target_t *t = target(target_id);
    if (t != nullptr) {
        t->stream()->write(std::move(msg));
    } else {
        printf("Cannot find target (%lu) to send reply to.\n", target_id.value());
    }
}

} // namespace indecorous
