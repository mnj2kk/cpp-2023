#ifndef BOE_NEW_ORDER_CROSS_FIELD_MACRO_INL
#define BOE_NEW_ORDER_CROSS_FIELD_MACRO_INL

#include "field_macro.h"
#include "fields.h"
#include "requests.h"

MAKE_FIELD_FUNCTION_STATIC(CrossType, encode_char, '1')

MAKE_FIELD_FUNCTION(Side, encode_char, char)
MAKE_FIELD_FUNCTION(OrderQty, encode_field_order_qty, unsigned int)
MAKE_FIELD_FUNCTION(ClOrdId, encode_field_cl_ord_id, std::string)
MAKE_FIELD_FUNCTION(Capacity, encode_field_capacity, char)
MAKE_FIELD_FUNCTION(AccountType, encode_char, char)
MAKE_FIELD_FUNCTION(AlgorithmicIndicator, encode_algorithmic_indicator, char)
MAKE_FIELD_FUNCTION(Price, encode_price, double)
MAKE_FIELD_FUNCTION(Bitfield, encode_bitfield, std::vector<uint8_t>)
MAKE_FIELD_FUNCTION(GroupCount, encode, uint16_t)
MAKE_FIELD_FUNCTION(Symbol, encode_field_symbol, std::string)

MAKE_FIELD_FUNCTION_WITH_SIZE(ClearingFirm, encode_text, std::string, 4)
MAKE_FIELD_FUNCTION_WITH_SIZE(Legs, encode_text, std::string, 12)
MAKE_FIELD_FUNCTION_WITH_SIZE(CrossId, encode_text, std::string, 20)

#endif // BOE_NEW_ORDER_CROSS_FIELD_MACRO_INL
