#include "requests.h"

#include "field_macro.inl"

namespace {

void encode_new_order_opt_fields(unsigned char * bitfield_start,
                                 const double price,
                                 const char ord_type,
                                 const char time_in_force,
                                 const unsigned max_floor,
                                 const std::string & symbol,
                                 const char capacity,
                                 const std::string & account)
{
    auto * p = bitfield_start + new_order_bitfield_num();
#define FIELD(name, bitfield_num, bit)                    \
    set_opt_field_bit(bitfield_start, bitfield_num, bit); \
    p = encode_field_##name(p, name);
#include "new_order_opt_fields.inl"
}

uint8_t encode_request_type(const RequestType type)
{
    switch (type) {
    case RequestType::New:
        return 0x38;
    case RequestType::NewCross:
        return 0x7A;
    case RequestType::NewCrossMultileg:
        return 0x85;
    }
    return 0;
}

unsigned char * add_request_header(unsigned char * start, unsigned length, const RequestType type, unsigned seq_no)
{
    *start++ = 0xBA;
    *start++ = 0xBA;
    start = encode(start, static_cast<uint16_t>(length));
    start = encode(start, encode_request_type(type));
    *start++ = 0;
    return encode(start, static_cast<uint32_t>(seq_no));
}

char convert_side(const Side side)
{
    switch (side) {
    case Side::Buy: return '1';
    case Side::Sell: return '2';
    }
    return 0;
}

char convert_ord_type(const OrdType ord_type)
{
    switch (ord_type) {
    case OrdType::Market: return '1';
    case OrdType::Limit: return '2';
    case OrdType::Pegged: return 'P';
    }
    return 0;
}

char convert_time_in_force(const TimeInForce time_in_force)
{
    switch (time_in_force) {
    case TimeInForce::Day: return '0';
    case TimeInForce::IOC: return '3';
    case TimeInForce::GTD: return '6';
    }
    return 0;
}

char convert_capacity(const Capacity capacity)
{
    switch (capacity) {
    case Capacity::Agency: return 'A';
    case Capacity::Principal: return 'P';
    case Capacity::RisklessPrincipal: return 'R';
    }
    return 0;
}

char convert_account_type(const AccountType account_type)
{
    switch (account_type) {
    case AccountType::Client: return '1';
    case AccountType::House: return '3';
    }
    return 0;
}

char convert_position(const Position position)
{
    switch (position) {
    case Position::Open: return 'O';
    case Position::Close: return 'C';
    case Position::None: return 'N';
    }
    return 0;
}

std::string convert_legs(const std::vector<Position> & legs)
{
    std::string str;
    str.reserve(legs.size());
    for (const auto p : legs) {
        str += convert_position(p);
    }
    return str;
}

unsigned char * start_order(unsigned char * p,
                            const std::string & cross_id,
                            const Side side,
                            const double price,
                            const double volume,
                            const std::vector<uint8_t> & bitfield,
                            const uint16_t count)
{
    EXPAND_FUNC(FUNCTION(CrossId, cross_id)
                        FUNCTION_STATIC(CrossType)
                                FUNCTION(Side, convert_side(side))
                                        FUNCTION(Price, price)
                                                FUNCTION(OrderQty, static_cast<unsigned int>(volume))
                                                        FUNCTION(Bitfield, bitfield)
                                                                FUNCTION(GroupCount, count))
    return p;
}

unsigned char * encode_base_order(unsigned char * p, const Order & order)
{
    EXPAND_FUNC(FUNCTION(Side, convert_side(order.side))
                        FUNCTION(OrderQty, static_cast<unsigned int>(order.volume))
                                FUNCTION(ClOrdId, order.cl_ord_id)
                                        FUNCTION(Capacity, convert_capacity(order.capacity))
                                                FUNCTION(ClearingFirm, order.clearing_firm)
                                                        FUNCTION(AccountType, convert_account_type(order.account_type)))
    return p;
}

MAKE_FIELD_FUNCTION(BaseOrder, encode_base_order, Order)

unsigned char * encode_order(unsigned char * p, const Order & order)
{
    EXPAND_FUNC(FUNCTION(BaseOrder, order)
                        FUNCTION(AlgorithmicIndicator, order.algorithmic_indicator))
    return p;
}

unsigned char * encode_multileg(unsigned char * p, const ComplexOrder & order)
{
    EXPAND_FUNC(FUNCTION(BaseOrder, order.order)
                        FUNCTION(Legs, convert_legs(order.legs))
                                FUNCTION(AlgorithmicIndicator, order.order.algorithmic_indicator))
    return p;
}

unsigned char * encode_contra(unsigned char * p, const std::vector<Order> & agency)
{
    for (const auto & order : agency) {
        p = encode_order(p, order);
    }
    return p;
}

unsigned char * encode_contra_multileg(unsigned char * p, const std::vector<ComplexOrder> & agency)
{
    for (const auto & order : agency) {
        p = encode_multileg(p, order);
    }
    return p;
}

MAKE_FIELD_FUNCTION(Order, encode_order, Order)
MAKE_FIELD_FUNCTION(OrderMultileg, encode_multileg, ComplexOrder)
MAKE_FIELD_FUNCTION(ContraOrder, encode_contra, std::vector<Order>)
MAKE_FIELD_FUNCTION(ContraOrderMultileg, encode_contra_multileg, std::vector<ComplexOrder>)

} // anonymous namespace

