#ifndef IO_NET_HPP_
#define IO_NET_HPP_

#include <netinet/in.h>

#include <cstdint>
#include <string>
#include <vector>

#include "rpc/serialize.hpp"

namespace indecorous {

class ip_address_t {
public:
    static ip_address_t loopback();
    static ip_address_t any();

    bool is_loopback() const;
    bool is_any() const;

    static ip_address_t from_sockaddr(sockaddr_in6 *sa);
    void to_sockaddr(sockaddr_in6 *sa) const;

private:
    friend class ip_and_port_t;
    friend class udns_ctx_t;
    //ip_address_t(const in6_addr &addr, uint32_t scope_id);
    in6_addr m_addr;
    uint32_t m_scope_id;

    DECLARE_SERIALIZABLE(ip_address_t, m_addr, m_scope_id);
};

std::vector<ip_address_t> resolve_hostname(const std::string &host);

class ip_and_port_t {
public:
    static ip_and_port_t loopback(uint16_t _port);
    static ip_and_port_t any(uint16_t _port);

    static ip_and_port_t from_sockaddr(sockaddr_in6 *sa);
    void to_sockaddr(sockaddr_in6 *sa) const;

    const ip_address_t &addr() const;
    uint16_t port() const;

private:
    ip_and_port_t(const in6_addr &_addr, uint32_t scope_id, uint16_t _port);

    ip_address_t m_addr;
    uint16_t m_port;

    DECLARE_SERIALIZABLE(ip_and_port_t, m_addr, m_port);
};

// Serialization for in6_addr struct
template <> struct serializer_t<in6_addr> {
    static size_t size(const in6_addr &item);
    static int write(write_message_t *msg, const in6_addr &item);
    static in6_addr read(read_message_t *msg);
};

} // namespace indecorous

#endif // IO_NET_HPP_