std::array<unsigned char, calculate_size(RequestType::New)> create_new_order_request(const unsigned seq_no,
                                                                                     const std::string & cl_ord_id,
                                                                                     const Side side,
                                                                                     const double volume,
                                                                                     const double price,
                                                                                     const OrdType ord_type,
                                                                                     const TimeInForce time_in_force,
                                                                                     const double max_floor,
                                                                                     const std::string & symbol,
                                                                                     const Capacity capacity,
                                                                                     const std::string & account)
{
    static_assert(calculate_size(RequestType::New) == 78, "Wrong New Order message size");

    std::array<unsigned char, calculate_size(RequestType::New)> msg;
    auto * p = add_request_header(&msg[0], msg.size() - 2, RequestType::New, seq_no);
    p = encode_text(p, cl_ord_id, 20);
    p = encode_char(p, convert_side(side));
    p = encode_binary4(p, static_cast<uint32_t>(volume));
    p = encode(p, static_cast<uint8_t>(new_order_bitfield_num()));
    encode_new_order_opt_fields(p,
                                price,
                                convert_ord_type(ord_type),
                                convert_time_in_force(time_in_force),
                                max_floor,
                                symbol,
                                convert_capacity(capacity),
                                account);
    return msg;
}

std::vector<unsigned char> create_new_order_cross_request(
        unsigned seq_no,
        const std::string & cross_id,
        double price,
        const std::string & symbol,
        const Order & agency_order,
        const std::vector<Order> & contra_orders)
{
    std::vector<unsigned char> msg;
    msg.resize(calculate_size(RequestType::NewCross) + 12 + 32 * (contra_orders.size() + 1));
    auto p = add_request_header(&msg[0], msg.size() - 2, RequestType::NewCross, seq_no);
    p = start_order(p, cross_id, agency_order.side, price, agency_order.volume, {65}, static_cast<uint16_t>(contra_orders.size() + 1));
    EXPAND_FUNC(FUNCTION(Order, agency_order)
                        FUNCTION(ContraOrder, contra_orders)
                                FUNCTION(Symbol, symbol))
    return msg;
}

std::vector<unsigned char> create_new_order_cross_multileg_request(
        unsigned seq_no,
        const std::string & cross_id,
        double price,
        const std::string & symbol,
        const ComplexOrder & agency_order,
        const std::vector<ComplexOrder> & contra_orders)
{
    std::vector<unsigned char> msg;
    msg.resize(calculate_size(RequestType::NewCrossMultileg) + 14 + 44 * (contra_orders.size() + 1));
    auto p = add_request_header(&msg[0], msg.size() - 2, RequestType::NewCrossMultileg, seq_no);
    p = start_order(p, cross_id, agency_order.order.side, price, agency_order.order.volume, {65, 0, 16}, static_cast<uint16_t>(contra_orders.size() + 1));
    EXPAND_FUNC(FUNCTION(OrderMultileg, agency_order)
                        FUNCTION(ContraOrderMultileg, contra_orders)
                                FUNCTION(Symbol, symbol))
    return msg;
}
